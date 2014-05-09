#ifndef SERVER_H
#define SERVER_H
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "player.h"

using boost::asio::ip::tcp;
using std::cout;
using std::endl;


class server
{
public:
    server( boost::asio::io_service& io_service, const tcp::endpoint& endpoint );

private:
    void start_accept();
    void handle_accept( boost::shared_ptr<player> sharedPtr, const boost::system::error_code& error );

    boost::asio::io_service &io_service_;
    tcp::acceptor acceptor_;
};






#endif // SERVER_H
