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
    
    return ent;
}

void space_bug_think(Entity *self)
{
    int mx,my;
    if (!self)return;
    SDL_GetMouseState(&mx,&my);
    if (mx < self->position.x)self->velocity.x = -0.1;
    if (mx > self->position.x)self->velocity.x = 0.1;
    if (my < self->position.y)self->velocity.y = -0.1;
    if (my > self->position.y)self->velocity.y = 0.1;
}

/*eol@eof*/
