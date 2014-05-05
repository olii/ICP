#include "serverinfo.h"
#include "../server/manager.h"

#include <string>
#include <ostream>

ServerInfo::ServerInfo()
{
}

ServerInfo::ServerInfo( unsigned int idp, std::string namep, unsigned int playingp, unsigned int maxp, std::string mapp, float tickp, int timeoutp)
    : id(idp), name(namep), playing(playingp), max(maxp), map(mapp), timer(tickp), timeout(timeoutp)
{
}

bool ServerInfo::ValidateNewGame()
{
    if (name == std::string(""))
        return false;
    if ( max <1 || max > 4 )
        return false;
    /* TODO manager mapy */
    //std::string map;        // nazov mapy


    if ( timer <0.5 || timer > 5 )
        return false;

    if ( timeout <0 || timeout > ( 10*60 ) ) // max 10 minut
        return false;

    return true;
}
/*
unsigned int id;        // id hry
std::string name;       // nazov servera
unsigned int playing;   // pocet hrajucich hracov
unsigned int max;       // maximum hracov
std::string map;        // nazov mapy
float timer;            // ako casto sa vzhodnocuje go
int timeout;            // cakaci cas na dalsich hracov, menej ako 10 min
*/

std::ostream& operator<<(std::ostream& os, const ServerInfo& t)
{
    os << "#" << t.id << " | " << t.name << " | " << t.playing << "/" << t.max << " | " << t.map << " | " << t.timer << " | " << t.timeout;
    return os;
}


ServerInfoList::ServerInfoList()
{

}


ServerInfoCreate::ServerInfoCreate()
{

}
