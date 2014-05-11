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
    void invalidate();
    bool IsValid();

    enum StaticTypes {GRASS = 0, WALL = 1, KEY = 2, PLAYER_SPAWN = 3, GUARD_SPAWN = 4, GATE = 5, FINISH = 6, PLAYER_BASE = 100, GUARD_BASE=150, GATE_OPEN = 200,};
    std::vector< std::vector< StaticTypes > > items;
    std::string name;

    typedef std::vector< std::vector< StaticTypes > > MapMatrix;
private:
    bool isValid;
    /* serializacia */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & items;
    }
};


class MapItemsInfo
{
public:
    MapItemsInfo();
    MapItemsInfo(int x, int y, bool optionFlag = false , playerDirection dir = playerDirection::UP, int model = 0, uint32_t id = 0, std::string strf = std::string(),int keyCount = 0, int steps = 0);
    uint32_t id; // id
    int x, y; // suradnice
    playerDirection dir; // orientacia
    int keyCount;
    int steps; // pocet krokov
    int model;
    std::string stringField; // rezervovane pre buduce pouzitie - vyuziva sa ako timer

    bool optionFlag;   // pri brane, ci je otvorena, pri kluci ci je na mape

private:
    /* serializacia */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & x;
        ar & y;
        ar & id;
        ar & dir;
        ar & keyCount;
        ar & steps;
        ar & model;
        ar & optionFlag;

         ar & stringField;
    }
};

/*kontajner nad polozkami mapy*/
class MapUpdate
{
public:
    enum HeaderCode { CODE = packetHeader::GAME_UPDATE }; // mandatory
    MapUpdate();

    std::vector< MapItemsInfo > players;
    std::vector< MapItemsInfo > guards;
    std::vector< MapItemsInfo > keys;
    std::vector< MapItemsInfo > gates;
    MapItemsInfo treasure;

private:

    /* serializacia */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & players;
        ar & guards;
        ar & keys;
        ar & gates;
        ar & treasure;
    }
};

#endif // MAP_H
