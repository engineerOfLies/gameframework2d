#include "simple_logger.h"

#include "gf2d_entity_common.h"

#include "player.h"
#include "level.h"
#include "monsters.h"
#include "bat.h"

void bat_update(Entity *self);
void bat_free(Entity *self);
void bat_die(Entity *self);
int  bat_activate(Entity *self,Entity *activator);
int  bat_damage(Entity *self,float *amount, Entity *source);
void bat_sleep_think(Entity *self);
void bat_die(Entity *self);


Entity *bat_spawn(Vector2D position,Uint32 id, SJson *args,const char *entityDef)
{
    Entity *self;
    self = monster_spawn_common(position,id, args,entityDef);
    if (!self)return NULL;
    self->think = bat_sleep_think;
    self->die = bat_die;
    self->free = bat_free;
    self->activate = bat_activate;
    self->state = ES_Inactive;
    level_add_entity(self);    
    return self;
}

void bat_waking_think(Entity *self)
{
    if (!self->cooldown)
    {
        self->think = monster_think_flying_hunting;
        self->body.gravity = 0;
    }
}

void bat_sleep_think(Entity *self)
{
    if (monster_player_sight_check(self))
    {
        self->think = bat_waking_think;
        gf2d_entity_set_action(self, "flying");
        self->body.gravity = 1;// fall a little
        self->cooldown = 10;
    }
}

int bat_activate(Entity *self,Entity *activator)
{
    if ((!self)||(!activator))return 0;
    if (self->state != ES_Inactive)return 0;
    self->state = ES_Activated;
    self->think = bat_waking_think;
    gf2d_entity_set_action(self, "flying");
    self->body.gravity = 1;// fall a little
    self->cooldown = 10;
    return 1;
}

void bat_die(Entity *self)
{
    self->body.gravity = 1;
    self->body.worldclip = 1;
    if (gf2d_entity_get_action_frames_remaining(self) <= 0)
    {
        self->think = NULL;
        self->body.cliplayer = OBJECT_LAYER;
        gf2d_entity_set_action(self, "dead");        
    }
}

void bat_free(Entity *self)
{
    if (!self)return;
    level_remove_entity(self);
}



