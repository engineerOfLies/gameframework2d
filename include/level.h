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
    TextLine    backgroundImage;    /**<background music for this level*/
    TextLine    backgroundMusic;    /**<background music for this level*/
    Vector2D    tilesize;           /**<how tile space width & height*/
    char       *tilemap;            /**<tile data*/
    List        SpawnList;          /**<the things that go here*/
}Level;


void level_load(char *filename);
void level_init();
void level_update();

#endif
