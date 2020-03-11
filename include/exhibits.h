#ifndef __EXHIBITS_H__
#define __EXHIBITS_H__

#include "simple_json.h"
#include "gfc_types.h"
#include "gfc_text.h"
#include "gf2d_entity.h"
#include "gf2d_mouse.h"
#include "gf2d_shape.h"

typedef struct
{
    TextLine    name;           /**<exhibit name*/
    Bool        displayName;    /**<if the interaction needs to be displayed.*/
    TextLine    actor;          /**<actor to use for this exhibit*/
    TextLine    action;         /**<action to use for the actor for the exhibit*/
    Rect        rect;           /**<screen rectangle for interaction*/
    Bool        proximity;      /**<if the player needs to be close to the exhibit to interact with it*/
    Vector2D    near;           /**<where the player needs to stand to interact with it.*/
    SJson      *args;           /**<additional arguments*/
    Entity     *entity;         /**<pointer to the entity spawned by this exhibit*/
}Exhibit;

/**
 * Allocate a new exhibit
 */
Exhibit *exhibit_new();

/**
 * @brief Load an exhibit from json data
 * @return NULL on error or an allocated and initialized exhibit
 */
Exhibit *exhibit_load(SJson *json);

Entity *exhibit_entity_spawn(Exhibit *exhibit); 

void exhibit_free(Exhibit *exhibit);

/**
 * @brief check if the exhibit has been interacted with by the mouse
 * @param exhibit pointer to the exhibit to check
 * @returns false if no interaction, true if there was an interaction.
 */
int exhibit_mouse_check(Exhibit *exhibit);
#endif
