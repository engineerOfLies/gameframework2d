#ifndef __GF2D_ENTITY_H__
#define __GF2D_ENTITY_H__

#include "gf2d_text.h"
#include "gf2d_vector.h"
#include "gf2d_actor.h"
#include "gf2d_particles.h"
#include "gf2d_collision.h"

typedef struct Entity_S
{
    Uint8 inuse;                            /**<never touch this*/

    TextLine name;                          /**<name of the entity, for information purposes*/
    
    Body *body;                             /**<the physics body for this entity*/
    
    Sprite *sprite;                         /**<which sprite to draw this entity with*/
    float frame;                            /**<current frame for the sprite*/
    ActionList *al;                         /**<action list for managing sprite animations*/
    TextLine action;                        /**<the current action*/
    
    Vector2D position;                      /**<draw position*/
    Vector2D velocity;                      /**<movement direction*/
    Vector2D acceleration;                  /**<acceleration (or gravity)*/
    
    Vector2D scale;                         /**<scale to draw sprite at*/
    Vector2D scaleCenter;                   /**<where to scale sprite from*/
    Vector3D rotation;                      /**<how to rotate the sprite*/
    Vector2D flip;                          /**<if to flip the sprite*/
    Color color;                            /**<color to shift sprite too*/

    SDL_Rect boundingbox;                   /**<axis aligned bounding box*/
    
    ParticleEmitter *pe;                    /**<if this entity has its own particle emitter*/
    
    struct Entity_S *parent;                /**<pointer to the entity that spawned this one, if it applies*/
    
    void (*draw)(struct Entity_S *self);    /**<called after system entity drawing for custom effects*/
    void (*think)(struct Entity_S *self);   /**<called before system updates to make decisions / hand input*/
    void (*update)(struct Entity_S *self);  /**<called after system entity update*/
    void (*touch)(struct Entity_S *self,struct Entity_S *other);/**<when this entity touches another entity*/
    void (*damage)(struct Entity_S *self,int amount, struct Entity_S *source);/**<when this entity takes damage*/
    void (*die)(struct Entity_S *self);     /**<when this entity dies*/
    
    float health;                           /**<health of entity*/
    int   maxHealth;                        /**<maximum health of entity*/
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

#endif
