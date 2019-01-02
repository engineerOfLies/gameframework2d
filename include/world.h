#ifndef __WORLD_H__
#define __WORLD_H__

#include "gf2d_collision.h"
#include "gf2d_sprite.h"
#include "gf2d_audio.h"

/**
 * @Purpose game specific world code
 * This game is going to be a tile based side scrolling 2d platformer
 */

/**
 * @brief The world structure holds data for a given game level.
 */
typedef struct
{
    Space *space;
    
}World;

#endif
