#include "player.h"
#include "simple_logger.h"
#include "camera.h"
#include "projectiles.h"
#include "level.h"
#include "gui.h"

static Entity *_player = NULL;

void player_draw(Entity *self);
void player_think(Entity *self);
void player_update(Entity *self);
int  player_touch(Entity *self,Entity *other);
int  player_damage(Entity *self,int amount, Entity *source);
void player_die(Entity *self);

typedef struct
{
    int baseSpeed;
    int maxSpeed;
    float baseAcceleration;
    float shields,shieldMax;
    float charge,chargeMax;
    float energy,energyMax;
    int capacitors,capacitorsMax;
    int attackType;
}PlayerData;


static PlayerData playerData = {
    3,50,3.6,
    0,100,
    0,100,
    0,100,
    0,0};

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
        vector2d(playerData.baseSpeed,0),
        10,
        0,
        0,
        &self->shape,
        self,
        NULL,
        NULL);

    gf2d_actor_load(&self->actor,"actors/ed210.actor");
    gf2d_actor_set_action(&self->actor,"idle");
    self->sound = gf2d_sound_load("audio/plasma_fire.wav",1,-1);

    self->health = self->maxHealth = 100;
    
    vector2d_copy(self->position,position);
    vector2d_set(self->velocity,playerData.baseSpeed,0);
    
    vector2d_set(self->scale,1,1);
    vector2d_set(self->scaleCenter,64,64);
    vector3d_set(self->rotation,64,64,0);
    vector2d_set(self->flip,1,0);
    
    self->pe = gf2d_particle_emitter_new(1000);
    
    self->think = player_think;
    self->draw = player_draw;
    self->update = player_update;
    self->touch = player_touch;
    self->damage = player_damage;
    self->die = player_die;
    self->free = level_remove_entity;

    self->data = (void*)&playerData;
    _player = self;
    return self;
}

void player_draw(Entity *self)
{
    gui_set_health(self->health/(float)self->maxHealth);
    gui_set_thrust(self->velocity.x/playerData.maxSpeed);
    gui_set_charge(playerData.charge/playerData.chargeMax);
}

void player_think(Entity *self)
{
    int mx,my;
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL);
    SDL_GetRelativeMouseState(&mx,&my);
    if ((my < 0)||(keys[SDL_SCANCODE_W]))
    {
        self->acceleration.y = (self->acceleration.y * 0.9) + (-playerData.baseAcceleration *0.1);
    }
    if ((my > 0)||(keys[SDL_SCANCODE_S]))
    {
        self->acceleration.y = (self->acceleration.y * 0.9) + (playerData.baseAcceleration *0.1);
    }
    if ((mx < 0)||(keys[SDL_SCANCODE_A]))
    {
        self->acceleration.x = (self->acceleration.x * 0.9) + (-playerData.baseAcceleration *0.1);
    }
    if ((mx > 0)||(keys[SDL_SCANCODE_D]))
    {
        self->acceleration.x = (self->acceleration.x * 0.9) + (playerData.baseAcceleration *0.1);
    }
    if (self->state == ES_Idle)
    {
        if (keys[SDL_SCANCODE_J])
        {
            self->state = ES_Attacking;
            gf2d_actor_set_action(&self->actor,"attack1");
            playerData.attackType = 0;
        }
        if (keys[SDL_SCANCODE_K])
        {
            self->state = ES_Charging;
            gf2d_actor_set_action(&self->actor,"attack3");
            playerData.charge = 0;
            playerData.attackType = 1;
        }
    }
    else if (self->state == ES_Charging)
    {
        if (keys[SDL_SCANCODE_K])
        {
            // continue to charge
            if (playerData.charge < playerData.chargeMax)
            {
                playerData.charge += 10;
            }
        }
        else
        {
            // release
            self->state = ES_Attacking;
            gf2d_actor_set_action(&self->actor,"attack2");
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
    if (!self)return;
    camPosition.x = self->position.x - 200 + (self->velocity.x - playerData.baseSpeed)* 2;
    camera_set_position(camPosition);

    level_bounds_clamp(self);

        /*velocity checks*/
    self->velocity.y *= 0.999;
    if (self->velocity.x < playerData.baseSpeed)self->velocity.x = playerData.baseSpeed;
    if (self->velocity.x > playerData.maxSpeed)self->velocity.x = playerData.maxSpeed;
    if (self->velocity.y < -playerData.maxSpeed)self->velocity.y = -playerData.maxSpeed;
    if (self->velocity.y > playerData.maxSpeed)self->velocity.y = playerData.maxSpeed;
    
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
        case ES_Charging:
            // play sound
            // emit particles
            break;
        case ES_Attacking:
            if (self->actor.at == ART_END)
            {
                switch(playerData.attackType)
                {
                    case 0:
                        projectile_new(
                            vector2d(self->position.x + 16,self->position.y + 16),
                            vector2d(140,0),
                            5,
                            0.25,
                            self,
                            "actors/plasma_bolt.actor");
                        self->state = ES_Cooldown;
                        self->cooldown = 8;
                        gf2d_sound_play(self->sound,0,0.2,-1,-1);
                        break;
                    case 1:
                        projectile_new(
                            vector2d(self->position.x + 16,self->position.y + 10),
                            vector2d(120,0),
                            (playerData.charge/playerData.chargeMax)*50,
                            0.5 + (playerData.charge/playerData.chargeMax)*1.5,
                            self,
                            "actors/charge_bolt.actor");
                        self->state = ES_Cooldown;
                        self->cooldown = 8+(playerData.charge*0.5);
                        break;
                }
                playerData.charge = 0;
                gf2d_actor_set_action(&self->actor,"idle");
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

int player_damage(Entity *self,int amount, Entity *source)
{
    slog("player taking %i damage!",amount);
    self->health -= amount;
    if (self->health <= 0)self->health = 0;
    self->die(self);
    return amount;//todo factor in shields
}

void player_die(Entity *self)
{
    
}

/*eol@eof*/
