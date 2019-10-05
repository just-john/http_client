#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/streambuf.hpp>

#include <string>
#include <istream>
#include <ostream>
#include <memory>
#include <functional>

#include "request.hpp"
#include "response.hpp"



namespace web::http
{

class client
{
public:
    using response_callback =
        std::function<void(std::unique_ptr<response>)>;

    client(
        request &                                   req,
        boost::asio::io_service &                   io,
        response_callback const &                   response_callback,
        boost::asio::ssl::context::method const     client_context_method =
            boost::asio::ssl::context::sslv23_client);

    ~client();

private:
    request &                                           request_;

    boost::asio::io_service &                           io_;

    response_callback const &                           response_callback_;

    boost::asio::ssl::context                           ssl_context_;

    boost::asio::ssl::stream <
        boost::asio::ip::tcp::socket>                   ssl_socket_;

    boost::asio::streambuf                              buffer_;

    boost::asio::ip::tcp::resolver                      resolver_;

    std::unique_ptr<response>                           response_;


    void set_handshake_options();

    bool on_verify_peer_certificate(
        bool preverified,
        boost::asio::ssl::verify_context & ctx
    );

    void on_resolve(
        boost::system::error_code                   ec,
        boost::asio::ip::tcp::resolver::iterator    endpoint_iterator);

    void on_connect(boost::system::error_code ec);

    void on_handshake(boost::system::error_code ec);

    void on_write(boost::system::error_code ec);

    void on_read(boost::system::error_code ec);

    void disconnect();

    void on_disconnect(boost::system::error_code ec);
};



}

#endif