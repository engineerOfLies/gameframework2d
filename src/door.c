#include "door.h"
#include "player.h"
#include "level.h"
#include "simple_logger.h"
#include "entity_common.h"

void door_draw(Entity *self);
void door_think(Entity *self);
void door_update(Entity *self);
void door_activate(Entity *self,Entity *activator);
int  door_touch(Entity *self,Entity *other);
int  door_damage(Entity *self,int amount, Entity *source);
void door_die(Entity *self);
Entity *door_new(Vector2D position,const char *nextLevel, const char *target, Uint32 id);

Entity *door_spawn(Vector2D position,SJson *args)
{
    int id = 0;
    if (!args)return NULL;
    sj_get_integer_value(sj_object_get_value(args,"targetId"),&id);
    return door_new(position,
                    sj_get_string_value(sj_object_get_value(args,"level")),
                    sj_get_string_value(sj_object_get_value(args,"target")),
                    id);
}

Entity *door_new(Vector2D position,const char *nextLevel, const char *target, Uint32 id)
{
    Entity *self;
    self = gf2d_entity_new();
    if (!self)return NULL;
    
    gf2d_line_cpy(self->name,"door");
    self->parent = NULL;
    
    self->shape = gf2d_shape_rect(-16, -32, 32, 64);
    gf2d_body_set(
        &self->body,
        "door",
        OBJECT_LAYER,
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

    gf2d_actor_load(&self->actor,"actors/door.actor");
    gf2d_actor_set_action(&self->actor,"idle");
    
    self->sound[0] = gf2d_sound_load("sounds/door_fast.wav",1,-1);
    
    vector2d_copy(self->position,position);
    
    vector2d_copy(self->scale,self->actor.al->scale);
    vector2d_set(self->scaleCenter,self->actor.sprite->frame_w/2,self->actor.sprite->frame_h/2);
    vector3d_set(self->rotation,64,64,0);
    vector2d_set(self->flip,0,0);
    self->state = ES_Idle;
    
    gf2d_line_cpy(self->targetLevel,nextLevel);
    gf2d_line_cpy(self->targetEntity,target);
    self->targetId = id;
    
    self->think = door_think;
    self->draw = door_draw;
    self->update = door_update;
    self->touch = door_touch;
    self->damage = door_damage;
    self->die = door_die;
    self->activate = door_activate;
    self->free = level_remove_entity;

    level_add_entity(self);
    return self;
}

void door_draw(Entity *self)
{
    
}

void door_activate(Entity *self,Entity *activator)
{
    if (activator != player_get())return;// only the player can activate
    if (self->state != ES_Idle)return;
    // check if maybe we are locked
    gf2d_sound_play(self->sound[0],0,1,-1,-1);
    gf2d_actor_set_action(&self->actor,"open");
    self->state = ES_Charging;
    activator->state = ES_Leaving;
    activator->cooldown = gf2d_actor_get_frames_remaining(&self->actor);
    slog("player leaving for %i frames",activator->cooldown);
}

void door_think(Entity *self)
{
}

void door_update(Entity *self)
{
    if (self->state == ES_Charging) 
    {
        if (gf2d_actor_get_frames_remaining(&self->actor) == 0)
        {
            slog("transitioning to level %s",self->targetLevel);
            level_transition(self->targetLevel, self->targetEntity, self->targetId);
            self->state = ES_Idle;
        }
    }
}

int  door_touch(Entity *self,Entity *other)
{
    return 0;
}

int  door_damage(Entity *self,int amount, Entity *source)
{
    return 0;
}

void door_die(Entity *self)
{
    
}
/*eol@eof*/
