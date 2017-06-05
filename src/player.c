#include "player.h"
#include "simple_logger.h"
#include "camera.h"
#include "projectiles.h"
#include "level.h"

static Entity *_player = NULL;

void player_draw(Entity *self);
void player_think(Entity *self);
void player_update(Entity *self);
int  player_touch(Entity *self,Entity *other);
void player_damage(Entity *self,int amount, Entity *source);
void player_die(Entity *self);

#define baseSpeed 3
#define baseAcceleration 3.6

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
    vector2d_copy(_player->position,position);
    vector2d_copy(_player->body.position,position);
}

Entity *player_new(Vector2D position)
{
    Entity *self;
    self = gf2d_entity_new();
    if (!self)return NULL;
    
    gf2d_line_cpy(self->name,"player");
    self->parent = NULL;
    
    
    self->shape = gf2d_shape_rect(-32, -16, 64, 52);
    gf2d_body_set(
        &self->body,
        "player",
//        0,//no layer
        ALL_LAYERS,//all layers
        1,
        position,
        vector2d(baseSpeed,0),
        10,
        0,
        0,
        &self->shape,
        self,
        NULL,
        NULL);

    self->sprite = gf2d_sprite_load_all("images/ed210.png",128,128,16);

    self->frame = 0;
    self->al = gf2d_action_list_load("actors/ed210.actor");
    gf2d_line_cpy(self->action,"idle");
    
    vector2d_copy(self->position,position);
    vector2d_set(self->velocity,baseSpeed,0);
    
    vector2d_set(self->scale,1,1);
    vector2d_set(self->scaleCenter,64,64);
    vector3d_set(self->rotation,64,64,0);
    self->color = gf2d_color8(255,255,255,255);
    vector2d_set(self->flip,1,0);
    
    self->pe = gf2d_particle_emitter_new(1000);
    
    self->think = player_think;
    self->draw = player_draw;
    self->update = player_update;
    self->touch = player_touch;
    self->damage = player_damage;
    self->die = player_die;
    self->free = level_remove_entity;

    _player = self;
    return self;
}

void player_draw(Entity *self)
{
    
}
void player_think(Entity *self)
{
    int mx,my;
    Uint32 buttons;
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL);
    buttons = SDL_GetRelativeMouseState(&mx,&my);
    if ((my < 0)||(keys[SDL_SCANCODE_W]))
    {
        self->acceleration.y = (self->acceleration.y * 0.9) + (-baseAcceleration *0.1);
    }
    if ((my > 0)||(keys[SDL_SCANCODE_S]))
    {
        self->acceleration.y = (self->acceleration.y * 0.9) + (baseAcceleration *0.1);
    }
    if ((mx < 0)||(keys[SDL_SCANCODE_A]))
    {
        self->acceleration.x = (self->acceleration.x * 0.9) + (-baseAcceleration *0.1);
    }
    if ((mx > 0)||(keys[SDL_SCANCODE_D]))
    {
        self->acceleration.x = (self->acceleration.x * 0.9) + (baseAcceleration *0.1);
    }
    if (self->state == ES_Idle)
    {
        if (keys[SDL_SCANCODE_J])
        {
            self->state = ES_Attacking;
            self->frame = gf2d_action_set(self->al,"attack1");
            gf2d_line_cpy(self->action,"attack1");
        }
    }
}

void player_spawn_thrust(Entity *self,Vector2D offset,int count)
{
    Vector2D camPosition;
    int i;
    camPosition = camera_get_position();
    for (i = 0; i < count;i++)
    {
        gf2d_particle_new_full(
            self->pe,
            100,
            NULL,
            NULL,
            vector2d(
                self->position.x - camPosition.x + gf2d_crandom()*2 + offset.x,
                self->position.y - camPosition.y + gf2d_crandom()*2 + offset.y),
            vector2d(
                -self->velocity.x*0.5 + gf2d_crandom()*2.5 - 2,
                (-self->velocity.y*0.5 + gf2d_crandom()*2.5)*0.2),
            vector2d(-self->acceleration.x*0.5 -0.1,-self->acceleration.y*0.05),
            gf2d_color8(10,255,100,255),
            gf2d_color(0,0,0,-0.01),
            PT_Pixel,
            0,
            0,
            0,
            0,
            SDL_BLENDMODE_ADD);
    }
}

void player_update(Entity *self)
{
    Vector2D camPosition = {0,0};
    Rect bounds,lbounds;
    if (!self)return;
    camPosition.x = self->position.x - 200 + (self->velocity.x - baseSpeed)* 2;
    camera_set_position(camPosition);
    Rect level_get_bounds();

    bounds = gf2d_shape_get_bounds(self->shape);
    vector2d_add(bounds,bounds,self->position);
    lbounds = level_get_bounds();

    if ((bounds.y <= lbounds.y + 10) && (self->velocity.y < 0))
    {
        self->acceleration.y = 0;
        self->velocity.y = 0;
    }

    if ((bounds.y + bounds.h >= lbounds.y + lbounds.h - 16) && (self->velocity.y > 0))
    {
        self->acceleration.y = 0;
        self->velocity.y = 0;
    }
    
    self->velocity.y *= 0.999;

    if (self->velocity.x < baseSpeed)self->velocity.x = baseSpeed;
    if (self->velocity.x > 50)self->velocity.x = 50;
    if (self->velocity.y < -50)self->velocity.y = -50;
    if (self->velocity.y > 50)self->velocity.y = 50;
    
    vector2d_scale(self->acceleration,self->acceleration,0.8);
    // make thrust particles

    player_spawn_thrust(self,vector2d(-40,8),50);
    player_spawn_thrust(self,vector2d(-30,-16),25);
    player_spawn_thrust(self,vector2d(-20,16),25);
    switch (self->state)
    {
        case ES_Idle:
            break;
        case ES_Seeking:
            break;
        case ES_Attacking:
            if (self->at == ART_END)
            {
                projectile_new(
                    vector2d(self->position.x + 16,self->position.y + 16),
                    vector2d(140,0),
                    self);
                self->state = ES_Cooldown;
                self->cooldown = 8;
                self->frame = gf2d_action_set(self->al,"idle");
                gf2d_line_cpy(self->action,"idle");
            }
            break;
        case ES_Pain:
        case ES_Cooldown:
            self->cooldown--;
            if (self->cooldown <= 0)
            {
                self->state = ES_Idle;
            }
            break;
        case ES_Dying:
            return;
        case ES_Dead:
            return;
    }
    
}

int player_touch(Entity *self,Entity *other)
{
    return 0;// player does not touch
}

void player_damage(Entity *self,int amount, Entity *source)
{
    
}

void player_die(Entity *self)
{
    
}

/*eol@eof*/
