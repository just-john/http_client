#include "request.hpp"

#include <memory>
#include <ostream>


namespace web::http
{

request::request(
    std::string server,
    std::string resource,
    std::string body,
    method      req_method,
    protocol    proto,
    int         port)
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

void request::construct_request()
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

boost::asio::streambuf & request::buffer()
{
    return buffer_;
}

std::string const & request::server() const
{
    return server_;
}

}
