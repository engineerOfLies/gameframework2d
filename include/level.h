#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "tiledef.h"

typedef struct
{
    Sprite *background;     //background sprite
    Sprite *tileLayer;      //static tile layer for fast rendering
    Uint8  *tileMap;        //pointer to the tile map data
    Uint32  width,height;   //size of the tileMap in tiles
    GFC_Vector2D size;      //size of the tileMap in pixels
    TileDef *tiledef;        //describes the tiles
}Level;

/**
 * @brief allocate and build a level from the parameters
 * @param filepath the path to the file to load for the level
 * @return NULL on error or nonsensical parameters, a pointer to a level otherwise
 * @note the tilemap still needs to be set
 */
Level *level_load(const char *filepath);

/**
 * @brief set the boarder tiles of the level to the given tile
 * @param level the level to set
 * @param tile the tileMap index to set it too
 */
void level_add_border(Level *level,Uint8 tile);

/**
 * @brief bake the tiles to the tileLayer image
 */
void level_bake_tiles(Level *level);

/**
 * @brief given a level, get the index of the tileMap for a tile's coordinates
 * @param level to check
 * @param x the x coordinate
 * @param y the y coordinate
 * @return -1 if the level is bad or the coordinates are beyond the tilemap, the index otherwise
 */
int level_get_tile_index(Level *level,Uint32 x, Uint32 y);

/**
 * @brief allocated a blank level
 */
Level *level_new();

/**
 * @brief free a level;
 */
void level_free(Level *free);

/**
 * @brief draw the current level
 * @param level the level to draw
 */
void level_draw(Level *level);

#endif
