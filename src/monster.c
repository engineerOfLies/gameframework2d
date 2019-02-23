#include "monster.h"
#include "level.h"
#include "player.h"
#include "simple_logger.h"
#include "entity_common.h"

void monster_draw(Entity *self);
void monster_think(Entity *self);
void monster_update(Entity *self);
int  monster_touch(Entity *self,Entity *other);
int  monster_damage(Entity *self,int amount, Entity *source);
void monster_die(Entity *self);
int monster_player_sight_check(Entity *self);
void monster_think_hunting(Entity *self);

Entity *monster_new(Vector2D position,char *actorFile);

Entity *monster_spawn(Vector2D position,SJson *args)
{
    return monster_new(position,"actors/space_bug.actor");
}


Entity *monster_new(Vector2D position,char *actorFile)
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
        WORLD_LAYER | MONSTER_LAYER,//all layers
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
    
    self->sound[0] = gf2d_sound_load("sounds/bug_attack1.wav",1,-1);
    self->sound[1] = gf2d_sound_load("sounds/bug_pain1.wav",1,-1);

    
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

    self->health = self->maxHealth = 20;
    level_add_entity(self);
    return self;
}

void monster_draw(Entity *self)
{
    
}

void monster_think_attacking(Entity *self)
{
    Entity *other = NULL;
    Collision c = {0};
    if (gf2d_actor_get_frames_remaining(&self->actor) == 1)
    {
        slog("checking for hit");
        c = entity_block_hit(self,gf2d_rect(self->position.x + 32 + (self->flip.x * -64),self->position.y-8,16,32));
        if (c.collided)
        {
            other = c.body->data;
            slog("HIT %s",other->name);
            entity_damage(other,self,5,5);
        }

    }
    if (gf2d_actor_get_frames_remaining(&self->actor) <= 0)
    {
        slog("back to search");
        self->think = monster_think_hunting;
    }
}

void monster_attack(Entity *self)
{
    slog("attacking player");
    gf2d_actor_set_action(&self->actor,"attack1");
    self->think = monster_think_attacking;
    gf2d_sound_play(self->sound[1],0,1,-1,-1);
}

void monster_think_hunting(Entity *self)
{
    Entity *player = player_get();
    if (self->jumpcool)return;
    //monster loses sight of player
    if (vector2d_magnitude_compare(vector2d(self->position.x - player->position.x,self->position.y - player->position.y),500) > 0)
    {
        slog("lost the player");
        self->think = monster_think;// idle think
        return;
    }
    //monster gets in range of player
    if (vector2d_magnitude_compare(vector2d(self->position.x - player->position.x,self->position.y - player->position.y),100) < 0)
    {
        monster_attack(self);
        return;
    }
    slog("moving towards player");
    // jump to player
    self->jumpcool = gf2d_actor_get_frames_remaining(&self->actor);
    self->velocity.y -= 10;
    if (player->position.x < self->position.x)
    {
        self->flip.x = 0;
        self->velocity.x -= 15;
    }
    if (player->position.x > self->position.x)
    {
        self->flip.x = 1;
        self->velocity.x += 15;
    }
}

void monster_think(Entity *self)
{
    if (monster_player_sight_check(self))
    {
        self->think = monster_think_hunting;
    }
}

void monster_update(Entity *self)
{
    if (self->jumpcool > 0) self->jumpcool -= 0.2;
    else self->jumpcool = 0;
    //world clipping

    
    // walk dampening
    if (self->velocity.x)
    {
        self->velocity.x *= 0.8;
        if (fabs(self->velocity.x) < 1)self->velocity.x = 0;
    }
    entity_apply_gravity(self);
    entity_world_snap(self);    // error correction for collision system
}

int  monster_touch(Entity *self,Entity *other)
{
    slog("monster touch called");
    if (!other)return 0;
    if (gf2d_line_cmp(other->name,"player") != 0)return 0;
    entity_damage(other,self,5,5);
    entity_push(self,other,5);
    return 0;
}

int  monster_damage(Entity *self,int amount, Entity *source)
{
    slog("monster taking %i damage!",amount);
    self->health -= amount;
    gf2d_sound_play(self->sound[1],0,0.1,-1,-1);
    if (self->health <= 0)
    {
        self->health = 0;
        self->die(self);
    }
    return amount;//todo factor in shields}
}

int monster_player_sight_check(Entity *self)
{
    Entity *player = player_get();
    if (!player)return 0;
    if (vector2d_magnitude_compare(vector2d(self->position.x - player->position.x,self->position.y - player->position.y),100) < 0)
    {
        gf2d_sound_play(self->sound[0],0,1,-1,-1);
        return 1;
    }
    return 0;
}

void monster_die(Entity *self)
{
    self->dead = 1;
}
/*eol@eof*/
