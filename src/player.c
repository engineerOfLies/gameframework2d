#include "player.h"
#include "simple_logger.h"



Player *player_new()
{
    Entity *player;
    player = entity_new();

    if(!player)
    {
        slog("Failed to create the player");
        return NULL;
    }

    player->sprite = gf2d_sprite_load_all("images/skelebones.png",64,64,9,0);
    player->frame = 0;
    player->position = vector2d(0,0);
    player->entityName = "Greg";
    player->think = player_think;
    player->update = player_update;
    player->free = player_free;


    return player;
}


/*eol@eof*/