#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gf2d_text.h"
#include "gf2d_particles.h"
#include "gf2d_list.h"
#include "gf2d_vector.h"


typedef struct
{
    Vector2D    position;
    TextLine    name;
}Spawn;


/**
 * @brief information about a level that is loaded from file
 */
typedef struct
{
    TextLine    name;
    Spawn      *spawnList;
    Uint32      spawnCount;
    Rect        bounds;
    TextLine    backgroundImage;
    Vector2D    backgroundOffset;
    TextLine    backgroundMusic;
    Vector4D    backgroundColor;
    Vector4D    starfield;
    Vector4D    starfieldVariance;
    Uint32      starRate;
    float       starSpeed;
}LevelInfo;

/**
 * @brief Load level information from file
 * @param filename the file to load info from
 * @return the information loaded for the level
 */
LevelInfo *level_info_load(char *filename);

/**
 * @brief free the information loaded for a level
 * @note: this can be done after the level has been started
 * @param info the info to free
 */
void level_info_free(LevelInfo *info);

/**
 * @brief using the level info, start running the level
 * @param info the level info to base this level on
 */
void level_start(LevelInfo *info);

/**
 * @brief clean up the loaded level and free all data
 */
void level_close();


/**
 * @brief update the current level
 */
void level_update();

/**
 * @brief draw the current level
 */
void level_draw();
#endif
