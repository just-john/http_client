#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <boost/asio/streambuf.hpp>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <ostream>


namespace web::http
{

enum parse_result { not_parsed, parse_succeeded, parse_failed };

class response
{
public:
    using headermap = std::map<std::string, std::string>;

private:
    response(response const &)              = delete;
    response & operator= (response const &) = delete;
    response();

    friend std::ostream & operator << (std::ostream & os, response & r);
    friend std::unique_ptr<response> std::make_unique<response>();


    std::stringstream   response_buffer_;
    headermap           headers_;
    int                 status_code_;
    int                 body_size_;
    parse_result        parse_result_;


    parse_result parse_status_line(std::string const & status_line);

public:
    static std::unique_ptr<response> create();
    response(response &&)               = default;
    response & operator= (response &&)  = default;
    ~response();

    void consume(boost::asio::streambuf & buffer);

    parse_result parse();


    int                 status_code() const;

    headermap const &   headers() const;

    std::string         body() const;

    int                 body_size() const;
};

std::ostream & operator << (std::ostream & os, response & r);

}

#endif