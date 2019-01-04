#include "player.h"
#include "simple_logger.h"
#include "camera.h"
#include "level.h"
#include "entity_common.h"

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
    
    self->shape = gf2d_shape_rect(-32, -32, 30, 60);
    gf2d_body_set(
        &self->body,
        "player",
//        0,//no layer
        ALL_LAYERS,//all layers
        1,
        position,
        vector2d(0,0),
        10,
        1,
        0,
        &self->shape,
        self,
        NULL,
        NULL);

    gf2d_actor_load(&self->actor,"actors/ed210.actor");
    gf2d_actor_set_action(&self->actor,"stand");

    self->sound[0] = gf2d_sound_load("sounds/jump_10.wav",1,-1);

    
    vector2d_copy(self->position,position);
    
    vector2d_copy(self->scale,self->actor.al->scale);
    vector2d_set(self->scaleCenter,64,64);
    vector3d_set(self->rotation,64,64,0);
    vector2d_set(self->flip,1,0);
    
    self->think = player_think;
    self->draw = player_draw;
    self->update = player_update;
    self->touch = player_touch;
    self->damage = player_damage;
    self->die = player_die;
    self->free = level_remove_entity;

    self->data = (void*)&playerData;
    _player = self;
    level_add_entity(self);
    return self;
}

void player_draw(Entity *self)
{
    //additional player drawings can go here
}

void player_think(Entity *self)
{
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL);
    if ((keys[SDL_SCANCODE_A]) && (!entity_left_check(self,1)))
    {   
        self->velocity.x -= 1.5;
    }
    if ((keys[SDL_SCANCODE_D]) && (!entity_right_check(self,1)))
    {
        self->velocity.x += 1.5;
    }
    if (((keys[SDL_SCANCODE_SPACE])&&(self->grounded))&&(!self->jumpcool))
    {
        self->velocity.y -= 10;
        self->jumpcool = 5;
        gf2d_sound_play(self->sound[0],0,1,-1,-1);
    }
}

void player_update(Entity *self)
{
    Vector2D camPosition = {0,0};
    if (!self)return;
    camera_set_position(camPosition);
    if (self->jumpcool > 0) self->jumpcool -= 0.2;
    else self->jumpcool = 0;
    
    //world clipping
    if (entity_right_check(self,0.1))
    {
        self->position.x -= 0.1;
        slog("right wall");
    }
    if (entity_left_check(self,0.1))
    {
        self->position.x += 0.1;
        slog("left wall");
    }
    
    // walk dampening
    if (self->velocity.x)
    {
        self->velocity.x *= 0.9;
        if (fabs(self->velocity.x) < 1)self->velocity.x = 0;
    }
    self->velocity.y += 0.58;
    if (entity_ground_check(self,1))
    {
        slog("on the ground");
        if (self->velocity.y > 0)self->velocity.y = 0;
        self->grounded = 1;
    }
    else
    {
        self->grounded = 0;
        slog("in the air");
    }
    if (entity_ground_check(self,0.1))
    {
        self->position.y -= 0.1;
    }
    
    switch (self->state)
    {
        case ES_Idle:
            break;
        case ES_Seeking:
            break;
        case ES_Charging:
        case ES_Attacking:
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
