#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gfc_types.h"
#include "gfc_text.h"

#include "gf2d_actor.h"
#include "gf2d_sprite.h"

typedef enum
{
    TT_Empty,
    TT_Brick,
    TT_MAX
}TileTypes;

typedef struct
{
    Sprite     *baseTile;
    int         tileFrame;  /**<which tile frame to use*/
    Sprite     *background;
    Vector2D    levelSize;   /**<how large, in pixels, the level is*/
}Level;


/**
 * @brief allocate and initialize a level
 * @return NULL on error, else a pointer to a new level
 */
Level *level_new();


/**
 * @brief load a level from a json file
 * @param filename the file to load
 * @return NULL on error (See the logs) a loaded level otherwise
 */
Level *level_load(const char *filename);

/**
 * @brief free up a previously loaded level
 * @param level a pointer to the level to free
 */
void level_free(Level *level);

/**
 * @brief perform maintenance for the level.  should be called once a frame
 * @param level the level to update
 */
void level_update(Level *level);


/**
 * @brief draw a level
 * @param level the level the draw
 */
void level_draw(Level *level);

#endif
