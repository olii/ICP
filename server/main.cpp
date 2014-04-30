#include "server.h"
#include <boost/asio.hpp>
#include <iostream>
#include <iostream>
#include <csignal>
#include "manager.h"

using std::cout;
using std::cerr;
using std::endl;

boost::asio::io_service io_service;


static void sighandler(int signum) 
{ 
    (void)signum;
    io_service.reset();
    io_service.stop();
    Manager::instance().Shutdown();
}

int main(int argc, char *argv[])
{
    if (  argc != 2)
    {
        cout << "USAGE ./server [port]" << endl;
        return 1;
    }

    tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));
    server labyrint( io_service, endpoint );

    //Manager::instance().CreateGame();

    signal(SIGINT, sighandler);
    io_service.run();

    cerr << "Exitting program" << endl;
    return 0;
}
