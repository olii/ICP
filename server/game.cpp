#include "game.h"
#include "manager.h"
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <iostream>
#include <set>
#include <utility>
#include <boost/bind.hpp>
#include <random>


using std::set;

uint32_t Game::index = 0;


Game::Game(std::string name, int max, float tick, int timeout, Map map ):index_( Game::index++ ), timer(io_service)
{
    this->name = name;
    this->maxPlayers = max;
    this->tick = boost::posix_time::seconds(int(tick)) + boost::posix_time::millisec( (tick - int(tick))*1000 );
    this->timeout = timeout;
    this->map_original = map;
    this->tickF = tick;
    for ( int i = 0; i<playerModel::COUNT;i++ ) player_model.insert(i); // dostupne modely

    std::cout << "Game [" << index_ << "  " << this->name << "] constructed" << std::endl;
    matrix = map.items;


    /*
    std::vector< MapItemsInfo > players;
    std::vector< MapItemsInfo > guards;
    std::vector< MapItemsInfo > keys;
    std::vector< MapItemsInfo > gates;
    MapItemsInfo treasure;
    */

    //    enum StaticTypes {GRASS = 0, WALL = 1, KEY = 2, PLAYER_SPAWN = 3, GUARD_SPAWN = 4, GATE = 5, FINISH = 6, PLAYER_BASE = 99, PLAYER_A = 100, PLAYER_B, PLAYER_C, PLAYER_D};
    for( uint i =0; i<matrix.size();++i )
    {
        for( uint j=0; j<matrix[i].size();++j)
        {
            switch (matrix[i][j]) {
                case Map::GRASS:
                case Map::WALL:
                    break;
                case Map::GATE:
                    updatePacket.guards.push_back( MapItemsInfo( i,j, true ) );
                    break;
                case Map::PLAYER_SPAWN:
                {
                    Point tmp = std::make_pair(uint(i), uint(j));
                    playerSpawn.push_back( tmp );
                    break;
                }
                case Map::KEY:
                    matrix[i][j] = Map::GRASS;
                    updatePacket.keys.push_back( MapItemsInfo( i,j, true ) );
                    break;
                case Map::GUARD_SPAWN:
                    guardSpawn.push_back( std::make_pair(i, j) );
                    break;
                case Map::FINISH:
                    updatePacket.treasure = MapItemsInfo(i, j);
                    break;
                default:
                    break;
            }

        }
    }
}

Game::~Game()
{
    timer.cancel();
    players.clear();
    messageQue.clear();
    std::cout << "Game [" << index_ <<"] destructed, message que="<< messageQue.size() << std::endl;
}

bool Game::Join( boost::shared_ptr<player> user )
{
    if (players.size() >= maxPlayers)
    {
        std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] couldn't join to the game. Exceeding maxplayers." << std::endl;
        return false;
    }
    players.insert(user);

    user->steps = 0;
    user->alive = true;
    user->keys = 0;
    /* spawn position genetate */

    user->SetModel(pick_model());
    if ( players.size() == 1 )
    {
        Start();
    }
    std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] added to the game with model = " << user->GetModel()<< "." << std::endl;
    user->SendStaticMap(map_original);
    /* send initial map update */

    return true;
}

void Game::Leave( boost::shared_ptr<player> user )
{
    std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] leaving the game." << std::endl;
    players.erase( user );
    RemovePlayerMessage( user );
    player_model.insert(user->GetModel());
    if ( players.empty() )
    {
        std::cout << "Game [" << index_ <<"]: is empty." << std::endl;
        timer.cancel();
        Manager::instance().DestroyGame(shared_from_this());
    }
}

bool Game::Joined( boost::shared_ptr<player> user )
{
    return (players.find(user) != players.end());
}

void Game::GameMessage(boost::shared_ptr<player> user, Command c)
{
    std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] got command" << std::endl;
    RemovePlayerMessage(user);
    messageQue.emplace_back( user, c );
}

bool Game::Full()
{
    return ( players.size() >= maxPlayers );
}

uint32_t Game::GetIndex()
{
    return index_;
}

std::string Game::GetName()
{
    return name;
}

int Game::GetPlayerCount()
{
    return players.size();
}

int Game::GetMaxPlayers()
{
    return maxPlayers;
}

std::string Game::GetMap()
{
    return map_original.name;
}

float Game::GetTick()
{
    return tickF;
}

int Game::GetTimeout()
{
    return timeout;
}

void Game::Shutdown()
{
    timer.cancel();
    for( auto x: players )
    {
        x->LeaveServerRequest();
    }
    players.clear();
    messageQue.clear();
}

void Game::Start()
{
    timer.expires_from_now(boost::posix_time::seconds(timeout));
    timer.async_wait( boost::bind( &Game::GameLoop, shared_from_this(), boost::asio::placeholders::error ) );
}

void Game::GameLoop(const boost::system::error_code &error)
{
    if( error )
    {
        if ( error == boost::asio::error::operation_aborted )
        {
            std::cout << "Game [" << index_ <<"]: timer operation aborted" << std::endl;
            return;
        }
        std::cout << "Game [" << index_ <<"]: timer error " << error << std::endl;
        /* TODO ukoncit server bepecne -- ASI HOTOVO*/
    }
    std::cout << "Game [" << index_ <<"]: Gameloop ok: Joined Players: " << players.size() << "/"<< maxPlayers << "." << std::endl;
    Dispatch();

    timer.expires_at(timer.expires_at() + tick);
    timer.async_wait( boost::bind( &Game::GameLoop, shared_from_this(), boost::asio::placeholders::error ) );
}

void Game::Dispatch()
{
    for( auto x: players )
    {
        x->SendString( "This is Tick-Tock\r\n" );
    }
}

void Game::RemovePlayerMessage(boost::shared_ptr<player> user)
{
    messageQue.remove_if([&user](std::pair< boost::shared_ptr<player>, Command> &x)
    {
        return x.first == user;
    });
}

int Game::pick_model()
{
    int offset = (Manager::instance().Random())%player_model.size();
    std::cout << "offset = " << offset << std::endl;
    auto it = player_model.begin();
    for (; offset != 0; offset--) it++;

    int retval = *it;
    player_model.erase(retval);
    return retval;
}
