#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "simple_json.h"
#include "gf2d_entity.h"

/**
 * @brief spawn a new player entity
 * @param position place the player here
 * @param id the player id (should be zero)
 * @param args the spawn args for the player
 * @return a pointer to the player entity
 */
Entity *player_new(Vector2D position,Uint32 id, SJson *args,const char *entityDef);

/**
 * @brief get a pointer to the player entity
 * @return a pointer to the player entity
 */
Entity *player_get();

/**
 * @brief stop the player from moving for a number of frames
 * @param frames how many render frames to stop for
 */
void player_halt(int frames);

/**
 * @brief sets the player position to the one specified
 * @note risk of solid collision
 * @param position the new position for the player
 */
void player_set_position(Vector2D position);

/**
 * @brief call to spawn the player into the level or put the player into the new level if its already been spawned
 * @param position, where to put the player
 * @param id the id of the player (should always be zero)
 * @param args the player's spawn args
 * @return a pointer to the player
 */
Entity *player_spawn(Vector2D position,Uint32 id, SJson *args,const char *entityDef);



#endif
