
#include <iostream>
#include <memory>
#include <stdexcept>

#include "http/client.hpp"
#include "nano/api/nanoverse.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>


namespace web
{

std::string const nano_account =
    "nano_3jwrszth46rk1mu7rmb4rhm54us8yg1gw3ipodftqtikf5yqdyr7471nsg1k";


void test_https_query()
{
    boost::asio::io_service io;

    try
    {
        // endpoint: https://nanoverse.io/api/node
        http::request nano_balance_request (
            "nanoverse.io", "/api/node",
            nano::api::request::account_balance(nano_account),
            http::request::POST);

        http::client c (
            nano_balance_request,
            io,
            [](std::unique_ptr <http::response> && balance_response)
            {
                if (balance_response)
                {
                    balance_response->parse();
                    std::cout << __func__ << "\n" << *balance_response << "\n\n";
                }
                else
                {
                    std::cout << "<insert inappropriate user insult here>\n";
                }
            }
        );

        io.run();
    }
    catch(std::runtime_error const & e)
    {
        std::cerr << e.what() << "\n";
    }
}

}

int main(int argc, char const *argv[])
{
    web::test_https_query();

    return 0;
}