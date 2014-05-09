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

    updatePacket.players.clear();
    updatePacket.guards.clear();
    updatePacket.keys.clear();
    updatePacket.gates.clear();
    updatePacket.treasure = MapItemsInfo(0,0);

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
                    matrix[i][j] = Map::GRASS;
                    break;
                }
                case Map::KEY:
                    matrix[i][j] = Map::GRASS;
                    updatePacket.keys.push_back( MapItemsInfo( i,j, true ) );
                    break;
                case Map::GUARD_SPAWN:
                    guardSpawn.push_back( std::make_pair(i, j) );
                    matrix[i][j] = Map::GRASS;
                    break;
                case Map::FINISH:
                    updatePacket.treasure = MapItemsInfo(i, j);
                    break;
                default:
                    break;
            }

        }
    }
    messageQue.clear();
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
    user->dir = static_cast<playerDirection>(Manager::instance().Random() % 4); // nahodne smer
    /* spawn position genetate */

    user->SetModel(pick_model());
    std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] added to the game with model = " << user->GetModel()<< "." << std::endl;

    user->SendStaticMap(map_original);


    /* find player spawn */
    Point clientSpawn = std::make_pair( 9999,9999 );
    for( auto spawnPos: playerSpawn)
    {
        if ( matrix[spawnPos.first][spawnPos.second] == Map::GRASS )
        {
            clientSpawn = spawnPos;
            break;
        }
    }
    if( clientSpawn.first != 9999 && clientSpawn.second != 9999 )
    {
        user->x = clientSpawn.first;
        user->y = clientSpawn.second;
        user->alive = true;
        user->keys = 0;
        user->steps = 0;
        matrix[clientSpawn.first][clientSpawn.second] = static_cast<Map::StaticTypes>( Map::PLAYER_BASE );
    }
    else
    {
        std::cout << "TODO!!!!!!!!!!!!!!!!!! Spawn not found" << std::endl;
        Leave(user);
        return false;
    }
    /* send initial map update to all */
    Dispatch();

    if ( players.size() == 1 )
    {
        Start();
    }
    return true;
}

void Game::Leave( boost::shared_ptr<player> user )
{
    std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] leaving the game." << std::endl;
    players.erase( user );
    RemovePlayerMessage( user );
    player_model.insert(user->GetModel());


    /* odstranit kluce TODO */

    matrix[user->x][user->y] = Map::GRASS;


    if ( players.empty() )
    {
        std::cout << "Game [" << index_ <<"]: is empty." << std::endl;
        timer.cancel();
        Manager::instance().DestroyGame(shared_from_this());
    }
    else
    {
        Dispatch();
    }
}

bool Game::Joined( boost::shared_ptr<player> user )
{
    return (players.find(user) != players.end());
}

void Game::GameMessage(boost::shared_ptr<player> user, Command c)
{
    std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] got command" << std::endl;
    switch (c.GetType()) {
        case Command::LEFT:
            user->dir = static_cast<playerDirection>((user->dir + 3)%4);
            RemovePlayerMessage(user);
            Dispatch();
            break;

        case Command::RIGHT:
            user->dir = static_cast<playerDirection>((user->dir + 1)%4);
            RemovePlayerMessage(user);
            Dispatch();
            break;

        case Command::STOP:
            RemovePlayerMessage(user);
            break;

        case Command::GO:
            RemovePlayerMessage(user);
            messageQue.emplace_back( user, c );
            break;

        default:
            break;
    }

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
    if( timeout <= 0 )
    {
        SendTextToAll(std::string("Good luck!") );
        boost::system::error_code ignored;
        GameLoop(ignored);
    }
    else
    {
        SendTextToAll(std::string("Game starts in: ") +  std::to_string(timeout) + std::string(" seconds.") );
        timer.expires_from_now(boost::posix_time::seconds(1));
        timer.async_wait( boost::bind( &Game::WaitLoop, shared_from_this(), boost::asio::placeholders::error ) );
        timeout--;
    }
}

void Game::WaitLoop(const boost::system::error_code &error)
{
    if( error )
    {
        if ( error == boost::asio::error::operation_aborted )
        {
            std::cout << "Game [" << index_ <<"]: timer operation aborted" << std::endl;
            return;
        }
        std::cout << "Game [" << index_ <<"]: timer error " << error << std::endl;
    }
    if( timeout <= 0 )
    {
        SendTextToAll(std::string("Good luck!") );
        boost::system::error_code ignored;
        GameLoop(ignored);
    }
    else
    {
        SendTextToAll(std::string("Game starts in: ") +  std::to_string(timeout) + std::string(" seconds.") );
        timer.expires_from_now(boost::posix_time::seconds(1));
        timer.async_wait( boost::bind( &Game::WaitLoop, shared_from_this(), boost::asio::placeholders::error ) );
        timeout--;
    }
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
    std::cout << "Pending messages " << messageQue.size() << std::endl;

    /* prechod hracskymi prikaymi a vykonavanie*/
    std::list< std::pair< boost::shared_ptr<player>, Command>  >::iterator it;
    it = messageQue.begin();
    while (it != messageQue.end())
    {
        Point new_coord = std::make_pair( it->first->x, it->first->y );
        switch (it->first->dir)
        {
            case playerDirection::UP:
                new_coord.first -= 1;
                break;
            case playerDirection::RIGHT:
                new_coord.second += 1;
                break;
            case playerDirection::DOWN:
                new_coord.first += 1;
                break;
            case playerDirection::LEFT:
                new_coord.second -= 1;
                break;
            default:
                break;
        }
        //cout << "previous pos = " << it->first->x << ","<< it->first->y << std::endl;
        //cout << "new pos = " << new_coord.first << ","<< new_coord.second << std::endl;
        try
        {
            if ( matrix.at(new_coord.first).at(new_coord.second) == Map::GRASS )
            {
                matrix[it->first->x][it->first->y] = Map::GRASS;
                matrix[new_coord.first][new_coord.second] = Map::PLAYER_BASE;
                it->first->x = new_coord.first;
                it->first->y = new_coord.second;
                it->first->steps += 1;
            }
            else
            {
                messageQue.erase(it++);
            }
        }
        catch (const std::out_of_range& oor)
        {
            std::cout << "Out of Range error: " << oor.what() << '\n';
            messageQue.erase(it++);
        }
        it++;
    }

    /* do AI*/



    //SendTextToAll( "THIS IS LONG STRING" );
    Dispatch();

    timer.expires_at(timer.expires_at() + tick);
    timer.async_wait( boost::bind( &Game::GameLoop, shared_from_this(), boost::asio::placeholders::error ) );
}

void Game::SendTextToAll(  std::string text)
{
    for( auto x: players )
    {
        x->SendString( text );
    }
}

void Game::Dispatch()
{
    updatePacket.players.clear();
    for ( auto &x: players )
    {
        MapItemsInfo p(x->x, x->y,true, x->dir, x->GetModel(),x->GetIndex(), x->keys, x->steps );
        updatePacket.players.push_back(p);
    }
    for( auto x: players )
    {
        x->SendMapUpdate( updatePacket );
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
