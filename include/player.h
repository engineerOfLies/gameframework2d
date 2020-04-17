#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "simple_json.h"

#include "gf2d_entity.h"

#include "inventory.h"

/**
 * @brief spawn a blank player entity at the position provided for the given scene
 * @param position where in the scene to put the player
 * @param scene scene to set the player into
 * @return NULL on error a pointer to a valide player entity
 */
Entity *player_spawn(Vector2D position,char *scene);

/**
 * @brief spawn a blank player entity
 * @return NULL on error or an initialized player entity otherwise
 */
Entity *player_new();

/**
 * @brief Save a player's information to file
 * @param player the player data to save;
 * @param filename the file to save the player to
 */
void player_save(Entity *player, char *filename);

/**
 * @brief convert player data to a json object
 * @param player the player to convert
 * @return NULL on error or the json data that describes the player otherwise
 */
SJson *player_to_json(Entity *player);

/**
 * @brief load a game and spawn a player from its save file
 * @param filename the name of the save file to load
 * @return NULL on file not found or other error.  A pointer to an entity otherwise
 */
Entity *player_load(char *filename);

/**
 * @brief parse player data from loaded json
 * @param json the json to define the player
 * @return NULL on error or a configured player entity otherwise
 */
Entity *player_parse_from_json(SJson *json);

/**
 * @brief free a player entity and its related data
 * @param player the entity to free
 */
void player_free(Entity *player);

/**
 * @brief instruct the player to walk to towards the point on the screen.
 */
void player_walk_to(Entity *player,Vector2D position);

/**
 * @brief check if the player is near a point in the scene
 */
int player_near_scene_point(Entity *player,Vector2D position);

/**
 * @brief set the scene information for the player's save data
 * @param player the player to alter
 * @param scene the scene to set it to
 */
void player_set_scene(Entity *player,char *scene);

/**
 * @brief get the scene name from the player data
 * @param player the player entity to get the data from
 * @return NULL on error or the scene associated with the player
 */
char *player_get_scene(Entity *player);

/**
 * @brief get the filename from the player data
 * @param player the player entity to get the data from
 * @return NULL on error or the filename associated with the player
 */
char *player_get_filename(Entity *player);


Inventory *player_get_item_inventory(Entity *player);
int player_give_item(Entity *player,char *item,int count);


/**
 * @brief set a callback for the player to perform after the current command is completed
 * @param player the player to set a callback for
 * @param call the function to all
 * @param data data to pass to the function
 */
void player_set_callback(Entity *player,void(*call)(void *),void *data);

/**
 * @brief set the player to stand at a specific position
 * @param player the player to set the position for
 * @param position the position where the player should be standing
 */
void player_set_position(Entity *player,Vector2D position);

/**
 * @brief run the set callback if there is one
 * @param player the player to run the callback for
 */
void player_run_callback(Entity *player);

#endif
