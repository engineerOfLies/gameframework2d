#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_list.h"

#include "gf2d_camera.h"

#include "gf2d_entity_common.h"
#include "gf2d_message_buffer.h"
#include "gf2d_windows_common.h"
#include "gf2d_particle_effects.h"
#include "level.h"
#include "monsters.h"
#include "player.h"

extern int __DebugMode;
extern int giveAll;

static Entity *_player = NULL;

void player_draw(Entity *self);
void player_think(Entity *self);
void player_update(Entity *self);
int  player_touch(Entity *self,Entity *other);
float player_damage(Entity *self,float *amount, Entity *source);
void player_die(Entity *self);
void player_free(Entity *self);
void player_action_end(Entity *self);
void player_use_equipped_item();

Entity *player_get()
{
    return _player;
}

void player_set_position(Vector2D position)
{
    if (!_player)
    {
        slog("no player loaded");
        return;
    }
    vector2d_copy(_player->body.position,position);
}

Entity *player_spawn(Vector2D position,Uint32 id,SJson *args,const char *entityDef)
{
    if (_player != NULL)
    {
        vector2d_copy(_player->body.position,position);
        level_add_entity(_player);
        return NULL;
    }
    return player_new(position,id,args,entityDef);
}

Entity *player_new(Vector2D position,Uint32 id,SJson *args,const char *entityDef)
{
    Entity *self;
    self = gf2d_entity_new();
    if (!self)return NULL;
    self = entity_config_from_file(self,args,position,entityDef);
    
    gfc_shape_copy(&self->crouchShape,self->shape);
    
    self->crouchShape.s.r.h *= 0.5;
    self->crouchShape.s.r.y += self->crouchShape.s.r.h;//push it down that much
    
    vector2d_set(self->facing,-1,0);
    
    self->think = player_think;
    self->draw = player_draw;
    self->update = player_update;
    self->touch = player_touch;
    self->damage = player_damage;
    self->die = player_die;
    self->free = player_free;
    self->actionEnd = player_action_end;

    self->id = 0;
    _player = self;

    level_add_entity(self);
    return self;
}

void player_draw(Entity *self)
{
    //additional player drawings can go here
}

void player_activation_check(Entity *self)
{
    entity_activate(self);
}

void player_halt(int frames)
{
    if (!_player)return;
    _player->halt = frames;
}

void player_crouch(Entity *self)
{
    if (!self)return;
    if (strcmp(gf2d_entity_get_action(self),"crouching")!= 0)
    {
        gf2d_entity_set_action(self,"crouching");
        self->crouching = 1;
        self->body.shape = &self->crouchShape;
    }
}

void player_stand(Entity *self)
{
    if (!self)return;
    if (!self->crouching)return;
    if (strcmp(gf2d_entity_get_action(self),"crouching")== 0)
    {
        gf2d_entity_set_action(self,"idle");
    }
    self->crouching = 0;
    self->body.shape = &self->shape;
}


void player_think(Entity *self)
{
    if (self == NULL)return;
    if (self->halt < 0)return;// this is a permanent pause until unpaused
    if (self->halt > 0)
    {
        self->halt--;
        return;
    }
    if (gfc_input_command_down("walkleft"))
    {
        if (self->state  == ES_Idle)
        {
            entity_face_from_direction(self, vector2d(-1,0));
        }
        if (!entity_wall_check(self,vector2d(-1,0)))
        {   
            self->body.velocity.x -= self->speed;
        }
        player_stand(self);
    }
    else if (gfc_input_command_down("walkright"))
    {
        if (self->state  == ES_Idle)
        {
            entity_face_from_direction(self, vector2d(1,0));
        }
        if (!entity_wall_check(self,vector2d(1,0)))
        {
            self->body.velocity.x += self->speed;
        }
        player_stand(self);
    }
    else if (gfc_input_command_down("crouch"))
    {
        if ((!self->crouching)&&(self->state == ES_Idle)&&(self->grounded))
        {
            player_crouch(self);
        }
    }
    else player_stand(self);
    switch (self->state)
    {
        case ES_Idle:
            if ((gfc_input_command_down("walkright")||gfc_input_command_down("walkleft"))&&(!self->jumpcool))
            {
                if (gfc_line_cmp(self->figureInstance.action->name,"walk")!=0)
                {
                    gf2d_entity_set_action(self, "walk");
                }
            }
            else if ((gfc_input_command_pressed("jump"))&&(self->grounded)&&(!self->jumpcool))
            {
                self->body.velocity.y = -self->jump_height;
                self->jumpcool = 7;
                gf2d_entity_play_sound(self, "jump");
                gf2d_entity_set_action(self, "jump");
                self->cooldown = self->jumpdelay;
                self->state = ES_Jumping;
            }
            if (gfc_input_command_pressed("activate"))
            {
                self->cooldown  = 30;
                self->state = ES_Cooldown;
                player_activation_check(self);
            }
            if (gfc_input_command_pressed("melee"))
            {
                self->cooldown = gf2d_entity_set_action(self, "attack");
                self->state = ES_Attacking;
                self->attack = 1;
                self->connectedAttack = 0;
                gf2d_entity_play_sound(self, "attack");
            }
            break;
        default:
            player_stand(self);
            break;
    }
}

void player_melee(Entity *self)
{
    if (!self)return;    
    self->connectedAttack = entity_melee_slash_update(
        self,
        self->body.position,
        self->facing,
        35,
        15,
        NULL,
        MONSTER_LAYER,
        1);
}

void player_action_end(Entity *self)
{
    if (!self)return;
    if (self->state >= ES_Dying)return;
    self->state = ES_Idle;
    gf2d_entity_set_action(self, "idle");
}

void player_update(Entity *self)
{
    if (!self)return;

    level_set_light_at(self->body.position,gfc_color8(255,225,200,0),1);
    gf2d_camera_set_focus(self->body.position);
    gf2d_camera_bind();
//cooldowns
    if (self->state == ES_Dying)return;
    self->cooldown-= 1;
    if (self->cooldown <= 0)
    {
        self->cooldown = 0;
        self->state = ES_Idle;
    }
    if (self->jumpcool > 0)
    {
        self->jumpcool -= 0.5;
    }
    else self->jumpcool = 0;

    // walk dampening
    if (self->body.velocity.y)
    {
        if (self->inWater)
        {
            self->body.velocity.y *= 0.95;
        }
    }
    if (self->body.velocity.x)
    {
        if (self->inWater)
        {
            self->body.velocity.x *= 0.7;
        }
        else self->body.velocity.x *= 0.8;
        if (fabs(self->body.velocity.x) < 0.7)
        {
            self->body.velocity.x = 0;
        }
    }
    entity_world_snap(self);    // error correction for collision system
    
    if (self->body.velocity.x == 0)
    {
        if (self->figureInstance.action)
        {
            if (gfc_line_cmp(self->figureInstance.action->name,"walk")==0)
            {
                gf2d_entity_set_action(self, "idle");
            }
        }
    }
    switch (self->state)
    {
        case ES_Idle:
            break;
        case ES_Seeking:
            break;
        case ES_Jumping:
            if (self->jumpcool == 0)
            {
                self->state = ES_Idle;
            }
            break;
        case ES_Attacking:
            player_melee(self);
            break;
        case ES_Leaving:
        case ES_Charging:
        case ES_Pain:
        case ES_Cooldown:
            break;
        case ES_Walking:
        case ES_Dying:
            return;
        case ES_Dead:
            return;
        default:
            return;
    }
    
}

int player_touch(Entity *self,Entity *other)
{
    return 0;// player does not touch
}

float player_damage(Entity *self,float *amount, Entity *source)
{
    Vector2D dir;
    const char *action;
    if (source)
    {
        dir = monster_direction_to_player(source);
    }
    else
    {
        dir.x = -self->facing.x;
        dir.y = -1;
    }
    particle_spray(
        level_get_particle_emitter(),
        self->body.position,
        dir,
        gfc_color8(255,100,100,200),//from config
        40);
    switch(self->state)
    {
        case ES_Dying:
        case ES_Dead:
        case ES_Leaving:
        case ES_Pain:
            return 0;
        default:
            break;
    }
    if (self->damageCooldown)return 0;

    self->health -= *amount;
    self->damageCooldown = 50;
    action = gf2d_entity_get_action(self);
    if ((action)&&(strcmp(action,"pain")!=0))
    {
        gf2d_entity_set_action(self,"pain");
        self->state = ES_Pain;
    }
    gf2d_entity_play_sound(self, "pain");

    //play pain sound, set pain state
    if (self->health <= 0)
    {
        self->health = 0;
        self->state = ES_Dying;
        self->die(self);
    }
    return *amount;//todo factor in shields
}


void player_free(Entity *self)
{
    if (!self)return;
    level_remove_entity(self);
}

void player_dying(Entity *self)
{
    if (!self)return;
    if (gf2d_entity_get_action_frames_remaining(self))return;// play out death first
    self->think = NULL;
    self->halt = -1;
}

void player_die(Entity *self)
{
    if (!self)return;
    gf2d_entity_set_action(self,"death");
    //play a dying sound
    self->think = player_dying;
}

/*eol@eof*/
