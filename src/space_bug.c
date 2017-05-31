#include "space_bug.h"
#include "simple_logger.h"
#include "camera.h"

void space_bug_draw(Entity *self);
void space_bug_think(Entity *self);
void space_bug_update(Entity *self);
void space_bug_touch(Entity *self,Entity *other);
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
        ALL_LAYERS,
        1,
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

    self->frame = 0;
    self->al = gf2d_action_list_load("actors/space_bug.actor");
    gf2d_line_cpy(self->action,"idle");
    
    vector2d_copy(self->position,position);
    
    vector2d_set(self->scale,1,1);
    vector2d_set(self->scaleCenter,64,64);
    vector3d_set(self->rotation,64,64,0);
    self->color = gf2d_color8(255,255,255,255);
    
    self->pe = gf2d_particle_emitter_new(500);
    
    self->think = space_bug_think;
    self->draw = space_bug_draw;
    self->update = space_bug_update;
    self->touch = space_bug_touch;
    self->damage = space_bug_damage;
    self->die = space_bug_die;

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
                self->position.x - camPosition.x + gf2d_crandom()*2 + offset.x,
                self->position.y - camPosition.y + gf2d_crandom()*2 + offset.y),
            vector2d(
                -self->velocity.x*0.5 + gf2d_crandom()*2.5 + 2,
                (-self->velocity.y*0.5 + gf2d_crandom()*2.5)*0.2),
            vector2d(-self->acceleration.x*0.5 + 0.1,-self->acceleration.y*0.05),
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

void space_bug_update(Entity *self)
{
    Rect bounds,lbounds;
    if (!self)return;
    Rect level_get_bounds();

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
    // make thrust particles

    space_bug_spawn_thrust(self,vector2d(40,8),50);
}

void space_bug_touch(Entity *self,Entity *other)
{
    
}

void space_bug_damage(Entity *self,int amount, Entity *source)
{
    
}

void space_bug_die(Entity *self)
{
    
}

/*eol@eof*/
