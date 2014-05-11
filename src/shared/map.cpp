/** @file map.cpp
* @author Oliver Nemček
* @brief implementacia metod nad triedou Map
*
* prevazne konstruktory
* settery a gettery nie su lebo dolezite polozky nie su privatne
*/

#include "map.h"


/**
 * @brief defaultny konstruktor
 */
Map::Map()
{
    isValid = true;
}


/**
 * @brief nastavi instanciu na neplatnu mapu
 *
 * vyuziva sa na kontrolu spravnosti serializacie
 */
void Map::invalidate()
{
    isValid = false;
}

bool Map::IsValid()
{
    return isValid;
}

/**
 * @brief zakladny konstruktor pre serializaciu
 * vyuziva iba kniznica boost
 */
MapItemsInfo::MapItemsInfo()
{
    x = y =  0;
    id = 0;
    dir = playerDirection::UP;
    steps = 0;
    keyCount = 0;
}
/**
 * @brief konstruktor hlavneho objektu ktory abstrahuje lubovolnu entitiu v hre
 * @param x  x-ova suradnica objektu
 * @param y y-ova suradnica objektu
 * @param optionFlag volitelne pole, vyznam zavisi na entite ktoru reprezentuje
 * @param dir reprezentuje smer otocenia entity
 * @param model id modelu pre synchronizaciu klientov
 * @param id  jednoznacne id entity
 * @param strf textove pole, nadstavba nad optionFlag
 * @param keyCount pocet klucoov hraca
 * @param steps povet krokov hraca
 *
 */
MapItemsInfo::MapItemsInfo(int x, int y, bool optionFlag , playerDirection dir, int model, uint32_t id , std::string strf, int keyCount, int steps)
{
    this->x = x;
    this->y = y;
    this->dir = dir;
    this->model = model;
    this->id = id;
    this->optionFlag = optionFlag;
    this->id = id;
    this->keyCount = keyCount;
    this->steps = steps;

    this->stringField = strf;
}

/**
 * @brief konstruktor pre serializaciu
 *
 */
MapUpdate::MapUpdate()
{

}
