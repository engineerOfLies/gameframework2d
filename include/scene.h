#ifndef __SCENE_H__
#define __SCENE_H__

#include "simple_json.h"
#include "gfc_types.h"

#include "gf2d_actor.h"

typedef struct
{
    Uint8      _inuse;              /**<no touchy*/
    Actor      background;         /**<background image*/
    SJson      *config;             /**<additional configuration for the scene*/
    List       *exhibits;           /**<list of exhibits in this scene*/
    List       *entities;           /**<list of entities spawned in this scene*/
}Scene;


/**
 * @brief initialize scene manager subsystem
 * @param sceneMax how many scenes to support in memory at once
 */
void scene_manager_init(Uint32 sceneMax);


/**
 * @brief spawn all the exhibit entities
 * @param scene the scene to spawn entities for
 */
void scene_spawn_exhibits(Scene *scene);

/**
 * @brief load a scene from a json file
 * @param filename the name of the file to load
 * @returns NULL on error or a valid scene pointer
 * @note: free with scene_free()
 */
Scene *scene_load(char *filename);

/**
 * @brief get a pointer to a new plank scene
 * @returns NULL on error, or a pointer to an empty scene
 */
Scene *scene_new();

/**
 * @brief draw the scene
 * @param scene the scene to draw
 */
void scene_draw(Scene *scene);

/**
 * @brief check for interaction with all of the exhibits in the scene
 * @param scene the scene to update
 */
void scene_update(Scene *scene);

/**
 * @brief frees a scene from memory
 * @param scene the scene to free
 */
void scene_free(Scene *scene);

#endif
