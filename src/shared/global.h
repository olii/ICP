/** @file global.h
* @author Oliver Nemček
* @brief Obecne konstanty ktore sa pouzivaju v celom projekte
*
*
*/


#ifndef GLOBAL_H
#define GLOBAL_H

#include <cstdint>


enum packetHeader { header_size = 2 * (sizeof (uint32_t) ),  HELLO=1000, SERVER_LIST, COMMAND, CREATE_SERVER, STATIC_MAP, GAME_UPDATE };
enum playerModel{ COUNT = 7 };
enum GuardModel{ GUARD_COUNT = 7 };
enum playerDirection { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3 };

#endif // GLOBAL_H
