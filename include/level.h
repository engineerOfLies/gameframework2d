#ifndef __DUNGEON_H__
#define __DUNGEON_H__

#include "gfc_list.h"

#include "gf2d_sprite.h"

#include "rooms.h"


typedef struct
{
  List     *rooms;    /*list of rooms in the dungeon*/
  Uint32    roomIdPool;
  SDL_Rect  bounds;    /*level bounding rect for placing rooms*/
  Uint8    *tileMap;    /*tiles of the map, after it is "dug"*/
  int       numtiles;  /*number of tiles in the list*/
  Sprite   *tileLayer;
  SDL_Point start;    /*where the map begins*/
  SDL_Point end;      /*where it ends*/
}Level;

/**
 * @brief load what is necessary for rendering a level in 2D
 */
void level_system_init();

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
 * @brief generate the sprite to draw the layer with
 * @param level the one to update
 */
void level_generate_tile_layer(Level *level);

/**
 * @brief draw the level tileLayer to the screen
 * @param level the level to draw
 * @param offset where on the screen to draw it
 */
void level_draw(Level * level, Vector2D offset);

/**
 * @brief free a level
 */
void level_free(Level *level);

/**
 * @brief get the tile position based on world position
 * @param level to check
 * @param position where to check
 * @return (-1,-1) if bad, the tile position otherwise
 */
SDL_Point level_get_tile_from_position(Level *level, Vector2D position);

/**
 * @brief get the tile position base on the mouse
 * @note SDL events must have been updated for this to work
 * @param level to check
 * @return (-1,-1) if bad, the tile position otherwise
 */
SDL_Point level_get_tile_from_mouse_position(Level *level);

/**
 * @brief given a tile position, return the tile index at that position
 * @param level to check
 * @param x position
 * @param y position
 * @return 0 on error, or the tile index otherwise.  0 is a valid choice, an empty tile
 */
int level_get_tile_index(Level *level, int x,int y);


#endif
