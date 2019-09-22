#ifndef NANOVERSE_API_HPP
#define NANOVERSE_API_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <string>
#include <sstream>


namespace nano::api::request
{

namespace pt = boost::property_tree;


std::string account_balance(std::string const & account)
{
    std::stringstream ss;

    try
    {
        pt::ptree request;
        request.put("action", "account_balance");
        request.put("account", account);

        write_json(ss, request);
    }
    catch(boost::property_tree::ptree_error const & e)
    {
        std::cerr << __func__ << ": " << e.what() << "\n";
    }

    return ss.str();
}

}

#endif