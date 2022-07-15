#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_entity_common.h"
#include "gf2d_message_buffer.h"

#include "player.h"
#include "level.h"

void exit_update(Entity *self);
void exit_think(Entity *self);
void exit_free(Entity *self);
void exit_die(Entity *self);
int exit_activate(Entity *self,Entity *activator);

Entity *exit_spawn(Vector2D position,Uint32 id, SJson *args,const char *entityDef)
{
    Entity *self;
    self = gf2d_entity_new();
    if (!self)return NULL;
    self->id = id;
    self->parent = NULL;

    self = entity_config_from_file(self,args,position,entityDef);

    self->think = exit_think;
    self->draw = NULL;
    self->update = exit_update;
    self->touch = NULL;
    self->damage = NULL;
    self->die = exit_die;
    self->free = exit_free;
    self->activate = exit_activate;
    
    level_add_entity(self);
    return self;
}

void exit_go(Entity *self)
{
    int id = 1;
    const char *next_level;
    SJson *keys;
    keys = sj_object_get_value(self->args,"keys");
    if (!keys)return;
    next_level = sj_get_string_value(sj_object_get_value(keys,"next_level"));
    if (!next_level)
    {
        slog("exit does not have next_level set");
        return;
    }
    sj_get_integer_value(sj_object_get_value(keys,"player_enter"),&id);
    level_transition((char *)next_level, "player_enter", id);
}

int exit_activate(Entity *self,Entity *activator)
{
    if ((!self)||(!activator))return 0;
    if ((self->state != ES_Locked)&&(self->state != ES_Open))return 0;
    if (activator == player_get())
    {
        if (self->state == ES_Locked)
        {
            message_new("Locked!");
            return 1;
        }
        if (self->state == ES_Open)
        {
            self->state = ES_Activated;
            player_halt(gf2d_entity_set_action(self, "open"));
        }
        return 1;
    }
    else if (self->state == ES_Locked)
    {
        self->state = ES_Open;
        gf2d_entity_set_action(self, "unlocked");
    }
    return 1;
}

void exit_update(Entity *self)
{
    Vector2D position = {0,-16};
    if (!self)return;
    if (self->light_level > 0)
    {
        vector2d_add(position,position,self->body.position);
        level_set_light_at(position,self->light_color,self->light_level);
    }
}

void exit_think(Entity *self)
{
    Entity *player = NULL;
    if (self->state == ES_Activated)
    {
        if (!gf2d_entity_get_action_frames_remaining(self))
        {
            exit_go(self);
            return;
        }
    }
    if (self->state != ES_Idle)return;
    player = entity_get_touching_player(self);
    if (player == NULL)return;
    exit_go(self);
}

void exit_free(Entity *self)
{
    if (!self)return;
    level_remove_entity(self);
}

void exit_die(Entity *self)
{
    
}

