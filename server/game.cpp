#include "game.h"
#include "manager.h"
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <set>

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
    std::cout << "Game [" << index_ <<"] destructed" << std::endl;
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

bool Game::Full()
{
    std:: cout << players.size() <<  "  " << maxPlayers<< std::endl;
    return ( players.size() >= maxPlayers );
}

uint32_t Game::GetIndex()
{
    return index_;
}
