
#include "response.hpp"
#include <utility/string_functions.hpp>

#include <algorithm>
#include <iostream>
#include <memory>


namespace web::http
{

response::response() :
    status_code_    (0),
    body_size_      (0),
    parse_result_   (not_parsed)
{
}

std::unique_ptr<response> response::create()
{
    return std::make_unique<response>();
}

response::~response()
{
}

std::ostream & operator << (std::ostream & os, response & r)
{
    os << "http response\nheaders\n";


    std::for_each (
        r.headers_.cbegin(), r.headers_.cend(),

        [&](response::headermap::value_type const & i)
        {
            os << i.first << ": " << i.second << "\n";
        }
    );

    if (r.body_size_ > 0)
    {
        os  << "body:\n"
            << r.response_buffer_.str() << "\n\n";
    }

    return os;
}

void response::consume(boost::asio::streambuf & buffer)
{
    std::ostream os (&buffer);
    response_buffer_ << os.rdbuf();
}

parse_result response::parse_status_line(std::string const & status_line)
{
    // status line should have the format:
    // HTTP/1.1 200 OK\r\n
    std::vector<std::string> tokens;
    std::istringstream is (status_line);
    std::string buf;

    while((is >> buf) && (buf != "\r"))
    {
        tokens.emplace_back(std::move(buf));
        buf = std::string();
    }

    if ((tokens.size() != 3) ||
        (tokens[0].find("HTTP") == std::string::npos))
    {
        return parse_failed;
    }

    if (::utility::is_numeric(tokens[1]))
    {
        status_code_ = std::stoi(tokens[1]);
    }
    else
    {
        return parse_failed;
    }

    return parse_succeeded;
}

parse_result response::parse()
{
    response_buffer_.seekg(0, std::ios::end);
    int const message_length = response_buffer_.tellg();
    response_buffer_.seekg(0, std::ios::beg);

    if (! message_length)
    {
        return parse_failed;
    }

    std::string status_line;
    std::getline(response_buffer_, status_line);

    if (status_line.length())
    {
        parse_result const parse_status_result =
            parse_status_line(status_line);

        if (parse_status_result != parse_succeeded)
        {
            return parse_status_result;
        }
    }
    else
    {
        return parse_failed;
    }

    // read headers
    std::string line;

    while (std::getline(response_buffer_, line) && line != "\r")
    {
        size_t const i = line.find(":");

        if (i != std::string::npos)
        {
            std::string key     = line.substr(0, i);
            std::string value   = utility::trim_copy(line.substr(i+1));

            headers_.insert(std::make_pair(std::move(key), std::move(value)));
        }
        else
        {
            return parse_failed;
        }
    }

    // compute body_size from content remaining in response_buffer_
    response_buffer_.seekg(0, std::ios::end);
    body_size_ = response_buffer_.tellg();
    response_buffer_.seekg(0, std::ios::beg);

    return parse_succeeded;
}


int response::status_code() const
{
    return status_code_;
}

response::headermap const & response::headers() const
{
    return headers_;
}

std::string response::body() const
{
    return (parse_result_ == parse_succeeded) ?
        response_buffer_.str() : std::string();
}

int response::body_size() const
{
    return body_size_;
}


}