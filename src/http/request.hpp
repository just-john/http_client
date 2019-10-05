#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <boost/asio/streambuf.hpp>
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

    void construct_request();

public:
    request(
        std::string server,
        std::string resource,
        std::string body        = std::string(),
        method      req_method  = GET,
        protocol    proto       = HTTPS,
        int         port        = 443);

    boost::asio::streambuf & buffer();

    std::string const & server() const;
};

}

#endif
