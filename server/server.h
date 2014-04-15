#ifndef SERVER_H
#define SERVER_H
#include "boost/asio.hpp"

/*zamedzi warningu unused variable*/
#define UNUSED(x) ((void)x)




class server
{
public:
    server(boost::asio::io_service& io_service, const tcp::endpoint& endpoint);

private:
    boost::asio::io_service &io_service_;
    tcp::acceptor acceptor_;
};






#endif // SERVER_H
