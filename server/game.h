#ifndef GAME_H
#define GAME_H
#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <list>
#include <utility>
#include "player.h"
#include "message.h"




extern boost::asio::io_service io_service;
class player;

class Game: public boost::enable_shared_from_this<Game>
{
public:
    Game();
    ~Game();
    bool Join( boost::shared_ptr<player> user );
    void Leave( boost::shared_ptr<player> user );
    bool Joined( boost::shared_ptr<player> user );
    void GameMessage(boost::shared_ptr<player> user, Message message);
    bool Full();
    uint32_t GetIndex();
    void Shutdown();

    void Start();
    void GameLoop(const boost::system::error_code& error);

    static uint32_t index;
private:
    uint32_t index_;
    std::set<boost::shared_ptr<player>> players;
    uint maxPlayers;
    std::list< std::pair< boost::shared_ptr<player>, Message>  > messageQue;
    //int minForPlay;
    void Dispatch();

    void RemovePlayerMessage(boost::shared_ptr<player> user);
    boost::asio::deadline_timer timer;
};







#endif // GAME_H
