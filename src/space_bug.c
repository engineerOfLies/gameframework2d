#include "simple_logger.h"
#include "space_bug.h"

void space_bug_think(Entity *self);


Entity *space_bug_new(Vector2D position)
{
    Entity *ent;
    ent = entity_new();
    if (!ent)return NULL;
    ent->sprite = gf2d_sprite_load_all(
        "images/space_bug_top.png",
        128,
        128,
        16,
        0);
    ent->think = space_bug_think;
    vector2d_copy(ent->position,position);
    ent->drawOffset = vector2d(64,74);
    ent->speed = 2.5;
    return ent;
}

void space_bug_think(Entity *self)
{
    Vector2D m,dir;
    int mx,my;
    if (!self)return;
    SDL_GetMouseState(&mx,&my);
    m.x = mx;
    m.y = my;
    vector2d_sub(dir,m,self->position);
    if (vector2d_magnitude_compare(dir,10)>0)
    {
        vector2d_set_magnitude(&dir,self->speed);
        vector2d_copy(self->velocity,dir);
    }
    else
    {
        vector2d_clear(self->velocity);
    }
}

/*eol@eof*/
