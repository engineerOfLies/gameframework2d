#include "simple_logger.h"
#include "camera.h"
#include "level.h"
#include "particle_effects.h"

void projectile_draw(Entity *self);
void projectile_think(Entity *self);
void projectile_update(Entity *self);
int  projectile_touch(Entity *self,Entity *other);
void projectile_damage(Entity *self,int amount, Entity *source);
void projectile_die(Entity *self);

Entity *projectile_new(Vector2D position,Vector2D velocity,float damage,float scale,Entity *parent,char *actor)
{
    Entity *self;
    self = gf2d_entity_new();
    if (!self)return NULL;

    if (parent)
    {
        snprintf(self->name,GF2DLINELEN,"%s's projectile",parent->name);
    }
    else
    {
        gf2d_line_cpy(self->name,"projectile");
    }
    self->parent = parent;
    
    
    self->shape = gf2d_shape_circle(0,0, 30);
    gf2d_body_set(
        &self->body,
        "projectile",
        LAYER_PROJECTILES,
        parent->body.team,
        position,
        velocity,
        10,
        0,
        0,
        &self->shape,
        self,
        NULL,
        NULL);

    gf2d_actor_load(&self->actor,actor);
    gf2d_actor_set_action(&self->actor,"idle");
    
    vector2d_copy(self->position,position);
    vector2d_copy(self->velocity,velocity);
    
    vector2d_set(self->scale,scale,scale);
    vector2d_set(self->scaleCenter,32,32);
    vector3d_set(self->rotation,32,32,0);
    
    self->pe = gf2d_particle_emitter_new(250);
    
    self->think = projectile_think;
    self->draw = projectile_draw;
    self->update = projectile_update;
    self->touch = projectile_touch;
    self->damage = projectile_damage;
    self->die = projectile_die;
    self->free = level_remove_entity;
    
    self->health = damage;
    level_add_entity(self);
    return self;
}

void projectile_draw(Entity *self)
{
    
}

void projectile_think(Entity *self)
{
//derp
}

void projectile_update(Entity *self)
{
    Rect camera;
    Rect bounds,lbounds;
    if (!self)return;
    camera = camera_get_dimensions();

    bounds = gf2d_shape_get_bounds(self->shape);
    vector2d_add(bounds,bounds,self->position);
    lbounds = level_get_bounds();
    if ((bounds.y <= lbounds.y + 1) && (self->velocity.y < 0))
    {
        self->acceleration.y = 0;
        self->velocity.y = 0;
        self->dead = 1;
        return;// off the map, do no work
    }
    if ((bounds.y + bounds.h >= lbounds.y + lbounds.h - 1) && (self->velocity.y > 0))
    {
        self->acceleration.y = 0;
        self->velocity.y = 0;
        self->dead = 1;
        return;// off the map, die
    }
    if (self->position.x > camera.x + camera.w + 32)
    {
        self->dead = 1;
        return;// off the map, die
    }
    if (self->position.x < camera.x - 128)
    {
        self->dead = 1;
        return;
    }
    pe_thrust(
        self->pe,
        vector2d(self->position.x - camera.x,self->position.y - camera.y),
        vector2d(5,5),
        vector2d(0,0),
        vector2d(0,0),
        vector2d(0,0),
        vector2d(0,0),
        gf2d_color_from_vector4(self->actor.al->colorSpecial),
        10);
}

int projectile_touch(Entity *self,Entity *other)
{
    if ((!self)||(!other))return 0;
    if (!gf2d_entity_deal_damage(other, self, self,self->health,self->velocity))
    {
        return 0;
    }
    vector2d_clear(self->velocity);
    vector2d_clear(self->acceleration);
    self->die(self);
    return 1;
}

void projectile_damage(Entity *self,int amount, Entity *source)
{
    return;
}

void projectile_die(Entity *self)
{
    self->body.layer = 0;// no longer clip
    self->dead = 1;
}

/*eol@eof*/
