#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <memory>
#include <functional>

#include "request.hpp"
#include "response.hpp"


namespace web::http
{

class client
{
private:
    client (client const &) = delete;
    client & operator = (client const &) = delete;

    class Impl;
    std::shared_ptr<Impl> impl_;

public:
    using response_callback =
        std::function<void(std::unique_ptr<response>)>;

    client(
        request &                                   req,
        boost::asio::io_service &                   io,
        response_callback const &                   response_callback,
        unsigned                                    timeout_seconds = 10,
        boost::asio::ssl::context::method const     client_context_method =
            boost::asio::ssl::context::sslv23_client);
};

}

#endif
