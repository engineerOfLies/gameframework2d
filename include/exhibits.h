#ifndef __EXHIBITS_H__
#define __EXHIBITS_H__

#include "simple_json.h"

#include "gfc_types.h"
#include "gfc_text.h"

#include "gf2d_entity.h"
#include "gf2d_mouse.h"
#include "gf2d_shape.h"
#include "scene.h"

typedef struct
{
    TextLine    name;           /**<exhibit name*/
    Bool        displayName;    /**<if the interaction needs to be displayed.*/
    TextLine    actor;          /**<actor to use for this exhibit*/
    TextLine    action;         /**<action to use for the actor for the exhibit*/
    Rect        rect;           /**<screen rectangle for interaction*/
    Vector2D    near;           /**<where the player needs to stand to interact with it.*/
    SJson      *args;           /**<additional arguments*/
    Entity     *entity;         /**<pointer to the entity spawned by this exhibit*/
    Scene      *scene;          /**<the scene this exhibit is attached to*/
    TextLine    command;        /**<when interacting with the exhibit this commend is the active one*/
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

void exhibit_set_scene(Exhibit *exhibit,Scene *scene);

/**
 * @brief check if the exhibit has been interacted with by the mouse
 * @param exhibit pointer to the exhibit to check
 * @returns false if no interaction, true if there was an interaction.
 */
int exhibit_mouse_check(Exhibit *exhibit);

/**
 * @brief check if the mouse is over the exhibit in question
 */
int exhibit_mouse_over(Exhibit *exhibit);

/**
 * @brief search a scene for an exhibit by name
 * @param scene the scene to search
 * @param name the name to search for
 * @return NULL on error or not found, or a pointer to the exhibit data otherwise
 */
Exhibit *exhibit_get_from_scene(Scene *scene,char *name);

/**
 * @brief get the exhibit currently under the mouse;
 * @param scene the scene to search
 * @return NULL on error or not found, an exhibit the mouse is over otherwise
 */
Exhibit *exhibit_get_mouse_over_from_scene(Scene *scene);

#endif
