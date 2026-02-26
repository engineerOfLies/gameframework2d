#include "simple_logger.h"

#include "gfc_input.h"

#include "camera.h"
#include "player.h"

static Entity *thePlayer = NULL;

Entity *player_entity_get()
{
    return thePlayer;
}

void player_entity_think(Entity *self)
{
    GFC_Vector2D move = {0};
    if (!self)return;
    if (gfc_input_key_down("d"))
    {
        move.x += 1;
    }
    if (gfc_input_key_down("a"))
    {
        move.x -= 1;
    }
    if (gfc_input_key_down("s"))
    {
        move.y += 1;
    }
    if (gfc_input_key_down("w"))
    {
        move.y -= 1;
    }
    if ((move.x) || (move.y))
    {
        self->rotation = gfc_vector2d_angle(move) * GFC_RADTODEG;
        gfc_vector2d_normalize(&move);
        gfc_vector2d_scale(self->velocity,move,self->topSpeed);
    }
    self->frame += 0.1;
    if (self->frame >= 8)self->frame = 0;
}

void player_entity_update(Entity *self)
{
    if (!self)return;
    camera_center_on(self->position);
}

Entity *player_entity_new(GFC_Vector2D position)
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    self->sprite = gf2d_sprite_load_all(
        "images/ed210_top.png",
        128,
        128,
        16,
        0);
    self->bounds = gfc_rect(-32,-32,72,72);
    self->rotationCenter = gfc_vector2d(64,64);
    self->topSpeed = 3;
    self->position = position;
    self->think = player_entity_think;
    self->update = player_entity_update;
    thePlayer = self;
    return self;
}


/**/
