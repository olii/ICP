#ifndef MAP_H
#define MAP_H

#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "global.h"

class Map
{
public:
    enum HeaderCode { CODE = packetHeader::STATIC_MAP }; // mandatory
    Map();

    enum StaticTypes {GRASS = 0, WALL = 1, KEY = 2, PLAYER_SPAWN = 3, GUARD_SPAWN = 4, GATE = 5, FINISH = 6};
    std::vector< std::vector< StaticTypes > > items;
    std::string name;

    typedef std::vector< std::vector< StaticTypes > > MapMatrix;
private:
    /* serializacia */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & items;
    }
};

#endif // MAP_H
