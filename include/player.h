#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "gf2d_entity.h"


/**
 * @brief spawn a blank player entity at the position provided
 * @param position where in the scene to put the player
 * @return NULL on error a pointer to a valide player entity
 */
Entity *player_spawn(Vector2D position);

/**
 * @brief Save a player's information to file
 * @param player the player data to save;
 */
void player_save(Entity *player);

/**
 * @brief load a player from its save file
 * @param filename the name of the save file to load
 * @return NULL on file not found or other error.  A pointer to an entity otherwise
 */
Entity *player_load(char *filename);

/**
 * @brief free a player entity and its related data
 * @param player the entity to free
 */
void    player_free(Entity *player);

/**
 * @brief instruct the player to walk to towards the point on the screen.
 */
void player_walk_to(Entity *player,Vector2D position);

/**
 * @brief check if the player is near a point in the scene
 */
int player_near_point(Entity *player,Vector2D position);
#endif
