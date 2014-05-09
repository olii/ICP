#include "map.h"

Map::Map()
{
    isValid = true;
}

void Map::invalidate()
{
    isValid = false;
}

bool Map::IsValid()
{
    return isValid;
}


MapItemsInfo::MapItemsInfo()
{
    x = y =  0;
    id = 0;
    dir = playerDirection::UP;
    steps = 0;
    keyCount = 0;
}

MapItemsInfo::MapItemsInfo(int x, int y, bool optionFlag, playerDirection dir, int model, uint32_t id, int keyCount, int steps)
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
}


MapUpdate::MapUpdate()
{

}
