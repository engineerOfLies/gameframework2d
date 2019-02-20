#ifndef __GF2D_ENTITY_H__
#define __GF2D_ENTITY_H__

#include "gf2d_text.h"
#include "gf2d_vector.h"
#include "gf2d_actor.h"
#include "gf2d_audio.h"
#include "gf2d_particles.h"
#include "gf2d_collision.h"

typedef enum
{
    ES_Idle,
    ES_Seeking,
    ES_Charging,
    ES_Attacking,
    ES_Cooldown,
    ES_Pain,
    ES_Dying,
    ES_Dead         //Auto cleaned up
}EntityState;

#define EntitySoundMax 8

typedef struct Entity_S
{
    Uint8 inuse;                            /**<never touch this*/
    Uint64  id;                             /**<auto increment id for this entity*/

    TextLine name;                          /**<name of the entity, for information purposes*/
    EntityState state;                      /**<state of the entity*/
        /*physics*/
    Shape shape;                            /**<the shape of the physics collider*/
    Body  body;                             /**<the physics body for this entity*/
    Vector2D position;                      /**<draw position*/
    Vector2D velocity;                      /**<desired movement direction*/
    Vector2D acceleration;                  /**<acceleration*/
    
        /*graphics*/
    Actor actor;                            /**<animated sprite information*/
    Vector2D scale;                         /**<scale to draw sprite at*/
    Vector2D scaleCenter;                   /**<where to scale sprite from*/
    Vector3D rotation;                      /**<how to rotate the sprite*/
    Vector2D flip;                          /**<if to flip the sprite*/
    
    ParticleEmitter *pe;                    /**<if this entity has its own particle emitter*/

    /*sound*/
    Sound *sound[EntitySoundMax];           /**<sounds*/
    /*system*/
    struct Entity_S *parent;                /**<pointer to the entity that spawned this one, if it applies*/
    
    void (*draw)(struct Entity_S *self);    /**<called after system entity drawing for custom effects*/
    void (*think)(struct Entity_S *self);   /**<called before system updates to make decisions / hand input*/
    void (*update)(struct Entity_S *self);  /**<called after system entity update*/
    int  (*touch)(struct Entity_S *self,struct Entity_S *other);/**<when this entity touches another entity*/
    void (*activate)(struct Entity_S *self,struct Entity_S *activator);    /**<some entities can be activated by others, doors opened, levels, etc*/
    int  (*damage)(struct Entity_S *self,int amount, struct Entity_S *source);/**<when this entity takes damage*/
    void (*die)(struct Entity_S *self);     /**<when this entity dies*/
    void (*free)(struct Entity_S *self);     /**<called when the entity is freed for any custom cleanup*/
    int dead;                               /**<when true, the entity system will delete the entity on the next update*/

    /*game specific data*/
    TextLine    targetLevel;                /*for doors*/
    TextLine    targetEntity;               
    Uint32      targetId;                   
    
    float health;                           /**<health of entity*/
    int   maxHealth;                        /**<maximum health of entity*/
    int   cooldown;                         /**<useful for timing cooldowns*/
    int   count;                            /**<useful for counting things like ammo count or health ammount*/
    float jumpcool;
    int   jumpcount;                        /**<used for multijump*/
    int   grounded;             
    void *data;                             /**<any other game specific data can be referenced here*/
}Entity;

/**
 * @brief initialize entity system
 * @param maxEntities limit on number of simultaneously active entities
 */
void gf2d_entity_system_init(Uint32 maxEntities);

/**
 * @brief get a new entity from the system
 * @returns NULL when no more entities, or a new valid entity ready to be given data
 */
Entity *gf2d_entity_new();

/**
 * @brief returns an entity back to the system for recycling
 * @param self the entity to be freed
 */
void gf2d_entity_free(Entity *self);

/**
 * @brief draw a single entity
 * @param self the entity to draw
 */
void gf2d_entity_draw(Entity *self);

/**
 * @brief draw all active entities in the system
 */
void gf2d_entity_draw_all();

/**
 * @brief call all think function for all active entities in the system
 */
void gf2d_entity_think_all();

/**
 * @brief update all active entities in the system
 */
void gf2d_entity_update_all();

/**
 * @brief call before call to collision space update to prep all bodies with their entities
 */
void gf2d_entity_pre_sync_all();

/**
 * @brief call after call to collision space update to get all entities in sync with what happened
 */
void gf2d_entity_post_sync_all();

/**
 * @brief deal damage to target entity
 * @param target the entity to take damage
 * @param inflictor the entity doing the damage
 * @param attacker the entity getting the credit for the damage
 * @param damage how much damage
 * @param kick damage direction
 * @return true if damage was dealth, false otherwise
 */
int gf2d_entity_deal_damage(Entity *target, Entity *inflictor, Entity *attacker,int damage,Vector2D kick);

/**
 * @brief check to see if a given pointer points to an entity in the entity system
 * @param p a pointer to data to check
 * @return 0 if the pointer is out of the range of the internal entity manager's list, 1 otherwise
 */
int gf2d_entity_validate_entity_pointer(void *p);

/**
 * @brief search active entities for the one with the provided id
 * @param id the id to search for
 * @return NULL if not found, or the first entity with the id specified otherwise
 */
Entity *gf2d_entity_get_by_id(Uint32 id);

/**
 * @brief search active entities for the one with the provided id and name
 * @param name the name to search for
 * @param id the id to search for
 * @return NULL if not found, or the first entity with the id specified otherwise
 */
Entity *gf2d_entity_get_by_name_id(const char *name,Uint32 id);

/**
 * @brief step through the entity list from the starting point provided
 * @note if start is NULL, it will return the first active entity in the list
 * @param start the starting position to iterate from
 * @return the next entity in the list that is active or NULL when there are not more
 */
Entity *gf2d_entity_iterate(Entity *start);


#endif
