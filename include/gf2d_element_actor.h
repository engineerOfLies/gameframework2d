#ifndef __GF2D_ACTOR_ELEMENT_H__
#define __GF2D_ACTOR_ELEMENT_H__

#include "gf2d_elements.h"
#include "gf2d_actor.h"

typedef struct
{
    Actor actor;
    Vector2D scale;
}ActorElement;

ActorElement *gf2d_element_actor_new();

ActorElement *gf2d_element_actor_new_full(char *actorFile);

/**
 * @brief get a pointer to the actor data in this element
 * @param e the element to get an actor from
 * @return NULL if it is not set or an error, the actor otherwise
 */
Actor *gf2d_element_actor_get_actor(Element *e);

/**
 * @brief set an element to be the actor provided
 * @param e the element to set
 * @param actor the actor to set it too
 */
void gf2d_element_make_actor(Element *e,ActorElement *actor);

/**
 * @brief load actor configuration for an actor element from config
 * @param e the element to configure
 * @param json the json config to use
 */
void gf2d_element_load_actor_from_config(Element *e,SJson *json);

#endif
