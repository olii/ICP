/** @file serverinfo.cpp
* @author Oliver Nemček
* @brief Informacie o serveri, ktore su prenasane po sieti
*
* implementacie tried ktore sa pouzivaju ako kontajnery pre prenos po sieti
* prevazne zakladne serializacne konstruktory
*/

#include "serverinfo.h"

#include <string>
#include <ostream>

ServerInfo::ServerInfo()
{
}

ServerInfo::ServerInfo( unsigned int idp, std::string namep, unsigned int playingp, unsigned int maxp, std::string mapp, float tickp, int timeoutp)
    : id(idp), name(namep), playing(playingp), max(maxp), map(mapp), timer(tickp), timeout(timeoutp)
{
}


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
