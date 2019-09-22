
#include <algorithm>
#include <locale>
#include "string_functions.hpp"


namespace utility
{

std::string & ltrim(std::string & str)
{
    auto it2 = std::find_if(
        str.begin() , str.end(),
        [](char ch)
        {
            return !std::isspace<char>(ch, std::locale::classic());
        }
    );

    str.erase(str.begin() , it2);
    return str;
}

std::string & rtrim(std::string & str)
{
    auto it1 =  std::find_if(
        str.rbegin(), str.rend(),
        [](char ch)
        {
            return ! std::isspace<char>(ch, std::locale::classic());
        }
    );

    str.erase(it1.base(), str.end());
    return str;
}

std::string & trim(std::string & str)
{
    return ltrim(rtrim(str));
}

std::string trim_copy(std::string const & str)
{
    auto s = str;
    return ltrim(rtrim(s));
}

bool is_numeric(std::string const & s)
{
    return (! s.empty() &&
        (s.find_first_not_of("0123456789.-") == std::string::npos)
    );
}

}