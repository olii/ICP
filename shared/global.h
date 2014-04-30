#ifndef GLOBAL_H
#define GLOBAL_H

#include <cstdint>


enum packetHeader { header_size = 2 * (sizeof (uint32_t) ),  HELLO=1000, SERVER_LIST, COMMAND, CREATE_SERVER, GAME_UPDATE };

#endif // GLOBAL_H
