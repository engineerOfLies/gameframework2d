#ifndef __GF2D_ENTITY_H__
#define __GF2D_ENTITY_H__

#include "simple_json.h"

#include "gfc_text.h"
#include "gfc_hashmap.h"
#include "gfc_vector.h"
#include "gfc_audio.h"

#include "gf2d_actor.h"
#include "gf2d_figure.h"
#include "gf2d_particles.h"
#include "gf2d_collision.h"

typedef enum
{
    DT_Physical,
    DT_Arcane,
    DT_Fire,
    DT_Ice,
    DT_Light,
    DT_Dark,
    DT_MAX
}DamageTypes;

typedef enum
{
    ES_Idle,
    ES_Activated,
    ES_Locked,
    ES_Open,
    ES_Walking,
    ES_Jumping,
    ES_Seeking,
    ES_Charging,
    ES_Attacking,
    ES_Cooldown,
    ES_Leaving,
    ES_Inactive,
    ES_Pain,
    ES_Dying,
    ES_Dead         //Auto cleaned up
}EntityState;

#define EntitySoundMax 8

typedef struct 
{
    TextLine bonename;        /**<what bone in the figure to link to*/
    Vector2D offset;
    Shape shape;              /**<what shape to link by (it won't rotate, so circles are best)*/
    Body body;                /**<this will be linked to the shap and given position by the armature pose bone*/
}EntityLink;

typedef struct Entity_S
{
    Uint8 inuse;                            /**<never touch this*/
    Uint32  id;                             /**<auto increment id for this entity*/

    TextLine name;                          /**<name of the entity, for information purposes*/
    EntityState state;                      /**<state of the entity*/
        /*physics*/
    Shape shape;                            /**<the shape of the physics collider*/
    Body  body;                             /**<the physics body for this entity*/
    float speed;                            /**<how fast this entity should be moving*/
    float jump_height;
    Vector2D acceleration;                  /**<acceleration*/
    int   crouching;                        /**<true if the player is crouching*/
    Shape crouchShape;                      /**<shape for when crouching*/
    
    List *attacks;                          /**<list of monster attacks*/
    int   nextattack;                       /**<chosen attack to perform*/
    
    List *links;                            /**<for multi-body entities like bosses and large monsters*/
        /*graphics*/
    ActionReturnType ar;                    /**<action return type of the current animation*/

    Color    color;                         /**<draw color for this entity*/
    FigureInstance  figureInstance;         /**<if the entity uses a figure to draw itself*/
    Actor   *actor;                         /**<animated sprite information*/
    int   cooldown;                         /**<useful for timing cooldowns*/
    int   halt;                             /**<player can be halted for some time*/

    Vector2D scale;                         /**<scale to draw sprite at*/
    Vector2D center;                        /**<where to scale sprite from*/
    float    rotation;                      /**<how to rotate the sprite*/
    Vector2D flip;                          /**<if to flip the sprite*/
    Vector2D facing;                        /**<direction the entity is facing*/
    Vector2D aiming;
    
    /*sound*/
    HashMap *sound;
    /*system*/
    struct Entity_S *parent;                /**<pointer to the entity that spawned this one, if it applies*/
    
    void (*draw)(struct Entity_S *self);    /**<called after system entity drawing for custom effects*/
    void (*think)(struct Entity_S *self);   /**<called before system updates to make decisions / hand input*/
    void (*update)(struct Entity_S *self);  /**<called after system entity update*/
    int  (*touch)(struct Entity_S *self,struct Entity_S *other);/**<when this entity touches another entity*/
    void (*actionEnd)(struct Entity_S *self); /**<called when a PASS action completes*/
    int  (*activate)(struct Entity_S *self,struct Entity_S *activator);    /**<some entities can be activated by others, doors opened, levels, etc*/
    float(*damage)(struct Entity_S *self,float *amount, struct Entity_S *source);/**<when this entity takes damage*/
    void (*die)(struct Entity_S *self);     /**<when this entity dies*/
    void (*free)(struct Entity_S *self);     /**<called when the entity is freed for any custom cleanup*/
    int dead;                               /**<when true, the entity system will delete the entity on the next update*/

    /*lighting*/
    float light_level;                      
    Color light_color;
    
    /*game specific data*/
    TextLine    targetLevel;                /*for doors*/
    TextLine    targetEntity;               
    Uint32      targetId;                   
    
    int   ontouch;                          /**<if true, the pickup activates on player touch*/
    float health;                           /**<health of entity*/
    int   healthmax;                        /**<maximum health of entity*/
    float healthRegen;
    float mana;                             /**<health of entity*/
    int   manamax;                          /**<maximum health of entity*/
    float manaRegen;
    int   attack;                           /**<which attack is being performed*/
    int   last_attack_frame;                /**<the last frame that */
    int   count;                            /**<useful for counting things like ammo count or health ammount*/
    float damageCooldown;                   /**<I frames*/
    int   attackCooldown;                   /**<how long to wait after an attack to go again*/
    
    int   is_boss;                           /**<if true, this will display boss health*/
    float sight_range;                      /**<how far away befor the monster will see the player*/
    float attack_range;                     /**<monster reach for an attack*/
    float damages[DT_MAX];                  /**<types of damage dealt by this entity*/
    float armor[DT_MAX];                    /**<defenses against different damage types*/
    int   connectedAttack;                  // for making sure we don't hit multiple times

    float jumpcool;
    int   jumpcount;                        /**<used for multijump*/
    float jumpdelay;                        /**<configured delay between jumps*/
    int   grounded;    
    int   inWater;                          /**<true if the entity is in water*/
    int   canSink;                          /**<if false, player floads to the surface*/
    
    SJson *args;                            /**<spawn args for the entity*/
    void  *data;                            /**<any other game specific data can be referenced here*/
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
 * @brief add an entity to a collision space
 * @param ent the entity to add
 * @param space the space to add to
 */
void gf2d_entity_add_to_space(Entity *ent,Space *space);

/**
 * @brief remove an entity from a collision space
 * @param ent the entity to remove
 * @param space the space to remove from
 */
void gf2d_entity_remove_from_space(Entity *ent,Space *space);

/**
 * @brief play a sound by its use name
 * @param self the entity to play a sound for
 * @param name the name of the sound (not filename.  name like "attack_sound"
 */
void gf2d_entity_play_sound(Entity *self, const char *name);

/**
 * @brief load a sound and associate it with a name
 * @param self the entity to load a sound for
 * @param name the name to reference the sound by
 * @param file the sound file to load
 * @note if the name is already in use, it will replace (or remove if NULL is used for the file) the file
 */
void gf2d_entity_load_sound(Entity *self, const char *name,const char *file);

/**
 * @brief set an action for an entity
 * @note tries figure first then actor before giving up
 * @note sets the animation frame and action for the entity
 * @param self the entity in question
 * @param action the name of the action to set it to
 * @note if the action name is bad, it defaults to the first action of the figure or actor, if that fails, his does nothing
 * @return the number of frames to complete the action.  If zero it may be due to error
 */
Uint32 gf2d_entity_set_action(Entity *self, const char *action);

/**
 * @brief get the name of the current action if set
 * @param self the entity to query
 * @return NULL on no action or error, or the name of the action otherwise
 */
const char *gf2d_entity_get_action(Entity *self);

/**
 * @brief get the number of animation frames (calls to next frame) remaining for the current action
 * @param self the entity to query
 * @return 0 on NULL or the number of frames remaining (could be zero)
 */
int gf2d_entity_get_action_frames_remaining(Entity *self);

/**
 * @brief get the percent complete of the current action (in integer space 0-100)
 * @param self the entity to query
 * @return 0 on NULL or the number of frames remaining (could be zero)
 */
int gf2d_entity_get_percent_complete(Entity *self);

/**
 * @brief get the number of animation frames (calls to next frame) since the current action began
 * @param self the entity to query
 * @return -1 on error or the number of frames remaining (could be zero)
 */
int gf2d_entity_get_action_frame(Entity *self);

/**
 * @brief get the number of animation frames (calls to next frame) in the current action
 * @param self the entity to query
 * @return -1 on error or the number of frames (could be zero)
 */
int gf2d_entity_get_animation_frames(Entity *self);

/**
 * @brief deal damage to target entity
 * @param target the entity to take damage
 * @param inflictor the entity doing the damage
 * @param attacker the entity getting the credit for the damage
 * @param damage damage affinity array of how much damage by type
 * @param kick damage direction
 * @return true if damage was dealth, false otherwise
 */
int gf2d_entity_deal_damage(Entity *target, Entity *inflictor, Entity *attacker,float *damage,Vector2D kick);

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

/**
 * @brief convert name to damage type
 * @param name of the damage type
 * @return DT_MAX on not a match, or the DT_* in question
 */
DamageTypes gf2d_entity_damage_type_from_name(const char *name);

/**
 * @brief convert damage type to name
 * @param type the DT
 * @return NULL on error or the printable name otherwise
 */
const char *gf2d_entity_damage_type_to_name(DamageTypes type);

/**
 * @brief convert name to entity state
 * @param name of the state
 * @return defaults to ES_Idle, otherwise the parsed name
 */
EntityState gf2d_entity_state_from_name(const char *state);

/**
 * @brief sync the body links to the main body's clip layers
 * @param self the entity
 */
void gf2d_entity_link_layers_sync(Entity *self);

/**
 * @brief Get an entity's bone link by name (of the bone)
 * @param self the entity
 * @param name the search criteria
 * @return NULL on not found, or the link otherwise
 */
EntityLink *gf2d_entity_link_get(Entity *self,const char *name);

/**
 * @brief just get the position of the link (in physics space) of the named link bone
 * @param self the entity ti check
 * @param name the name of the bone link to find
 * @return a zero vector on error or not found, a useful vector otherwise
 */
Vector2D gf2d_entity_link_get_position(Entity *self, const char *name);

#endif
