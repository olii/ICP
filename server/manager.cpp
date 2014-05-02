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

ServerInfoList Manager::ServerList()
{
    ServerInfoList l;
    for ( auto x: database )
    {
        ServerInfo tmp(x.second->GetIndex(), x.second->GetName(), x.second->GetPlayerCount(),x.second->GetMaxPlayers(), std::string("Dummy Map") );
        l.list.push_back(tmp);
    }
    return l;
}

boost::shared_ptr< Game > Manager::CreateGame(std::string name, int max)
{
    boost::shared_ptr< Game > ptr ( new Game(name, max) );
    AddGame(ptr);
    return ptr;
}

void Manager::DestroyGame(boost::shared_ptr<Game> game)
{
    RemoveGame(game);
    cout << "Manager: Current games = " << database.size() <<endl;
}

void Manager::Shutdown()
{
    for ( auto x: database )
    {
        x.second->Shutdown();
    }
    database.clear();
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




