#include "pickup.h"
#include "player.h"
#include "level.h"
#include "simple_logger.h"
#include "entity_common.h"
#include "gf2d_collision.h"

void pickup_draw(Entity *self);
void pickup_think(Entity *self);
void pickup_update(Entity *self);
int  pickup_touch(Entity *self,Entity *other);
int  pickup_damage(Entity *self,int amount, Entity *source);
void pickup_die(Entity *self);
Entity *pickup_new(Vector2D position,const char *name, char *actorFile);

Entity *pickup_spawn(Vector2D position,SJson *args)
{
    const char *item;
    if (!args)return NULL;
    item = sj_get_string_value(sj_object_get_value(args,"item"));
    if (!item)return NULL;
    // TODO: Make an item system more robust
    if (strcmp(item,"essence") == 0)
    {
        return pickup_new(position,item,"actors/essence.actor");
    }
    else if (strcmp(item,"crysalis") == 0)
    {
        return pickup_new(position,item,"actors/crysalis.actor");
    }
    return NULL;
}

Entity *pickup_new(Vector2D position,const char *name, char *actorFile)
{
    Entity *self;
    self = gf2d_entity_new();
    if (!self)return NULL;
    
    gf2d_line_cpy(self->name,name);
    self->parent = NULL;
    
    self->shape = gf2d_shape_rect(-15, -15, 30, 30);
    gf2d_body_set(
        &self->body,
        (char *)name,
        1,
        PICKUP_LAYER,
        0,
        0,
        position,
        vector2d(0,0),
        10,
        1,
        0,
        &self->shape,
        self,
        NULL);

    gf2d_actor_load(&self->actor,actorFile);
    gf2d_actor_set_action(&self->actor,"idle");
    
    self->sound[0] = gf2d_sound_load("sounds/essence.wav",1,-1);
    
    vector2d_copy(self->position,position);
    
    vector2d_copy(self->scale,self->actor.al->scale);
    vector2d_set(self->scaleCenter,self->actor.sprite->frame_w/2,self->actor.sprite->frame_h/2);
    vector3d_set(self->rotation,64,64,0);
    vector2d_set(self->flip,0,0);
    
    self->think = pickup_think;
    self->draw = pickup_draw;
    self->update = pickup_update;
    self->touch = pickup_touch;
    self->damage = pickup_damage;
    self->die = pickup_die;
    self->free = level_remove_entity;

    level_add_entity(self);
    return self;
}

void pickup_draw(Entity *self)
{
    
}

void pickup_think(Entity *self)
{
    Entity *player;
    player = entity_get_touching_player(self);
    if (player != NULL)
    {
        gf2d_sound_play(self->sound[0],0,1,-1,-1);
        self->dead = 1;            
    }
}

void pickup_update(Entity *self)
{
    entity_apply_gravity(self);
}

int  pickup_touch(Entity *self,Entity *other)
{
    return 0;
}

int  pickup_damage(Entity *self,int amount, Entity *source)
{
    return 0;
}

void pickup_die(Entity *self)
{
    
}
/*eol@eof*/
