#ifndef GAME_H
#define GAME_H
#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <list>
#include <utility>
#include <string>
#include "player.h"
#include "../shared/command.h"
#include "../shared/map.h"
#include <utility>



extern boost::asio::io_service io_service;

typedef std::pair<uint, uint> Point;


class player;

class Game: public boost::enable_shared_from_this<Game>
{
public:
    Game(std::string name, int max , float tick, int timeout, Map map);
    ~Game();
    bool Join( boost::shared_ptr<player> user );
    void Leave( boost::shared_ptr<player> user );
    bool Joined( boost::shared_ptr<player> user );
    void GameMessage(boost::shared_ptr<player> user, Command);
    bool Full();
    void Shutdown();

    void Start();
    void GameLoop(const boost::system::error_code& error);

    uint32_t GetIndex();
    std::string GetName();
    int GetPlayerCount();
    int GetMaxPlayers();
    std::string GetMap();
    float GetTick();
    int GetTimeout();

    static uint32_t index;
private:
    void Dispatch();
    void RemovePlayerMessage(boost::shared_ptr<player> user);

    uint32_t index_;
    std::string name;
    uint maxPlayers;
    boost::posix_time::time_duration tick;
    int timeout;
    Map map_original;
    Map::MapMatrix matrix;

    MapUpdate updatePacket;

    float tickF;

    std::set<int> player_model;
    std::set<boost::shared_ptr<player>> players;
    std::list< std::pair< boost::shared_ptr<player>, Command>  > messageQue;
    std::vector<Point> playerSpawn;
    std::vector<Point> guardSpawn;
    boost::asio::deadline_timer timer;
    int pick_model();
};







#endif // GAME_H
