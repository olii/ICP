#ifndef GLOBAL_H
#define GLOBAL_H

#include <cstdint>


enum packetHeader { header_size = 2 * (sizeof (uint32_t) ),  HELLO=1000, SERVER_LIST, COMMAND, CREATE_SERVER, STATIC_MAP, GAME_UPDATE };
enum playerModel{ COUNT = 2 };
enum GuardModel{ GUARD_COUNT = 1 };
enum playerDirection { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3 };

#endif // GLOBAL_H
