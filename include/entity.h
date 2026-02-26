#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <SDL.h>

#include "gfc_text.h"
#include "gfc_shape.h"

#include "gf2d_sprite.h"

typedef enum
{
    EL_NONE = 0,
    EL_Player = 1,
    EL_Monster = 2,
    EL_Item = 4,
    EL_World = 8,
    EL_Projectiles = 16,
    EL_ALL = 31
}EntityLayers;

typedef struct Entity_S
{
    Uint8           _inuse;                         //no touchy
    Uint8           team;                           // same team no touchy
    Uint32          layer;                          // which layer am I on
    Uint32          id;                             //unique ID to this entity
    GFC_Color       color;                          //lets mod outselves by this color
    GFC_TextLine    name;                           //name of the entity for debugging purposes
    GFC_Vector2D    position;
    GFC_Vector2D    velocity;                       //movement vector
    GFC_Rect        bounds;                         //my bounding box
    float           topSpeed;
    GFC_Vector2D    scale;
    GFC_Vector2D    rotationCenter;
    float           rotation;
    Sprite         *sprite;
    float           frame;
    void          (*think)(struct Entity_S *self);  //called every frame if defined for the entity
    void          (*update)(struct Entity_S *self); //called every frame if defined for the entity
    Uint8         (*touch)(struct Entity_S *self,struct Entity_S *other); //return 1 if you want to stop movement
    void          (*free)(struct Entity_S *self);   //called whent the entity is freed
    void           *data;                           //used for entity specific data
}Entity;

/**
 * @brief initialize the entity sub system
 * @param max the upper limit for concurrent entities
 */
void entity_manager_init(Uint32 max);

/**
 * @brief draw all active entities
 */
void entity_manager_draw_all();

/**
 * @brief all active entities run their think
 */
void entity_manager_think_all();

/**
 * @brief all active entities run their update
 */
void entity_manager_update_all();


/**
 * @brief get a pointer to a free entity
 * @return NULL if out of entities, a pointer to a blank entity otherwise
 */
Entity *entity_new();

/**
 * @brief free an entity
 * @param self the entity to free
 * @note do not use the memory address again after call this
 */
void entity_free(Entity *self);

/**
 * @brief draw an entity
 */
void entity_draw(Entity *self);

/**
 * @brief check if an entity is colliding with any other entity
 */
Uint8 entity_collision_test_world(Entity *self);

#endif
