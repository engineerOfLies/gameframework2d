#ifndef __WORLD_H__
#define __WORLD_H__

#include "gf2d_sprite.h"

typedef struct
{
    Sprite *background; /**<background image for the world*/
    Sprite *tileLayer;  /**<prerendered tile layer*/
    Sprite *tileSet;    /**<sprite containing tiles for the world*/
    Uint8  *tileMap;    /**<the tiles that make up the world*/
    Uint32  tileHeight; /**<how many tiles tall the map is*/
    Uint32  tileWidth;  /**<how many tiles wide the map is*/
}World;


/**
 * @brief load a world from a config file
 * @param filename the name of the world file to load
 * @return NULL on error, or a usable world otherwise
 */
World *world_load(const char *filename);

/**
 * @brief test function to see if world is working
 */
World *world_test_new();

/**
 * @brief set the camera bounds to the world size
 */
void world_setup_camera(World *world);

/**
 * @brief allocate a new empty world
 * @return NULL on error, or a blank world
 */
World *world_new();

/**
 * @brief free a previously allocated world
 * @param world the world to free
 */
void world_free(World *world);

/**
 * @brief draw the world
 * @param world the world to draw
 */
void world_draw(World *world);

#endif
