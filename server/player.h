#ifndef PLAYER_H
#define PLAYER_H

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "game.h"
#include "../shared/serverinfo.h"



using boost::asio::ip::tcp;
using std::cout;
using std::endl;


class player;
class Game;




class player: public boost::enable_shared_from_this<player>
{
public:
    player(boost::asio::io_service& io_service);
    ~player();

    void start();
    void LeaveServerRequest();
    void SendString( std::string str);

    uint32_t GetIndex();

    tcp::socket socket_;

    static uint32_t index;
private:
    uint32_t index_;
    bool shutdown = false;
    boost::shared_ptr < Game > game;

    uint32_t outheader[2];
    uint32_t inheader[2];
    std::string outdata;
    std::vector<char> indata;

    //bool ProcessSimpleCommand(Co);

    void listen(const boost::system::error_code &error);
    void do_write( const boost::system::error_code& error );
    void do_read();
    void handle_write(const boost::system::error_code& error);
    void handle_header(const boost::system::error_code& error, std::size_t bytes_transferred );
    void handle_payload(const boost::system::error_code &error, std::size_t bytes_transferred);

    bool IsInGame();

    void ErrorShutdown();
    void SendServerList();
    void ErrorMessage( std::string );

    typedef std::vector<boost::asio::const_buffer> SerializedData;

    template<class T, typename C>
    SerializedData Serialize( const T&, C code );

};


#endif // PLAYER_H
