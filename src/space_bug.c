#include "space_bug.h"
#include "simple_logger.h"
#include "camera.h"
#include "level.h"
#include "particle_effects.h"
#include "items.h"

void space_bug_draw(Entity *self);
void space_bug_think(Entity *self);
void space_bug_update(Entity *self);
int  space_bug_touch(Entity *self,Entity *other);
void space_bug_damage(Entity *self,int amount, Entity *source);
void space_bug_die(Entity *self);

Entity *space_bug_new(Vector2D position)
{
    Entity *self;
    self = gf2d_entity_new();
    if (!self)return NULL;
    
    gf2d_line_cpy(self->name,"space_bug");
    self->parent = NULL;
    
    
    self->shape = gf2d_shape_rect(-32, -16, 64, 48);
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

    self->sprite = gf2d_sprite_load_all("images/space_bug.png",128,128,16);

    self->frame = (gf2d_random()*8);
    self->al = gf2d_action_list_load("actors/space_bug.actor");
    gf2d_line_cpy(self->action,"idle");
    
    vector2d_copy(self->position,position);
    
    vector2d_set(self->scale,1,1);
    vector2d_set(self->scaleCenter,64,64);
    vector3d_set(self->rotation,64,64,0);
    self->color = gf2d_color8(255,255,255,255);
    
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

    return self;
}

void space_bug_draw(Entity *self)
{
    
}
void space_bug_think(Entity *self)
{

    
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
        self->state = ES_Dead;
        return;
    }
    switch(self->state)
    {
        case ES_Idle:
            self->body.layer = LAYER_MOBS|LAYER_PROJECTILES;
            self->state = ES_Seeking;
            break;
        case ES_Seeking:
        case ES_Attacking:
        case ES_Pain:
        case ES_Cooldown:
            space_bug_spawn_thrust(self,vector2d(-20,-14),20);
            break;
        case ES_Dying:
            pe_blood_spray(
                self->pe, 
                vector2d(self->position.x - camera.x,self->position.y - camera.y),
                vector2d(gf2d_crandom(),gf2d_crandom()),
                gf2d_color8(50,200,50,200),
                50);
            if (self->at == ART_END)
            {
                // finished dying
                item_spawn("small_crysalis",self->position);
                self->state = ES_Dead;
            }
            return;
        case ES_Dead:
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
    vector2d_clear(self->velocity);
    vector2d_clear(self->acceleration);
    self->die(self);
    return 1;
}

void space_bug_damage(Entity *self,int amount, Entity *source)
{
    self->health -= amount;
    if (self->health <= 0)self->die(self);
}

void space_bug_die(Entity *self)
{
    self->body.layer = 0;// no longer clip
    self->state = ES_Dying;
    if (gf2d_crandom() > 0)
    {
        gf2d_line_cpy(self->action,"death1");
    }else gf2d_line_cpy(self->action,"death1");
    self->frame = gf2d_action_set(self->al,self->action);
}

/*eol@eof*/
