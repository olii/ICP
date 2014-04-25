#include "manager.h"
#include <boost/shared_ptr.hpp>
#include <iostream>
#include "game.h"

using std::cout;
using std::endl;

boost::shared_ptr<Game> Manager::GetGameById(uint32_t id)
{
    if ( database.find( id ) == database.end() )
    {
        return  boost::shared_ptr< Game >();
    }
    return database[id];
}

boost::shared_ptr<Game> Manager::GetJoinableGame()
{
    for( auto x: database )
    {
        if( !( x.second->Full() ) )
        {
            return x.second;
        }
    }
    return boost::shared_ptr<Game>();
}

boost::shared_ptr< Game > Manager::CreateGame()
{
    boost::shared_ptr< Game > ptr ( new Game() );
    AddGame(ptr);
    return ptr;
}

void Manager::DestroyGame(boost::shared_ptr<Game> game)
{
    RemoveGame(game);
    cout << "Manager: games = " << database.size() <<endl;
}

void Manager::AddGame(boost::shared_ptr<Game> newgame)
{
    database[newgame->GetIndex()] = newgame;
}

void Manager::RemoveGame(boost::shared_ptr<Game> oldgame)
{
    database.erase(oldgame->GetIndex());
}

Manager::Manager()
{
    cout << "Creating GameManager Singleton" << endl;
}




