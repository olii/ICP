/** @file serverinfo.h
* @author Oliver Nemček
* @brief Informacie o serveri, ktore su prenasane po sieti
*
* definicie datovych typov
* triedy obsahuju vsetko co moze klient o stave servera ziskat
*/

#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <string>
#include <vector>
#include <ostream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include "global.h"

class ServerInfo
{
public:
    ServerInfo( );
    ServerInfo( unsigned int, std::string, unsigned int, unsigned int, std::string, float, int  );

    unsigned int id;        // id hry
    std::string name;       // nazov servera
    unsigned int playing;   // pocet hrajucich hracov
    unsigned int max;       // maximum hracov
    std::string map;        // nazov mapy
    float timer;            // ako casto sa vzhodnocuje go
    int timeout;            // cakaci cas na dalsich hracov, menej ako 10 min

    friend std::ostream& operator<<(std::ostream& os, const ServerInfo& t);


private:
    /* serializacia */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & id ;
        ar & name;
        ar & playing;
        ar & max;
        ar & map;
        ar & timer;
        ar & timeout;
    }
};

class ServerInfoList
{

public:
    enum HeaderCode { CODE = packetHeader::SERVER_LIST }; // mandatory
    std::vector<ServerInfo> list;
    ServerInfoList();
private:
    /* serializacia */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & list ;
    }
};




class ServerInfoCreate
{

public:
    ServerInfoCreate();

    enum HeaderCode { CODE = packetHeader::CREATE_SERVER }; // mandatory
    ServerInfo newServer;
private:


    /* serializacia */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & newServer ;
    }
};

#endif // SERVERINFO_H
