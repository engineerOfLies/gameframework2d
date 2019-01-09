#include "monster.h"
#include "level.h"
#include "simple_logger.h"
#include "entity_common.h"

void monster_draw(Entity *self);
void monster_think(Entity *self);
void monster_update(Entity *self);
int  monster_touch(Entity *self,Entity *other);
int  monster_damage(Entity *self,int amount, Entity *source);
void monster_die(Entity *self);

Entity *monster_spawn(Vector2D position,char *actorFile)
{
    Entity *self;
    self = gf2d_entity_new();
    if (!self)return NULL;
    
    gf2d_line_cpy(self->name,"monster");
    self->parent = NULL;
    
    self->shape = gf2d_shape_rect(-32, -16, 60, 30);
    gf2d_body_set(
        &self->body,
        "monster",
//        0,//no layer
        ALL_LAYERS,//all layers
        2,
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
    
    self->sound[0] = gf2d_sound_load("sounds/jump_10.wav",1,-1);

    
    vector2d_copy(self->position,position);
    
    vector2d_copy(self->scale,self->actor.al->scale);
    vector2d_set(self->scaleCenter,64,64);
    vector3d_set(self->rotation,64,64,0);
    vector2d_set(self->flip,0,0);
    
    self->think = monster_think;
    self->draw = monster_draw;
    self->update = monster_update;
    self->touch = monster_touch;
    self->damage = monster_damage;
    self->die = monster_die;
    self->free = level_remove_entity;

    level_add_entity(self);
    return self;
}

void monster_draw(Entity *self)
{
    
}

void monster_think(Entity *self)
{
    
}

void monster_update(Entity *self)
{
    entity_apply_gravity(self);
}

int  monster_touch(Entity *self,Entity *other)
{
    slog("monster touch called");
    if (!other)return 0;
    if (gf2d_line_cmp(other->name,"player") != 0)return 0;
    entity_damage(other,self,5,10);
    return 0;
}

int  monster_damage(Entity *self,int amount, Entity *source)
{
    return 0;
}

void monster_die(Entity *self)
{
    
}
/*eol@eof*/
