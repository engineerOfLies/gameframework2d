#include "simple_logger.h"

#include "player.h"

#include "monster.h"

typedef enum
{
    MS_Idle,
    MS_hunt,
    MS_Attack,
    MS_Pain,
    MS_Die,
    MS_MAX
}MonsterStates;

typedef struct
{
    Entity         *player;
    MonsterStates   state;
}MonsterData;

void monster_free(Entity *self)
{
    MonsterData *data;
    if ((!self)||(!self->data))return;
    data = (MonsterData*)self->data;
    //clean up anything I own that I asked for
    free(data);
}

void monster_think(Entity *self)
{
    GFC_Vector2D toPlayer = {0};
    MonsterData *data;
    if ((!self)||(!self->data))return;
    data = (MonsterData*)self->data;
    if (!data->player)return;
    gfc_vector2d_sub(toPlayer,data->player->position,self->position);
    gfc_vector2d_normalize(&toPlayer);
    self->rotation = gfc_vector2d_angle(toPlayer) * GFC_RADTODEG + 180;
    self->frame += 0.01;
    if (self->frame >= 8)self->frame = 0;
}

void monster_update(Entity *self)
{
    if (!self)return;

}

Uint8 monster_touch(Entity *self,Entity *other)
{
    MonsterData *data;
    if ((!self)||(!self->data))return 0;
    data = (MonsterData*)self->data;
    if (other == data->player)
    {
        entity_free(self);
        return 1;
    }
    return 0;
}

Entity *monster_new(GFC_Vector2D position)
{
    Entity *self;
    MonsterData*data;
    self = entity_new();
    if (!self)return NULL;
    self->sprite = gf2d_sprite_load_all(
        "images/space_bug_top_grey.png",
        128,
        128,
        16,
        0);
    data = gfc_allocate_array(sizeof(MonsterData),1);
    if (data)
    {
        data->player = player_entity_get();
    }
    self->data = data;
    self->rotationCenter = gfc_vector2d(64,64);
    self->topSpeed = 3;
    self->color = gfc_color(gfc_random(),gfc_random(),gfc_random(),1);
    self->bounds = gfc_rect(-32,-32,72,72);
    self->position = position;
    self->touch = monster_touch;
    self->think = monster_think;
    self->update = monster_update;
    self->free = monster_free;
    return self;
}


/**/
