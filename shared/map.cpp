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
