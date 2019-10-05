#ifndef NANO_API_REQUEST_NANO_REQUESTS_HPP
#define NANO_API_REQUEST_NANO_REQUESTS_HPP

#include <string>


namespace nano::api::request
{

std::string account_balance(std::string const & account);

}

#endif