
#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP


#include <boost/asio/streambuf.hpp>
#include <memory>
#include <string>


namespace web::http
{

class request
{
public:
    enum method     { GET, POST, PUT, DELETE };
    enum protocol   { HTTP, HTTPS };

private:
    std::string const       server_;    // api.coinmarketcap.com
    std::string const       resource_;  // /litecoin?convert=GBP
    std::string const       body_;      //
    method const            method_;    // GET
    int const               protocol_;  // HTTP
    int const               port_;      // 443

    boost::asio::streambuf  buffer_;

    void construct_request()
    {
        std::ostream os (&buffer_);

        switch (method_)
        {
            case GET:       os << "GET ";     break;
            case POST:      os << "POST ";    break;
            case PUT:       os << "PUT ";     break;
            case DELETE:    os << "DELETE ";  break;
            default: break;
        }

        os  << resource_   << " HTTP/1.0\r\n"
            << "Host: "    << server_ << "\r\n";

        if (body_.length() > 0)
        {
            os << "Content-Length: " << body_.length() << "\r\n";
        }

        os  << "Accept: */*\r\n"
            << "Connection: close\r\n\r\n";

        if (body_.length() > 0)
        {
            os << body_;
        }
    }

public:
    request(
        std::string server,
        std::string resource,
        std::string body        = std::string(),
        method      req_method  = GET,
        protocol    proto       = HTTPS,
        int         port        = 443)
    :
        server_     (std::move(server)),
        resource_   (std::move(resource)),
        body_       (std::move(body)),
        method_     (req_method),
        protocol_   (proto),
        port_       (port)
    {
        construct_request();
    }

    boost::asio::streambuf & buffer()
    {
        return buffer_;
    }

    std::string const & server() const
    {
        return server_;
    }
};

}

#endif