#include <stdlib.h>
#include <stdio.h>
#include "gf2d_actor_element.h"
#include "simple_logger.h"

void actor_draw(Element *element,Vector2D offset)
{
    ActorElement *actor;
    Vector2D position;
    if (!element)return;
    actor = (ActorElement*)element->data;
    if (!actor)return;
    vector2d_add(position,offset,element->bounds);
    slog("drawing actor at scale %f,%f",actor->scale.x,actor->scale.y);
    gf2d_actor_draw(
        &actor->actor,
        position,
        &actor->scale,
        NULL,
        NULL,
        NULL);
}

int  actor_update(Element *element,Vector2D offset)
{
    return 0;
}

void actor_free(Element *element)
{
    ActorElement *actor;
    if (!element)return;
    actor = (ActorElement*)element->data;
    if (actor != NULL)
    {
        free(actor);
    }
}

ActorElement *gf2d_actor_element_new()
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


ActorElement *gf2d_actor_element_new_full(char *actorFile)
{
    ActorElement *ae;
    ae = gf2d_actor_element_new();
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
    e->draw = actor_draw;
    e->update = actor_update;
    e->free_data = actor_free;
}
/*eol@eof*/
