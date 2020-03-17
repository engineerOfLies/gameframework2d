#ifndef __SCENE_H__
#define __SCENE_H__

#include "simple_json.h"
#include "gfc_types.h"

#include "gf2d_entity.h"
#include "gf2d_actor.h"


// NOTE: this forward declaration is necessary because Exhibits need to know about Scenes and Scenes need to know about exhibits.
// This is how you become your own grandfather
typedef struct Exhibit_S Exhibit;

typedef struct
{
    Uint8      _inuse;              /**<no touchy*/
    TextLine    filename;           /**<name of the file loaded*/
    Actor       background;         /**<background actor*/
    TextLine    action;             /**<action to take for the background actor*/
    SJson      *config;             /**<additional configuration for the scene*/
    List       *exhibits;           /**<list of exhibits in this scene*/
    List       *entities;           /**<list of entities spawned in this scene*/
    Entity     *activePlayer;       /**<active player entity*/
}Scene;


/**
 * @brief add a new exhibit to the scene
 * @param scene the scene to add the exhibit to.
 * @param exhibit the exhibit to add
 */
void scene_add_exhibit(Scene *scene,Exhibit *exhibit);

/**
 * @brief add a new entity to the scene
 * @param scene the scene to add the entity to
 * @param enityt the entity to add
 */
void scene_add_entity(Scene *scene, Entity *entity);

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
 * @brief save a scene to json file
 * @param scene the scene to save
 * @param filename the name of the file to save to
 */
void scene_save(Scene *scene, char *filename);

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

/**
 * @brief set the active player for the scene
 * @param scene the scene to modify
 * @param player the player to make the active player for
 */
void scene_set_active_player(Scene *scene,Entity *player);

Entity *scene_get_active_player(Scene *scene);

/**
 * @brief issue a walk command to the active player to walk towards the specified position in the scene
 * @param scene the scene to address
 * @param position the position in the scene to walk to
 */
void scene_active_player_walk_to(Scene *scene,Vector2D position);

/**
 * @brief transition into the next scene
 * @param nextScene the name of the file that describes the next scene
 * @param player the active player that will be placed into the new scene
 * @param positionExhibit the exhibit that will be used as a reference to place the player at
 */
void scene_next_scene(char *nextScene, Entity *player, char *positionExhibit);

/**
 * @brief set a scene to be the active scene
 * @param scene the scene to set as active
 */
void scene_set_active(Scene *scene);

/**
 * @brief get a pointer to the active scene
 * @return NULL if not set, or the pointer otherwise
 */
Scene *scene_get_active();

#endif
