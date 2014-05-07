#include "manager.h"
#include <boost/shared_ptr.hpp>
#include <iostream>
#include "game.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <fstream>

#include <boost/archive/text_iarchive.hpp>


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
        ServerInfo tmp(x.second->GetIndex(), x.second->GetName(), x.second->GetPlayerCount(),x.second->GetMaxPlayers(),
                       x.second->GetMap(), x.second->GetTick(), x.second->GetTimeout() );
        l.list.push_back(tmp);
    }
    return l;
}

ServerInfoList Manager::MapList()
{
    ServerInfoList l;
    for ( auto x: maplist )
    {
        ServerInfo tmp(0, "", 0,0, x.second.name, 0, 0 );
        l.list.push_back(tmp);
    }
    return l;
}

Map Manager::GetMapByName(std::string name)
{
    if (maplist.find(name) != maplist.end() ) // has key
    {
        return maplist[name];
    }
    Map dummy;
    dummy.invalidate();
    return dummy;
}

void Manager::LoadMapList()
{
    boost::filesystem::path targetDir("./maps");
    boost::filesystem::directory_iterator it(targetDir), eod;

    BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(it, eod))
    {
        if(is_regular_file(p) && p.extension().compare( std::string("imp")) )
        {
            Map m;
            try
            {
                std::ifstream ifs(p.generic_string());
                boost::archive::text_iarchive ia(ifs);
                ia >> m;
                m.name = p.stem().generic_string();

                maplist[ p.stem().generic_string() ] = m;
            }
            catch( boost::archive::archive_exception &e )
            {
                std::cerr << "Warning: " << e.what() <<std::endl;
            }
        }
    }
    cout << "Manager: Loaded " << maplist.size() << " map(s)." << endl;
}

bool Manager::ValidateNewGame(std::string &name, int max, float tick, int timeout, std::string &map )
{
    if (name == std::string(""))
        return false;
    if ( max <1 || max > 4 )
        return false;
    if ( tick <0.5 || tick > 5 )
        return false;
    if ( timeout <0 || timeout > ( 10*60 ) ) // max 10 minut
        return false;
    if ( maplist.find( map ) == maplist.end() )
    {
        return false;
    }
    return true;
}

boost::shared_ptr< Game > Manager::CreateGame(std::string name, int max, float tick, int timeout, std::string map )
{

    if( !ValidateNewGame(name, max, tick, timeout, map) )
    {
        return boost::shared_ptr< Game > ();
    }

    boost::shared_ptr< Game > ptr ( new Game(name, max, tick, timeout, maplist[map]) );
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

int Manager::Random()
{
    return distribution(generator);  // generates number
}

void Manager::AddGame(boost::shared_ptr<Game> newgame)
{
    database[newgame->GetIndex()] = newgame;
}

void Manager::RemoveGame(boost::shared_ptr<Game> oldgame)
{
    database.erase(oldgame->GetIndex());
}

Manager::Manager():distribution(0,playerModel::COUNT)
{
    cout << "Creating GameManager Singleton" << endl;
}




