# progress

[x] cmake: fix the install step, it's broken

[x] fix the SSL_R_SHORT_READ issue

[x] SNI support

[x] provide accessors for the request object

[x] add streamout operator to the response object

[x] add a timeout timer to the http::client

[] Need to be able to handle a chunked encoding response
    (URL to test against?)

[] add an agent string

- testing
    - component level testing
    - valgrind

- move onto implementing websockets


So at the moment, the client only reads until it is disconnected by the remote endpoint.  This is initially fine, most servers will play nice with this.  But it's not particularly polite for a client to behave in this manner.

If the server doesn't close the connection, the client may hold the connection open for multiple minutes before the server times out the connection.

Parsing the response header as it's received - we could gain information like
    i  notification of chunked encoding
    ii a content-length

    [x] simple https client
    [] should support chunked encoding response
    [] should also be able to handle http

    [x] should use sni if available
    [] should use http_parser external library ?
    [] client certificate support


# building boost
    wget https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.gz
    tar -xf boost_1_70_0.tar.gz
    cd boost_1_70_0
    ./bootstrap.sh
    ./b2 -j9

# boost asio http client references

[verification of server cert by client](https://stackoverflow.com/questions/16890339/how-to-verify-client-certificates-using-boostasio-ssl)

[adding SNI support to client](https://www.boost.org/doc/libs/develop/libs/beast/example/http/client/async-ssl/http_client_async_ssl.cpp)

[ssl short_read theory](https://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error)

[ssl short_read solution](https://stackoverflow.com/questions/22575315/how-to-gracefully-shutdown-a-boost-asio-ssl-client)

[lets encrypt](https://letsencrypt.org/)


# building openssl

TODO