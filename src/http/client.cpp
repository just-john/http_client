
#include "client.hpp"

#include <stdexcept>
#include <string>
#include <istream>
#include <ostream>
#include <memory>
#include <functional>

#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>


#include <openssl/ssl.h>
#include <openssl/x509_vfy.h>

#include <utility/logging.hpp>


namespace web::http
{

class client::Impl
{
private:
    request &                           request_;

    boost::asio::io_service &           io_;

    response_callback const &           response_callback_;

    boost::asio::ssl::context           ssl_context_;

    boost::asio::ssl::stream <
        boost::asio::ip::tcp::socket>   ssl_socket_;

    boost::asio::streambuf              buffer_;

    boost::asio::ip::tcp::resolver      resolver_;

    std::unique_ptr<response>           response_;

    boost::asio::deadline_timer         timeout_timer_;

    unsigned const                      timeout_seconds_;

public:
    Impl(
        request &                                   req,
        boost::asio::io_service &                   io,
        response_callback const &                   response_callback,
        unsigned                                    timeout_seconds,
        boost::asio::ssl::context::method const     client_context_method)
    :
        request_            (req),
        io_                 (io),
        response_callback_  (response_callback),
        ssl_context_        (client_context_method),
        ssl_socket_         (io_, ssl_context_),
        resolver_           (io_),
        response_           (response::create()),
        timeout_timer_      (io_),
        timeout_seconds_    (timeout_seconds)
    {
        STACKTRACE_ME()

        set_handshake_options();

        boost::asio::ip::tcp::resolver::query query(request_.server(), "https");

        restart_timer();

        resolver_.async_resolve(
            query,
            boost::bind(&Impl::on_resolve, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::iterator)
        );
    }

    ~Impl()
    {
        STACKTRACE_ME()
    }

    void set_handshake_options()
    {
        STACKTRACE_ME()

        // SNI
        if (! SSL_set_tlsext_host_name(ssl_socket_.native_handle(),
                request_.server().c_str()))
        {
            boost::system::error_code ec { static_cast<int>(::ERR_get_error()),
                boost::asio::error::get_ssl_category()
            };

            std::stringstream ss_error;
            ss_error << "ssl error: " << ec.message();

            throw(std::runtime_error(ss_error.str()));
        }

        ssl_socket_.set_verify_mode(
            boost::asio::ssl::verify_peer |
            boost::asio::ssl::verify_fail_if_no_peer_cert);

        ssl_context_.set_default_verify_paths();

        ssl_socket_.set_verify_callback(
            boost::bind(&Impl::on_verify_peer_certificate, this, _1, _2));

        // ssl_socket_.set_verify_callback(
        //     boost::asio::ssl::rfc2818_verification(request_.server().c_str()));
    }

    bool on_verify_peer_certificate(
        bool preverified,
        boost::asio::ssl::verify_context & ctx)
    {
        STACKTRACE_ME()

        int8_t subject_name[256];

        X509_STORE_CTX * store_ctx = ctx.native_handle();

        X509 const * cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());

        int32_t const depth = X509_STORE_CTX_get_error_depth(store_ctx);

        logstream << "context depth : " << depth << "\n";


        switch (X509_STORE_CTX_get_error(store_ctx))
        {
        case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
            logstream
                << __func__ << ": x509 error unable to get issuer certificate\n";
            preverified = false;
            break;

        case X509_V_ERR_CERT_NOT_YET_VALID:
        case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
            logstream << __func__ << ": x509 certificate is not yet valid\n";
            preverified = false;
            break;

        case X509_V_ERR_CERT_HAS_EXPIRED:
        case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
            logstream << __func__ << ": x509 certificate has expired\n";
            preverified = false;
            break;

        case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
            logstream << __func__ << ": Self signed x509 certificate in chain\n";
            // accept self signed certs?
            // preverified = true;
            break;

        default:
            break;
        }

        int32_t const name_length = 256;

        X509_NAME_oneline(
            X509_get_subject_name(cert),
            reinterpret_cast<char*>(subject_name), name_length);

        logstream
            << __func__
            << ": verifying "
            << subject_name
            << "\nverification status: "
            << std::boolalpha
            << preverified
            << "\n";

        return preverified;
    }

    void on_resolve(
        boost::system::error_code const &           ec,
        boost::asio::ip::tcp::resolver::iterator    endpoint_iterator)
    {
        STACKTRACE_ME()

        if (! ec)
        {
            boost::asio::async_connect(
                ssl_socket_.lowest_layer(),
                endpoint_iterator,
                boost::bind(&Impl::on_connect, this,
                    boost::asio::placeholders::error));
        }
        else
        {
            logstream
                << __func__ << ":" << ec.message() << "\n";

            disconnect();
        }
    }

    void on_connect(boost::system::error_code const & ec)
    {
        STACKTRACE_ME()

        if (! ec)
        {
            logstream
                << "tls handshake with remote host: " << request_.server() << "\n";

            ssl_socket_.async_handshake(
                boost::asio::ssl::stream_base::client,
                boost::bind(&Impl::on_handshake, this,
                    boost::asio::placeholders::error));
        }
        else
        {
            logstream
                << __func__ << ":" << ec.message() << "\n";

            disconnect();
        }
    }

    void on_handshake(boost::system::error_code const & ec)
    {
        STACKTRACE_ME()

        stop_timer();

        if (! ec)
        {
            // send the request
            restart_timer();

            boost::asio::async_write(
                ssl_socket_,
                request_.buffer(),
                boost::bind(&Impl::on_write, this,
                    boost::asio::placeholders::error));
        }
        else
        {
            logstream
                << __func__ << ":" << ec.message() << "\n";

            disconnect();
        }
    }

    void on_write(boost::system::error_code const & ec)
    {
        STACKTRACE_ME()

        stop_timer();

        if (! ec)
        {
            restart_timer();

            boost::asio::async_read(
                ssl_socket_,
                buffer_,
                boost::bind(&Impl::on_read, this,
                    boost::asio::placeholders::error));
        }
        else
        {
            logstream
                << __func__ << ":" << ec.message() << "\n";

            disconnect();
        }
    }

    void on_read(boost::system::error_code const & ec)
    {
        STACKTRACE_ME()

        stop_timer();

        // copy buffer into response
        response_->consume(buffer_);

        if (! ec)
        {
            restart_timer();

            boost::asio::async_read(
                ssl_socket_,
                buffer_,
                boost::bind(&Impl::on_read, this,
                    boost::asio::placeholders::error));
        }
        else if (
            (ec.category() == boost::asio::error::get_ssl_category()) &&
            (ec.value()    == boost::asio::ssl::error::stream_truncated)
        )
        {
            disconnect();
        }
        else
        {
            logstream
                << __func__ << ":" << ec.message() << "\n";

            disconnect();
        }
    }

    void disconnect()
    {
        STACKTRACE_ME()

        stop_timer();

        ssl_socket_.async_shutdown(
            boost::bind(
                &Impl::on_disconnect, this,
                boost::asio::placeholders::error));
    }

    void on_disconnect(boost::system::error_code const & ec)
    {
        STACKTRACE_ME()

        if (ec)
        {
            if (ec == boost::asio::error::eof)
            {
                logstream << __func__ << ": disconnected\n";
            }
            else
            {
                logstream
                    << __func__ << ":" << ec.message() << "\n";
            }
        }

        // call the response callback
        response_callback_(std::move(response_));
    }

    void restart_timer()
    {
        timeout_timer_.cancel();

        // logstream << __func__ << ": timer restart\n";

        timeout_timer_.expires_from_now(
            boost::posix_time::seconds(timeout_seconds_));

        timeout_timer_.async_wait(boost::bind(&Impl::on_timeout, this, _1));
    }

    void stop_timer()
    {
        timeout_timer_.cancel();
    }

    void on_timeout(boost::system::error_code const & ec)
    {
        if (ec != boost::asio::error::operation_aborted)
        {
            logstream << __func__ << "io operation timeout\n";
            disconnect();
        }
    }
};


client::client(
    request &                                   req,
    boost::asio::io_service &                   io,
    response_callback const &                   response_callback,
    unsigned                                    timeout_seconds,
    boost::asio::ssl::context::method const     client_context_method)
:
    impl_ (std::make_shared<Impl>(
        req, io, response_callback, timeout_seconds, client_context_method))
{
}

}