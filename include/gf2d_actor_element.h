#ifndef __GF2D_ACTOR_ELEMENT_H__
#define __GF2D_ACTOR_ELEMENT_H__

#include "gf2d_elements.h"
#include "gf2d_actor.h"

typedef struct
{
    Actor actor;
    Vector2D scale;
}ActorElement;

ActorElement *gf2d_actor_element_new();

ActorElement *gf2d_actor_element_new_full(char *actorFile);

/**
 * @brief set an element to be the actor provided
 * @param e the element to set
 * @param actor the actor to set it too
 */
void gf2d_element_make_actor(Element *e,ActorElement *actor);

#endif
