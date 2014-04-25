#include "game.h"
#include "manager.h"
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <set>
#include <utility>
#include <boost/bind.hpp>

using std::set;

uint32_t Game::index = 0;


Game::Game():index_( Game::index++ ), timer(io_service)
{
    maxPlayers = 4;
    std::cout << "Game [" << index_ <<"] constructed" << std::endl;
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
        std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] couldn't join to the game." << std::endl;
        return false;
    }
    players.insert(user);
    std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] added to the game." << std::endl;
    return true;
}

void Game::Leave( boost::shared_ptr<player> user )
{
    std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] leaving the game." << std::endl;
    players.erase( user );
    RemovePlayerMessage( user );
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

void Game::GameMessage(boost::shared_ptr<player> user, Message message)
{
    RemovePlayerMessage(user);
    messageQue.emplace_back( user, message );
}

bool Game::Full()
{
    std:: cout << players.size() <<  "  " << maxPlayers<< std::endl;
    return ( players.size() >= maxPlayers );
}

uint32_t Game::GetIndex()
{
    return index_;
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
    timer.expires_from_now(boost::posix_time::seconds(1));
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
    }
    std::cout << "Game [" << index_ <<"]: Gameloop ok: Joined Players: " << players.size() << "/"<< maxPlayers << "." << std::endl;
    timer.expires_from_now(boost::posix_time::seconds(1));
    timer.async_wait( boost::bind( &Game::GameLoop, shared_from_this(), boost::asio::placeholders::error ) );
}

void Game::RemovePlayerMessage(boost::shared_ptr<player> user)
{
    messageQue.remove_if([&user](std::pair< boost::shared_ptr<player>, Message> &x)
    {
        return x.first == user;
    });
}
