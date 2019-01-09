#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gf2d_collision.h"
#include "gf2d_sprite.h"
#include "gf2d_audio.h"
#include "gf2d_entity.h"
#include "simple_json.h"

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
    SJson      *spawnList;
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

/**
 * @brief adds an entity to the level physics space
 * @note entity must have a Body defined
 * @param ent the entity to add to the space
 */
void level_add_entity(Entity *ent);

/**
 * @brief removes an entity from the level physics space
 * @note this should not be done during a space update (ie: collision callbacks)
 * @param ent the entity to be removed
 */
void level_remove_entity(Entity *ent);

Space *level_get_space();


#endif
