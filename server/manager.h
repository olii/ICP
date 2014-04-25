#ifndef MANAGER_H
#define MANAGER_H


#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>


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

   boost::shared_ptr< Game > CreateGame();
   boost::shared_ptr<Game> GetGameById(uint32_t id);
   boost::shared_ptr<Game> GetJoinableGame();
   void ServerList();
   void DestroyGame(boost::shared_ptr< Game > game);
   void Shutdown();


private:
    Manager();
    std::map<uint32_t, boost::shared_ptr< Game >> database;
    void AddGame(boost::shared_ptr<Game> newgame);
    void RemoveGame( boost::shared_ptr< Game > oldgame );
};

#endif // MANAGER_H
