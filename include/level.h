#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gf2d_collision.h"
#include "gf2d_sprite.h"
#include "gf2d_audio.h"

/**
 * @Purpose game specific level code
 * This game is going to be a tile based side scrolling 2d platformer
 */

/**
 * @brief The level structure holds data for a given game level.
 */
typedef struct
{
    TextLine    backgroundImage;    /**<background image for this level*/
    TextLine    backgroundMusic;    /**<background music for this level*/
    TextLine    tileSet;            /**<tiles to use for this level*/
    Vector2D    tileSize;           /**<how tile space width & height*/
    Vector2D    tileMapSize;        /**<how tile space width & height*/
    int        *tileMap;            /**<tile data*/
    List        SpawnList;          /**<the things that go here*/
}LevelInfo;

/**
 * @brief clears all assets loaded for the live level
 * Called at program exit and before loading the next level
 */
void level_clear();

void level_draw();
void level_update();

/**
 * @brief create the live level based on the provided level info
 * @param linfo the level info to create the level for
 */
void level_init(LevelInfo *linfo);

void level_info_free(LevelInfo *linfo);
LevelInfo *level_info_load(char *filename);


#endif
