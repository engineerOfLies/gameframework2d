#ifndef __MONSTERS_H__
#define __MONSTERS_H__

#include "gf2d_entity.h"

typedef struct
{
    TextLine action;
    float damages[DT_MAX];
    List *attack_frames;
    float min_range,max_range;// threshold for choosing this attack
    float attack_range;
    TextLine type;
    TextLine link;
    TextLine spell;
    TextLine aim;
    TextLine sound;//which sound to play
    Vector2D offset;
}MonsterAttack;

/**
 * @brief generic spawn function for monsters.
 */
Entity *monster_spawn_generic(Vector2D position,Uint32 id, SJson *args,const char *entityDef);

/**
 * @brief handle all the boilerplate code for a monster spawn
 * @param position where it should be placed
 * @param id the level id for the entity
 * @param args the spawn arguments from the level
 * @param entityDef the definition file for configuration
 * @return NULL if the spawn should not happen or error, the entity otherwise
 */
Entity *monster_spawn_common(Vector2D position,Uint32 id, SJson *args,const char *entityDef);

/**
 * @brief generic monster function to check for the player
 * @param self the monster
 * @return 0 if the player is not seen, 1 otherwise
 */
int monster_player_sight_check(Entity *self);

/**
 * @brief check if the player is within a set range from the asking entity
 * @param self the monster searching
 * @param range how far away to check (self->sight_range is a good value to pass)
 * @return 1 if the player is within the range, 0 otherwise
 */
int monster_player_in_range(Entity *self,float range);

/**
 * @brief generic monster think function for monster patrolling
 * @param self the monster
 */
void monster_think_patroling(Entity *self);

/**
 * @brief generic monster function to execute an attack
 * @param self the monster
 */
void monster_attack(Entity *self);

/**
 * @brief generic monster think function for when the monster is attacking
 * @param self the monster
 */
void monster_think_attacking(Entity *self);

/**
 * @brief turn to face the player
 * @param self the monster to turn
 */
void monster_face_player(Entity *self);

/**
 * @brief check if the monster has lost the player
 * @param self the monster
 * @return 1 if the player is lost
 */
int monster_lost_player(Entity *self);

/**
 * @brief generic attack function for a monster thrusting attack
 * @param self the monster attacking
 * @return 1 if it hit anything
 */
int melee_thrust_update(
    Entity *self);

/**
 * @brief a generic function to handle a monster taking damage
 * @param self the monster to take damage
 * @param amount the array of damage types
 * @param source the entity to get the credit
 * @return the amount of damage dealt
 */
float monster_damage(Entity *self,float *amount, Entity *source);

/**
 * @brieft when the monster has died
 */
void monster_die(Entity *self);

/**
 * @brief generic monster update function
 * @param self the monster
 */
void monster_update(Entity *self);

/**
 * @brief generic function to turn the monster around during hunt mode
 * @param self the monster
 * @param dir the direciton to move to
 */
void monster_turn(Entity *self,int dir);

/**
 * @brief generic monster think function for hunting the player
 * @param self the monster
 */
void monster_think_hunting(Entity *self);
void monster_think_flying_hunting(Entity *self);

/**
 * @brief generic monster think function
 * @param self the monster
 */
void monster_think(Entity *self);

/**
 * @brief utility function to get direction to the player from the given entity
 * @param self not the player
 * @return a vector containing the direction
 */
Vector2D monster_direction_to_player(Entity *self);

#endif
