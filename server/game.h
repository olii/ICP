#ifndef GAME_H
#define GAME_H
#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "player.h"



class player;

class Game: public boost::enable_shared_from_this<Game>
{
public:
    Game();
    ~Game();
    bool Join( boost::shared_ptr<player> user );
    void Leave( boost::shared_ptr<player> user );
    bool Joined( boost::shared_ptr<player> user );
    bool Full();
    uint32_t GetIndex();

    static uint32_t index;
private:
    uint32_t index_;
    std::set<boost::shared_ptr<player>> players;
    uint maxPlayers;
    //int minForPlay;
};







#endif // GAME_H
