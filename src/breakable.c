#include "breakable.h"
#include "player.h"
#include "level.h"
#include "simple_logger.h"
#include "entity_common.h"

void breakable_draw(Entity *self);
void breakable_think(Entity *self);
void breakable_update(Entity *self);
void breakable_activate(Entity *self,Entity *activator);
int  breakable_touch(Entity *self,Entity *other);
int  breakable_damage(Entity *self,int amount, Entity *source);
void breakable_die(Entity *self);

Entity *breakable_new(Vector2D position,const char *actor);

Entity *breakable_spawn(Vector2D position,SJson *args)
{
    int id = 0;
    if (!args)return NULL;
    sj_get_integer_value(sj_object_get_value(args,"targetId"),&id);
    return breakable_new(position,
                    sj_get_string_value(sj_object_get_value(args,"actor")));
}

Entity *breakable_new(Vector2D position,const char *actor)
{
    Entity *self;
    self = gf2d_entity_new();
    if (!self)return NULL;
    
    gf2d_line_cpy(self->name,"breakable");
    self->parent = NULL;
    
    self->shape = gf2d_shape_rect(-16, -16, 32, 32);
    gf2d_body_set(
        &self->body,
        "breakable",
        1,
        WORLD_LAYER|MONSTER_LAYER,
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

    gf2d_actor_load(&self->actor,(char *)actor);
    gf2d_actor_set_action(&self->actor,"idle");
    
    self->sound[0] = gf2d_sound_load("sounds/wood_break.wav",1,-1);
    
    vector2d_copy(self->position,position);
    
    vector2d_copy(self->scale,self->actor.al->scale);
    vector2d_set(self->scaleCenter,self->actor.sprite->frame_w/2,self->actor.sprite->frame_h/2);
    vector3d_set(self->rotation,64,64,0);
    vector2d_set(self->flip,0,0);
    self->state = ES_Idle;
        
    self->think = breakable_think;
    self->draw = breakable_draw;
    self->update = breakable_update;
    self->touch = breakable_touch;
    self->damage = breakable_damage;
    self->die = breakable_die;
    self->activate = breakable_activate;
    self->free = level_remove_entity;

    level_add_entity(self);
    return self;
}

void breakable_draw(Entity *self)
{
    
}

void breakable_activate(Entity *self,Entity *activator)
{
}

void breakable_think(Entity *self)
{
}

void breakable_update(Entity *self)
{
    entity_world_snap(self);    // error correction for collision system
    entity_apply_gravity(self);
    switch(self->state)
    {
        case ES_Dying:
            if (gf2d_actor_get_frames_remaining(&self->actor) == 0)
            {
                self->state = ES_Dead;
                gf2d_actor_set_action(&self->actor,"dead");
            }
            break;
        default:
            break;
    }
}

int  breakable_touch(Entity *self,Entity *other)
{
    return 0;
}

int  breakable_damage(Entity *self,int amount, Entity *source)
{
    slog("CRUNCH");
    if (self->state != ES_Idle)return 0;
    // check if maybe we are locked
    gf2d_sound_play(self->sound[0],0,1,-1,-1);
    gf2d_actor_set_action(&self->actor,"die");
    self->body.cliplayer = 0;
    self->state = ES_Dying;
    return 0;
}

void breakable_die(Entity *self)
{
}
/*eol@eof*/
