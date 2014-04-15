#include "server.h"
#include <boost/asio.hpp>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char *argv[])
{
    
    if (  argc != 2)
    {
        cout << "USAGE ./server [port]" << endl;
        return 1;
    }

    boost::asio::io_service io_service;
    tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));

    server labyrint( io_service, endpoint );

    io_service.run();

    cerr << "Exitting program" << endl;
    return 0;
}
