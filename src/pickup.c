#include "pickup.h"
#include "player.h"
#include "level.h"
#include "simple_logger.h"
#include "entity_common.h"

void pickup_draw(Entity *self);
void pickup_think(Entity *self);
void pickup_update(Entity *self);
int  pickup_touch(Entity *self,Entity *other);
int  pickup_damage(Entity *self,int amount, Entity *source);
void pickup_die(Entity *self);
Entity *pickup_new(Vector2D position,const char *name, char *actorFile);

void pickup_spawn(Vector2D position,SJson *args)
{
    const char *item;
    if (!args)return;
    item = sj_get_string_value(sj_object_get_value(args,"item"));
    if (!item)return;
    // TODO: Make an item system more robust
    if (strcmp(item,"essence") == 0)
    {
        pickup_new(position,item,"actors/essence.actor");
    }
    else if (strcmp(item,"crysalis") == 0)
    {
        pickup_new(position,item,"actors/crysalis.actor");
    }
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
//        0,//no layer
        PICKUP_LAYER | WORLD_LAYER,//all layers
        0,
        position,
        vector2d(0,0),
        10,
        1,
        0,
        &self->shape,
        self,
        NULL,
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
    Collision c = {0};
    Shape shape;
    Entity *other;
    ClipFilter filter = {0};
    filter.layer = PLAYER_LAYER;
    filter.ignore = &self->body;
    slog("item %s is thinking...",self->name);
    shape = gf2d_body_to_shape(&self->body);
    gf2d_space_body_collision_test_filter(level_get_space(),shape, &c,filter);
    if (c.collided)
    {
        if ((!c.body)||(!c.body->data))
        {
            return;
        }
        other = (Entity *)c.body->data;
        if (other != player_get())
        {
            slog("other entity is not a player");
            return;
        }
        // TODO: give item to player
        gf2d_sound_play(self->sound[0],0,1,-1,-1);
        slog("item %s picked up by %s!",self->name,other->name);
        self->dead = 1;
        return;    
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
