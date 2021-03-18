#include <stdlib.h>
#include <stdio.h>
#include "simple_logger.h"

#include "gf2d_element_actor.h"

void gf2d_element_actor_draw(Element *element,Vector2D offset)
{
    ActorElement *actor;
    Vector2D position;
    if (!element)return;
    actor = (ActorElement*)element->data;
    if (!actor)return;
    vector2d_add(position,offset,element->bounds);
    if (actor->image)
    {
        gf2d_sprite_draw_image(actor->image,position);
    }
    else
    {
        gf2d_actor_draw(
            &actor->actor,
            position,
            &actor->scale,
            NULL,
            NULL,
            NULL);
    }
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
        gf2d_actor_free(&actor->actor);
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


ActorElement *gf2d_element_actor_new_full(char *actorFile, char *action,Vector2D scale,const char *image)
{
    ActorElement *ae;
    ae = gf2d_element_actor_new();
    if (!ae)
    {
        return NULL;
    }
    vector2d_copy(ae->scale,scale);
    if ((actorFile)&&(strlen(actorFile))&&(gf2d_actor_load(&ae->actor,actorFile)))
    {
        if ((action) && (strlen(action) > 0))
        {
            gf2d_actor_set_action(&ae->actor,action);
        }
    }
    else if (image != NULL)
    {
        ae->image = gf2d_sprite_load_image((char *)image);
    }
    return ae;
}

const char *gf2d_element_actor_get_action_name(Element *e)
{
    ActorElement *ae;
    if (!e)return NULL;
    ae = (ActorElement *)e->data;
    return gf2d_actor_get_action_name(&ae->actor);
}

void gf2d_element_actor_next_action(Element *e)
{
    ActorElement *ae;
    if (!e)return;
    ae = (ActorElement *)e->data;
    gf2d_actor_next_action(&ae->actor);
}

void gf2d_element_actor_auto_scale(Element *e)
{
    ActorElement *ae;
    if (!e)return;
    ae = (ActorElement *)e->data;
    ae->scale.x = e->bounds.w/ ae->actor.size.x ;
    ae->scale.y = e->bounds.h /ae->actor.size.y;
}

void gf2d_element_actor_set_actor(Element *e, char *actorFile)
{
    ActorElement *ae;
    if (!e)return;
    ae = (ActorElement *)e->data;
    gf2d_actor_free(&ae->actor);
    gf2d_actor_load(&ae->actor,actorFile);
}

void gf2d_element_actor_set_action(Element *e, char *action)
{
    ActorElement *ae;
    if (!e)return;
    ae = (ActorElement *)e->data;
    gf2d_actor_set_action(&ae->actor,action);
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
    const char *buffer = NULL;
    const char *action = NULL;
    const char *image = NULL;
    Vector2D scale;
    if ((!e) || (!json))
    {
        slog("call missing parameters");
        return;
    }
    value = sj_object_get_value(json,"actor");
    if (value)
    {
        buffer = sj_get_string_value(value);
        value = sj_object_get_value(json,"action");
        if (value)
        {
            action = sj_get_string_value(value);
        }
    }
    else
    {
        value = sj_object_get_value(json,"image");
        image = sj_get_string_value(value);
    }
    

    scale.x = scale.y = 1;
    sj_value_as_vector2d(sj_object_get_value(json,"scale"),&scale);
    gf2d_element_make_actor(e,gf2d_element_actor_new_full((char *)buffer,(char *)action,scale,image));
}
/*eol@eof*/
