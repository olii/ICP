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
#include "game.h"



using boost::asio::ip::tcp;
using std::cout;
using std::endl;


class player;
class Game;
extern std::map<int, boost::shared_ptr<player>> SKLAD;




class player: public boost::enable_shared_from_this<player>
{
public:
    player(boost::asio::io_service& io_service);
    ~player();
    void start();
    uint32_t GetIndex();
    
    static uint32_t index;
    tcp::socket socket_;
    
private:
    uint32_t index_;
    bool shutdown = false;
    Message buff;
    boost::asio::deadline_timer *timer;
    boost::shared_ptr < Game > game;


    void listen(const boost::system::error_code &error);
    void do_write( const boost::system::error_code& error );
    void do_read();
    void handle_write(const boost::system::error_code& error);
    void handle_header(const boost::system::error_code& error, std::size_t bytes_transferred );
    void handle_payload(const boost::system::error_code &error, std::size_t bytes_transferred);
};


#endif // PLAYER_H
