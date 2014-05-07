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
    this->map = map;
    this->tickF = tick;
    for ( int i = 0; i<playerModel::COUNT;i++ ) player_model.insert(i);

    std::cout << "Game [" << index_ << "  " << this->name << "] constructed" << std::endl;
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
    user->SetModel(pick_model());
    if ( players.size() == 1 )
    {
        Start();
    }
    std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] added to the game with model = " << user->GetModel()<< "." << std::endl;
    user->SendStaticMap(map);
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
    return map.name;
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
        /* TODO ukoncit server bepecne */
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
