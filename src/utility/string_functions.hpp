#ifndef UTILITY_STRING_FUNCTIONS_HPP
#define UTILITY_STRING_FUNCTIONS_HPP

#include <string>

namespace utility
{

std::string & ltrim(std::string & str);

std::string & rtrim(std::string & str);

std::string & trim(std::string & str);

std::string trim_copy(std::string const & str);

bool is_numeric(std::string const & s);

}

#endif