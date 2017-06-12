#include "space_bug.h"
#include "simple_logger.h"
#include "camera.h"
#include "level.h"
#include "particle_effects.h"
#include "items.h"
#include "player.h"
#include "entity_common.h"

void space_bug_draw(Entity *self);
void space_bug_think(Entity *self);
void space_bug_update(Entity *self);
int  space_bug_touch(Entity *self,Entity *other);
int  space_bug_damage(Entity *self,int amount, Entity *source);
void space_bug_die(Entity *self);

Entity *space_bug_new(Vector2D position)
{
    Entity *self;
    self = gf2d_entity_new();
    if (!self)return NULL;
    
    gf2d_line_cpy(self->name,"space_bug");
    self->parent = NULL;
    
    
    self->shape = gf2d_shape_rect(-34, -16, 64, 58);
    gf2d_body_set(
        &self->body,
        "space_bug",
        0,
        2,
        position,
        vector2d(0,0),
        10,
        0,
        0,
        &self->shape,
        self,
        NULL,
        NULL);

    gf2d_actor_load(&self->actor,"actors/space_bug.actor");
    gf2d_actor_set_action(&self->actor,"idle");
    
    self->sound[0] = gf2d_sound_load("audio/squishy2.wav",1,-1);
    
    vector2d_copy(self->position,position);
    
    vector2d_copy(self->scale,self->actor.al->scale);
    vector2d_set(self->scaleCenter,64,64);
    vector3d_set(self->rotation,64,64,0);
    
    self->pe = gf2d_particle_emitter_new(250);
    
    self->think = space_bug_think;
    self->draw = space_bug_draw;
    self->update = space_bug_update;
    self->touch = space_bug_touch;
    self->damage = space_bug_damage;
    self->die = space_bug_die;
    self->free = level_remove_entity;
    
    self->health = 20;
    self->maxHealth = 20;
    self->count = gf2d_random()*16;

    return self;
}

void space_bug_draw(Entity *self)
{
    
}

void space_bug_think(Entity *self)
{
    Entity * player;
    if (self->state != ES_Seeking)return;
    if (!entity_camera_view(self))
    {
        return;// off the map, don't think
    }
    self->acceleration.y = sin(SDL_GetTicks() + self->count)*6;
    player = player_get();
    if (!player)return;
    if (self->position.y < player->position.y)self->acceleration.y += 2;
    if (self->position.y > player->position.y)self->acceleration.y -= 2;
}

void space_bug_spawn_thrust(Entity *self,Vector2D offset,int count)
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
                self->position.x - camPosition.x + gf2d_crandom()*5 + offset.x,
                self->position.y - camPosition.y + gf2d_crandom()*5 + offset.y),
            vector2d(
                -self->velocity.x*0.5 + gf2d_crandom()*2.5 + 2,
                (-self->velocity.y*0.5 + gf2d_crandom()*2.5)*0.2),
            vector2d(-self->acceleration.x*0.5 + 0.1,-self->acceleration.y*0.05),
            gf2d_color8(10,100,255,255),
            gf2d_color(0,0,0,-0.01),
            PT_Pixel,
            0,
            0,
            0,
            0,
            SDL_BLENDMODE_ADD);
    }
}

void space_bug_update(Entity *self)
{
    Rect camera;
    Rect bounds,lbounds;
    float r;
    if (!self)return;
    camera = camera_get_dimensions();

    bounds = gf2d_shape_get_bounds(self->shape);
    vector2d_add(bounds,bounds,self->position);
    lbounds = level_get_bounds();
    if ((bounds.y <= lbounds.y + 10) && (self->velocity.y < 0))
    {
        self->acceleration.y = 0;
        self->velocity.y = 0;
    }
    if ((bounds.y + bounds.h >= lbounds.y + lbounds.h - 10) && (self->velocity.y > 0))
    {
        self->acceleration.y = 0;
        self->velocity.y = 0;
    }
    if (self->position.x > camera.x + camera.w + 32)return;// off the map, do no work
    if (self->position.x < camera.x - 128)
    {
        self->dead = 1;
        return;
    }
    switch(self->state)
    {
        case ES_Idle:
            self->body.layer = LAYER_MOBS|LAYER_PROJECTILES;
            self->state = ES_Seeking;
            break;
        case ES_Seeking:
        case ES_Charging:
        case ES_Attacking:
        case ES_Pain:
            if (self->actor.at == ART_END)
            {
                self->body.layer = LAYER_MOBS|LAYER_PROJECTILES;
                self->state = ES_Seeking;
                gf2d_actor_set_action(&self->actor,"idle");
            }
            break;
        case ES_Cooldown:
            space_bug_spawn_thrust(self,vector2d(-20,-14),20);
            break;
        case ES_Dying:
            pe_blood_spray(
                self->pe, 
                vector2d(self->position.x - camera.x - 20,self->position.y - camera.y),
                vector2d(gf2d_crandom(),gf2d_crandom()),
                gf2d_color8(50,200,50,200),
                1);
            if (self->actor.at == ART_END)
            {
                // finished dying
                self->state = ES_Dead;
            }
            return;
        case ES_Dead:
            r = gf2d_random();
            if (r < 0.1)
            {
                item_spawn("crysalis",self->position);
            }
            else if (r < 0.4)
            {
                item_spawn("small_crysalis",self->position);
            }
            self->dead = 1;
            return;
    }
}

int space_bug_touch(Entity *self,Entity *other)
{
    if ((!self)||(!other))return 0;
    if (!gf2d_entity_deal_damage(other, self, self,self->health,vector2d(0,0)))
    {
        return 0;
    }
    if (other != player_get())
    {
        return 0;
    }
    vector2d_clear(self->velocity);
    vector2d_clear(self->acceleration);
    self->die(self);
    return 1;
}

int space_bug_damage(Entity *self,int amount, Entity *source)
{
    int taken;
    taken = MIN(self->health,amount);
    self->health -= amount;
    slog("space_bug taking %i damage",amount);
    slog("space_bug health %f",self->health);
    if (self->health <= 0)
    {
        self->die(self);
        return taken;
    }
    self->state = ES_Pain;
    self->body.layer = 0;
    gf2d_actor_set_action(&self->actor,"pain1");
    return taken;
}

void space_bug_die(Entity *self)
{
    self->body.layer = 0;// no longer clip
    self->state = ES_Dying;
    vector2d_clear(self->velocity);
    vector2d_clear(self->acceleration);
    if (gf2d_crandom() > 0)
    {
        gf2d_actor_set_action(&self->actor,"death1");
    }else gf2d_actor_set_action(&self->actor,"death2");
    gf2d_sound_play(self->sound[0],0,1,-1,-1);
}

/*eol@eof*/
