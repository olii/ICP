#include "serverinfo.h"
#include "../server/manager.h"

#include <string>

ServerInfo::ServerInfo()
{
}

ServerInfo::ServerInfo( unsigned int idp, std::string namep, unsigned int playingp, unsigned int maxp, std::string mapp )
    : id(idp), name(namep), playing(playingp), max(maxp), map(mapp)
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


ServerInfoList::ServerInfoList()
{

}


ServerInfoCreate::ServerInfoCreate()
{

}
