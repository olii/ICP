#ifndef PLAYER_H
#define PLAYER_H

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <list>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "game.h"
#include "../shared/serverinfo.h"
#include "../shared/map.h"



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
    void SendStaticMap(Map &map);

    uint32_t GetIndex();

    void SetModel(int id);
    int GetModel();

    tcp::socket socket_;

    /*Static indexer*/
    static uint32_t index;


    /* GAME INFO */
    int x, y;
    bool alive;
    int keys;
    int steps;


private:
    /* GAME releated */
    uint32_t index_;
    boost::shared_ptr < Game > game;
    bool shutdown = false;
    int player_model = -1;


    /* Network releated */
    uint32_t outheader[2];
    uint32_t inheader[2];
    std::string outdata;
    std::vector<char> indata;
    std::list< std::vector<char> > dataQue;

    void listen(const boost::system::error_code &error);
    void do_read();
    void handle_write(const boost::system::error_code& error);
    void handle_header(const boost::system::error_code& error, std::size_t bytes_transferred );

    void HandleCreateServer(const boost::system::error_code &error, std::size_t bytes_transferred);
    void HandleCommand(const boost::system::error_code &error, std::size_t bytes_transferred);
    void SendServerList();
    void SendMapList();
    void SendMap(std::string mapname);

    bool IsInGame();

    void ErrorShutdown();
    void ErrorMessage( std::string );

    typedef std::vector<boost::asio::const_buffer> SerializedData;

    void WriteWrapper();
    template<class T, typename C>
    SerializedData Serialize( const T&, C code );
    template <class T>
    void Deserialize( T& );

};


#endif // PLAYER_H
