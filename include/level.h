#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gf2d_text.h"
#include "gf2d_particles.h"
#include "gf2d_list.h"
#include "gf2d_vector.h"


typedef struct
{
    Vector2D    position;
    TextLine    entityName;
}Spawn;


/**
 * @brief information about a level that is loaded from file
 */
typedef struct
{
    TextLine    name;
    List        spawnList;
    Rect        Bounds;
    TextLine    backgroundImage;
    TextLine    backgroundMusic;
    Color       backgroundColor;
    Color       starfield;
    Color       starfieldVariance;
    Uint32      starRate;
    float       starSpeed;
}LevelInfo;

/**
 * @brief Load level information from file
 * @param filename the file to load info from
 * @return the information loaded for the level
 */
LevelInfo level_info_load(char *filename);

/**
 * @brief using the level info, start running the level
 * @param info the level info to base this level on
 */
void level_start(LevelInfo info);

/**
 * @brief update the current level
 */
void level_update();

/**
 * @brief draw the current level
 */
void level_draw();
#endif
