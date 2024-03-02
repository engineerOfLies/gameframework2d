#ifndef __DUNGEON_H__
#define __DUNGEON_H__

#include "gfc_list.h"

#include "rooms.h"


typedef struct
{
  List     *rooms;    /*list of rooms in the dungeon*/
  Uint32    roomIdPool;
  SDL_Rect  bounds;    /*level bounding rect for placing rooms*/
  Uint8    *tileMap;    /*tiles of the map, after it is "dug"*/
  int       numtiles;  /*number of tiles in the list*/
}Level;

/**
 * @brief allocate a blank level
 */
Level *level_new();

/**
 * @brief generate a level of provided dimensions
 * @param w how wide in tiles
 * @param h how high, in tiles
 * @return NULL on error, or the generated level
 */
Level *level_generate(Uint32 w, Uint32 h);

/**
 * @brief free a level
 */
void level_free(Level *level);


#endif
