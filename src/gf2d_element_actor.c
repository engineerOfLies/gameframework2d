#include <stdlib.h>
#include <stdio.h>
#include "gf2d_element_actor.h"
#include "simple_logger.h"

void gf2d_element_actor_draw(Element *element,Vector2D offset)
{
    ActorElement *actor;
    Vector2D position;
    if (!element)return;
    actor = (ActorElement*)element->data;
    if (!actor)return;
    vector2d_add(position,offset,element->bounds);
    gf2d_actor_draw(
        &actor->actor,
        position,
        &actor->scale,
        NULL,
        NULL,
        NULL);
}

int  gf2d_element_actor_update(Element *element,Vector2D offset)
{
    ActorElement *actor;
    if (!element)return 0;
    actor = (ActorElement*)element->data;
    if (!actor)return 0;
    gf2d_actor_next_frame(&actor->actor);
    return 0;
}

void gf2d_element_actor_free(Element *element)
{
    ActorElement *actor;
    if (!element)return;
    actor = (ActorElement*)element->data;
    if (actor != NULL)
    {
        free(actor);
    }
}

ActorElement *gf2d_element_actor_new()
{
    ActorElement *actor;
    actor = (ActorElement *)malloc(sizeof(ActorElement));
    if (!actor)
    {
        slog("failed to allocate memory for actor");
        return NULL;
    }
    memset(actor,0,sizeof(ActorElement));
    return actor;
}


ActorElement *gf2d_element_actor_new_full(char *actorFile)
{
    ActorElement *ae;
    ae = gf2d_element_actor_new();
    if (!ae)
    {
        return NULL;
    }
    gf2d_actor_load(&ae->actor,actorFile);
    vector2d_copy(ae->scale,ae->actor.al->scale);
    return ae;
}

void gf2d_element_make_actor(Element *e,ActorElement *actor)
{
    if (!e)return;
    e->data = (void*)actor;
    e->draw = gf2d_element_actor_draw;
    e->update = gf2d_element_actor_update;
    e->free_data = gf2d_element_actor_free;
}
/*eol@eof*/
