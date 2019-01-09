#include "pickup.h"
#include "level.h"
#include "simple_logger.h"
#include "entity_common.h"

void pickup_draw(Entity *self);
void pickup_think(Entity *self);
void pickup_update(Entity *self);
int  pickup_touch(Entity *self,Entity *other);
int  pickup_damage(Entity *self,int amount, Entity *source);
void pickup_die(Entity *self);

Entity *pickup_spawn(Vector2D position,char *actorFile)
{
    Entity *self;
    self = gf2d_entity_new();
    if (!self)return NULL;
    
    gf2d_line_cpy(self->name,"pickup");
    self->parent = NULL;
    
    self->shape = gf2d_shape_rect(-15, -15, 30, 30);
    gf2d_body_set(
        &self->body,
        "pickup",
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
    vector2d_set(self->scaleCenter,64,64);
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
    Collision c;
    Entity *other;
    ClipFilter filter = {0};
    filter.layer = PLAYER_LAYER;
    
    gf2d_space_body_collision_test_filter(level_get_space(),gf2d_body_to_shape(&self->body), &c,filter);
    if (c.collided)
    {
        if ((!c.body)||(!c.body->data))
        {
            slog("error: expected body entity data not found");
            return;
        }
        other = (Entity *)c.body->data;
        // TODO: give item to player
        gf2d_sound_play(self->sound[0],0,1,-1,-1);
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
