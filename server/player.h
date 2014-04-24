#ifndef PLAYER_H
#define PLAYER_H

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "message.h"



using boost::asio::ip::tcp;
using std::cout;
using std::endl;


class player;
extern std::map<int, boost::shared_ptr<player>> SKLAD;

class player: public boost::enable_shared_from_this<player>
{
public:
    player(boost::asio::io_service& io_service);
    ~player();
    void start();
    void do_write( const boost::system::error_code& error );
    void do_read();
    void handle_write(const boost::system::error_code& error);
    void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred );
    
    
    boost::asio::deadline_timer *timer;
    static int index;
    tcp::socket socket_;
    
private:
    int index_;
    bool shutdown = false;
    Message buff;
};


#endif // PLAYER_H
