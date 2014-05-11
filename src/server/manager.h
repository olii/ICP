/** @file manager.h
* @author Oliver Nemček
* @brief Singleton, ktory sa stara o vytvorene hry
*
* Manager ma v sebe ulozene prave rozohrane hry, spravuje nacitane mapy
* implemetuje primitivnu spravu hier
*
*/


#ifndef MANAGER_H
#define MANAGER_H


#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "../shared/serverinfo.h"
#include "../shared/map.h"


#include "player.h"
#include "game.h"

class Manager
{
public:

   static Manager& instance()
   {
       static Manager instance_;
       return instance_;
   }

   void LoadMapList();

   boost::shared_ptr< Game > CreateGame(std::string name, int max, float tick, int timeout, std::string map );
   boost::shared_ptr<Game> GetGameById(uint32_t id);
   boost::shared_ptr<Game> GetJoinableGame();
   ServerInfoList ServerList();
   ServerInfoList MapList();
   Map GetMapByName( std::string name );
   void DestroyGame(boost::shared_ptr< Game > game);
   void Shutdown();
   int Random();


private:
    Manager();
    std::map<uint32_t, boost::shared_ptr< Game >> database;
    void AddGame(boost::shared_ptr<Game> newgame);
    void RemoveGame( boost::shared_ptr< Game > oldgame );
    bool ValidateNewGame(std::string &name, int max, float tick, int timeout, std::string &map );

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;

    std::map< std::string,Map > maplist;
};

#endif // MANAGER_H
