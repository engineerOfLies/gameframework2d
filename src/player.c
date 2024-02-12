#include "player.h"
#include "simple_logger.h"

static Player thePlayer = {0};

Player *player_new(const char *thePlayerName)
{
    slog("exsists?: %i", thePlayer.exsits);
    if(thePlayer.exsits)
    {
        slog("Player already exsists");
        return NULL;
    }
    

    thePlayer.exsits = 1;
    slog("exsists?: %i", thePlayer.exsits);

    Entity *player;
    player = entity_new();
    slog("player.c line 18");

    if(!player)
    {
        slog("Failed to create the player");
        return NULL;
    }

    player->sprite = gf2d_sprite_load_all("images/ed210.png",128,128,16,0);
    player->frame = 0;
    player->position = vector2d(0,0);
    player->isPlayer = 1;
    player->entityName = thePlayerName;
    player->think = player_think;
    player->update = player_update;

    slog("The Players Chosen Name: %s", thePlayerName);

    thePlayer.player = player;
    thePlayer.playerName = thePlayerName;

    return &thePlayer;
}

void player_think(Entity *self)
{
    if(!self)return;
}
void player_update(Entity *self)
{
    if(!self)return;
}
void player_free(Player *self)
{
    if(!self)return;
    entity_free(self->player);
    self->playerName = NULL;
    thePlayer.exsits = 0;
    slog("exsists?: %i | Player Freed", thePlayer.exsits);
}


/*eol@eof*/