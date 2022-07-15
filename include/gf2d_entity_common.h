#ifndef __ENTITY_COMMON_H__
#define __ENTITY_COMMON_H__

#include "gf2d_entity.h"

//common functions useful for working with entities

/**
 * @brief calculate total damage based on damage and armor affinities
 * @param damage an array of damages (up to DT_MAX must exist)
 * @param armor an array of defenses per damage Type
 * @return 0 on error, or the total damage to deal otherwise
 */
float entity_calc_damage(float *damage,float *armor);

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
 * @brief check for platforms above the player
 */
void entity_jump_up(Entity *self,float jump_height);

/**
 * @brief update the entity facing and flip flags based on a new direction
 * @param self the entity to update
 * @param direction the direction of motion.
 */
void entity_face_from_direction(Entity *self, Vector2D direction);


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
 * @brief deal attack damage at the circle provided
 * @param self the entity doing the attacking
 * @param shape a shape describing the impact for dealing damage
 * @param damage the damage types array for the attack
 * @param layers which layers to check against PLAYER_LAYER, MONSTER_LAYER, OBJECT_LAYER
 * @param team the team to ignore.  provide 0 to not filter by teams
 * @return 1 if damage was dealt
 */
int entity_attack_update(
    Entity *self,
    Shape shape,
    float *damage,
    Uint32 layers,
    Uint32 team);

/**
 * @brief called each frame during an attack, performs an overhead slash attack check
 * @param self the entity doing the attacking
 * @param position the center of the attack
 * @param facing the facing direction of the attacker
 * @param attack_range how far away the attack center will reach
 * @param inpact_radius how wide a circle is checked
 * @param damage the damage types array for the attack
 * @param layers which layers to check against PLAYER_LAYER, MONSTER_LAYER, OBJECT_LAYER
 * @param team the team to ignore.  provide 0 to not filter by teams
 * @return 1 if damage was dealt
 */
int entity_melee_slash_update(
    Entity *self,
    Vector2D position,
    Vector2D facing,
    float attack_range,
    float impact_radius,
    float *damage,
    Uint32 layers,
    Uint32 team);

/**
 * @brief called each frame during an attack, performs linear thrust attack check
 * @param self the entity doing the attacking
 * @param position the center of the attack
 * @param facing the facing direction of the attacker
 * @param attack_range how far away the attack center will reach
 * @param inpact_radius how wide a circle is checked
 * @param damage the damage types array for the attack
 * @param layers which layers to check against PLAYER_LAYER, MONSTER_LAYER, OBJECT_LAYER
 * @param team the team to ignore.  provide 0 to not filter by teams
 * @return 1 if damage was dealt
 */
int entity_melee_thrust_update(
    Entity *self,
    Vector2D position,
    Vector2D facing,
    float attack_range,
    float impact_radius,
    float *damage,
    Uint32 layers,
    Uint32 team);


/**
 * @brief perform a hit scan check against the bodies in the level space
 * @param self the entity doing the scanning, it will be ignored
 * @param start the beginning location of the scan
 * @param end the end point for the scan
 * @param filter how to filter the hit scan
 * @param endpoint (optional) if you want the point of collision from the trace
 * @returns the first entity hit by the trace, or NULL if none were.
 */
Entity *entity_scan_hit(Entity *self,Vector2D start,Vector2D end, CollisionFilter filter,Vector2D *endpoint);

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
 * @brief get a list of static shapes from the space that the entity is clipping
 * @param self this entity will be ignored
 * @param s this shape is used to determine which entities will be considered
 */
List *entity_get_clipped_world(Entity *self,Shape s);

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

/**
 * @brief check if the entity is about to walk off the edge of a platform
 * @note useful for AI
 * @param self the entity to check for
 * @return 0 if there is no more platform, 1 if there is something there
 * 
 */
int entity_platform_end_check(Entity *self);

/**
 * @brief parse the entity's id from the args
 */
int entity_id_from_args(Entity *self);

/**
 * @brief check if an entity has been configured to be persistent
 * @note: many are persistent by default.  
 * @param self the entity to check
 * @return 0 if not persistent, 1 if it is
 */
int entity_is_persistent(Entity *self);


/**
 * @brief call the activation function on the trigger target for the given entity
 * @param self the entity causing the trigger
 */
void entity_run_trigger(Entity *self);

/**
 * @brief configure an entity based on a json file
 * @param self the entity to configure
 * @param args the arguments that came from the level
 * @param position where to put the entity
 * @param filename the json file to use
 * @return NULL on error, or the entity back again
 */
Entity *entity_config_from_file(Entity *self,SJson *args,Vector2D position,const char *filename);

/**
 * @brief parse entity shape out of a an entityDef file
 * @param file the sjon data to parse
 * @return an empty shape if the data is missing or wrong, or the correct shape otherwise
 */
Shape entity_config_parse_shape(SJson *file);

/**
 * @brief parse armor and damage from entityDef file ( or any json for that matter)
 * @param file the json to parse
 * @param damages must be an array at least DT_MAX long
 */
void entity_parse_damages(SJson *file,float *damages);

/**
 * @brief parse armor and damage from entityDef file ( or any json for that matter)
 * @param file the json to parse
 * @param armor must be an array at least DT_MAX long
 */
void entity_parse_armors(SJson *file,float *armor);

#endif
