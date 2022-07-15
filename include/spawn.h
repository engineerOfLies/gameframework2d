#ifndef __SPAWN_H__
#define __SPAWN_H__

#include "simple_json.h"
#include "gfc_shape.h"
#include "gf2d_entity.h"

typedef struct
{
    const char *name;
    const char *actor;
    const char *action;
    Entity *(*spawn)(Vector2D,Uint32, SJson *args,const char *entityDef);
    char *entityDef;
}Spawn;

typedef struct
{
    Spawn *spawn;
    SJson *entityDef;
    Actor *actor;
    Action *action;
    Shape  shape;
    float frame;
    int index;
}SpawnData;


/**
 * @brief load the meta data on the spawns
 * @note only needed for the editor
 */
void spawn_load_data();

/**
 * @brief draw the spawn point based on spawn name at the given position
 * @param spawn the name of the spawn candidate to draw
 * @param position where on the screen to draw it
 */
void spawn_draw_spawn_by_name(const char* spawn, Vector2D position);

/**
 * @brief given spawn json, get its name
 */
const char *spawn_get_name(SJson *spawn);

/**
 * @brief draw the outline for a given spawn in the specified color
 * @param spawn the name of the spawn to draw
 * @param position the position of the spawn
 * @param color what color to draw with
 */
void spawn_draw_spawn_outline_by_name(const char* spawn, Vector2D position, Color color);

/**
 * @brief get spawn information by name
 * @param name the name to search for
 * @return NULL on not found or NULL name
 */
Spawn *spawn_get_by_name(const char *name);

/**
 * @brief get the shape of a spawn by its name
 * @param name the name of the spawn
 * @return the shape for it,  ALL zero if error
 */
Shape spawn_get_shape_by_name(const char *name);

/**
 * @brief given its name, get its index
 * @param name the name to search for
 * @return -1 on error or the index in question
 */
int spawn_get_index_by_name(const char *name);

/**
 * @brief get the name of a spawn by it's index;
 * @param index which spawn to get
 * @return NULL on not found or not loaded yet
 */
const char *spawn_get_name_by_index(Uint32 index);

/**
 * @brief get the number of spawns loaded.
 * @note should be called after spawn_load_data();
 * @returns the number of spawns loaded
 */
int spawn_get_count();

/**
 * @brief spawn an entity based on its spawn data
 */
void spawn_entity(const char *name,Vector2D position,Uint32 id,SJson *args);

/**
 * @brief get loaded spawn data by its index
 * @param index the index to get
 * @return NULL on not found
 */
SpawnData *spawn_data_get_by_index(Uint32 index);

#endif
