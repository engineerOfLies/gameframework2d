#ifndef __ENTITY_COMMON_H__
#define __ENTITY_COMMON_H__

#include "gf2d_entity.h"

/**
 * @brief Check if the entity in question is in the camera view
 * @param self the entity to check
 * @return 0 if the entity is not in the camera view, 1 otherwise
 */
int entity_camera_view(Entity *self);

/**
 * @brief adjust position of entity to redice clipping static shapes of the world
 * @param self the entity to adjust
 */
void entity_world_snap(Entity *self);

/**
 * @brief adjust velcity according to gravity/ test for grounded status
 * @param self the entity to adjust
 */
void entity_apply_gravity(Entity *self);

/**
 * @brief check if the entity is adjacent to a static shape in the world.
 * @note this is used for testing if the entity is on the ground or to prevent getting stuck on a wall
 * @param self the entity to check for
 * @param dir the direction to offset by
 * @returns 0 if all clear, 1 if the world can be touched
 */
int entity_wall_check(Entity *self, Vector2D dir);

/**
 * @brief check if the entity is touching the player
 * @param self the entity to check
 * @returns NULL on error or if not touching the player, a pointer to the player otherwise
 */
Entity *entity_get_touching_player(Entity *self);

/**
 * @brief push other entity away from self by amount
 */
void entity_push(Entity *self,Entity *other,float amount);

/**
 * @brief damage target entity if possible, applying kick as well
 */
void entity_damage(Entity *target,Entity *killer,int damage,float kick);

/**
 * @brief perform a hit scan check against the bodies in the level space
 * @param self the entity doing the scanning, it will be ignored
 * @param start the beginning location of the scan
 * @param end the end point for the scan
 * @returns collision information
 */
Collision entity_scan_hit(Entity *self,Vector2D start,Vector2D end, CollisionFilter filter);

/**
 * @brief get a list of all entities that overlap the test shape
 * @param self this entity will be ignored
 * @param s this shape is used to determine which entities will be considered
 * @param layers only check the matching layers for collisions
 * @param team if non zero, only check if the teams don't match
 * @return NULL on error or no results, or a list of collision data otherwise.  The list must be freed
 */
List *entity_get_clipped_entities(Entity *self,Shape s, Uint32 layers, Uint32 team);

/**
 * @brief get the entity associated with the body provided
 * @param body the body to check
 * @param return NULL on error or if the body is not part of an entity, the entity pointer otherwise
 */
Entity *entity_get_from_body(Body *body);

/**
 * @brief delete all entities, except the player entity.  Used in level transitions
 */
void entity_clear_all_but_player();

/**
 * @brief check if there are any entities touching the entity and trigger their activation function
 * @param self the entity to do the activation
 */
void entity_activate(Entity *self);

#endif
