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
        NULL,
        NULL,
        NULL,
        NULL);
}

List * gf2d_element_actor_update(Element *element,Vector2D offset)
{
    ActorElement *actor;
    if (!element)return NULL;
    actor = (ActorElement*)element->data;
    if (!actor)return NULL;
    gf2d_actor_next_frame(&actor->actor);
    return NULL;
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

Actor *gf2d_element_actor_get_actor(Element *e)
{
    ActorElement *ae;
    if (!e)return NULL;
    ae = (ActorElement *)e->data;
    if (!ae)return NULL;
    return &ae->actor;
}

void gf2d_element_make_actor(Element *e,ActorElement *actor)
{
    if (!e)return;
    e->data = (void*)actor;
    e->type = ET_Actor;
    e->draw = gf2d_element_actor_draw;
    e->update = gf2d_element_actor_update;
    e->free_data = gf2d_element_actor_free;
}

void gf2d_element_load_actor_from_config(Element *e,SJson *json)
{
    SJson *value;
    const char *buffer;
    if ((!e) || (!json))
    {
        slog("call missing parameters");
        return;
    }
    value = sj_object_get_value(json,"actor");
    buffer = sj_get_string_value(value);
    gf2d_element_make_actor(e,gf2d_element_actor_new_full((char *)buffer));
}
/*eol@eof*/
