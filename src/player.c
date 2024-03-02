#include "simple_logger.h"

#include "inventory.h"
#include "camera.h"
#include "player.h"

typedef struct
{
    int level;
    int experience;
    TextLine classname;
    Inventory inventory;
}PlayerData;


void player_think(Entity *self);
void player_update(Entity *self);
void player_free(Entity *self);

Entity *player_new()
{
    Entity *self;
    PlayerData *data;
    
    self = entity_new();
    if (!self)
    {
        slog("failed to spawn a player entity");
        return NULL;
    }
    self->sprite = gf2d_sprite_load_all(
        "images/ed210.png",
        128,
        128,
        16,
        0);
    self->frame = 0;
    self->position = vector2d(0,0);

    self->think = player_think;
    self->update = player_update;
    self->free = player_free;
    
    data = gfc_allocate_array(sizeof(PlayerData),1);
    if (data)
    {
        self->data = data;
        inventory_init(&data->inventory);
    }
    return self;
}

void player_think(Entity *self)
{
    Vector2D screen;
    Vector2D dir = {0};
    Sint32 mx = 0,my = 0;
    if (!self)return;
    screen = camera_get_position();
    SDL_GetMouseState(&mx,&my);
    mx += screen.x;
    my += screen.y;
    if (self->position.x < mx)dir.x = 1;
    if (self->position.y < my)dir.y = 1;
    if (self->position.x > mx)dir.x = -1;
    if (self->position.y > my)dir.y = -1;
    vector2d_normalize(&dir);
    vector2d_scale(self->velocity,dir,3);
}

void player_update(Entity *self)
{
    if (!self)return;
    self->frame += 0.1;
    if (self->frame >= 16)self->frame = 0;
    vector2d_add(self->position,self->position,self->velocity);
    
    camera_center_on(self->position);
}

void player_free(Entity *self)
{
    PlayerData *data;
    if ((!self)||(!self->data))return;
    data = (PlayerData *)self->data;
    inventory_cleanup(&data->inventory);
    free(data);
}

/*eol@eof*/
