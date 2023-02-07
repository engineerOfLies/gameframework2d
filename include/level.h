#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gfc_text.h"

#include "gf2d_sprite.h"

typedef struct
{
    int tileFrame;
    int solid;  // if true the tile cannot be walked through
}TileInfo;

typedef struct
{
    TextLine    name;
    Sprite     *tileSet;
    Vector2D    tileSize;    //the width and height of the tile map
    Vector2D    mapSize;    //the width and height of the tile map
    int        *tileMap;    //WTF???
    Sprite     *tileLayer;  //TBD
}Level;


/**
 * @brief load a level from a json file
 * @param filename path to the json file containing level data
 * @return NULL on not found or other error, or the loaded level
 */
Level *level_load(const char *filename);

/**
 * @brief allocate a blank level struct
 * @return NULL on error or an intialized level
 */
Level *level_new();

/**
 * @brief clean up a previously allocated level
 */
void level_free(Level *level);

#endif
