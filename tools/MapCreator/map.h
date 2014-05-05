#ifndef MAP_H
#define MAP_H

#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>



class Map
{
public:
    Map();


};




class StaticMap
{
public:
    enum StaticTypes {GRASS = 0, WALL, KEY, PLAYER_SPAWN, GUARD_SPAWN};
    StaticMap();
    StaticMap(int row, int column);

    std::vector< std::vector< StaticTypes > > items;


private:
    /* serializacia */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & items;
    }
};

#endif // MAP_H
