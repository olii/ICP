/** @file manager.cpp
* @author Oliver Nemček
* @brief implemetacia metod triedy Manager
*
* rezia pamate je zalozena na zdielanych pointroch
*
*/

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


/**
 * @brief vyhladavanie rozohranej hry podla id
 * @param[in] id id hry
 * @return zdielany pointer na hru, alebo neplatny zdielany pointer ak sa hra nenasla
 */
boost::shared_ptr<Game> Manager::GetGameById(uint32_t id)
{
    if ( database.find( id ) == database.end() )
    {
        return  boost::shared_ptr< Game >();
    }
    return database[id];
}


/**
 * @brief navrati prvu hru do ktorej je mozne sa ihned prihlasit
 * @return zdielany pointer na danu hru
 */
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


/**
 * @brief Metóda ktora vygeneruje aktualny zoznam prebiehanych hier
 * @return objekt triedy ServerInfoList, ktory obsahuje vsetky hry
 */
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

/**
 * @brief navrati dostupne mapy ktore mozu byt spustene na serveri
 * @return objekt ktory obsahuje vsetky mapy
 */
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


/**
 * @brief na zaklade nazvu mapy navrati danu mapu z databazy map
 * @param[in] name nazov mapy
 * @return objekt typu mapa, validnost indikuje priznak
 */
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

/**
 * @brief metoda nacitava mapy z adresara a uklada ich v databaze map
 */
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

/**
 * @brief overi poziadavky na novu hru
 */
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

/**
 * @brief vytvori hru podla zadanych parametrov a zaregistruje ju
 * @param[in] name nazov hry
 * @param[in] max maximalny pocet hracov
 * @param[in] tick aktualizacna frekvencia hry
 * @param[in] timeout pociatocne oddialenie zaciatku hry
 * @param[in] map nazov mapy
 * @return vrati zdielany pointer na vytvorenu hru
 */
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

/**
 * @brief odstrani zadanu hru a vola destruktor
 */
void Manager::DestroyGame(boost::shared_ptr<Game> game)
{
    RemoveGame(game);
    //cout << "Manager: Current games = " << database.size() <<endl;
}

/**
 * @brief metoda sa snazi bezpecne vypnut vsetky hry ktore su na serveri
 */
void Manager::Shutdown()
{
    for ( auto x: database )
    {
        x.second->Shutdown();
    }
    database.clear();
}

/**
 * @brief generator nahodnych cisel
 * @return vrati nahodny integer
 */
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

Manager::Manager():distribution(0,1000)
{
    //cout << "Creating GameManager Singleton" << endl;
}




