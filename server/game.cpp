#include "game.h"
#include "manager.h"
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <set>
#include <utility>

using std::set;

uint32_t Game::index = 0;


Game::Game():index_( Game::index++ )
{
    maxPlayers = 4;
    std::cout << "Game [" << index_ <<"] constructed" << std::endl;
    // Manager::instance().AddGame( shared_from_this() ); nemozem tu
}

Game::~Game()
{
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
    for( auto x: players )
    {
        x->LeaveServerRequest();
    }
    players.clear();
    messageQue.clear();
}

void Game::RemovePlayerMessage(boost::shared_ptr<player> user)
{
    messageQue.remove_if([&user](std::pair< boost::shared_ptr<player>, Message> &x)
    {
        return x.first == user;
    });
}
