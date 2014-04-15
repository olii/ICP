#include "server.h"
#include "boost/asio.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    UNUSED(argc);

    /*
        check if argv[1] is valid port
    */

    boost::asio::io_service io_service;
    tcp::endpoint endpoint(tcp::v4(), atoi(argv[i]));

    server labyrint(io_service, endpoint);

    io_service.run();

    return 0;
}
