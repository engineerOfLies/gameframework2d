#include "player.h"
#include "simple_logger.h"

static Player thePlayer = {0};

Player *player_new(const char *thePlayerName)
{
    //slog("exsists?: %i", thePlayer.exsits);
    if(thePlayer.exsits)
    {
        slog("Player already exsists");
        return NULL;
    }
    

    thePlayer.exsits = 1;
    //slog("exsists?: %i", thePlayer.exsits);

    Entity *player;
    player = entity_new();
    //slog("player.c line 18");

    if(!player)
    {
        slog("Failed to create the player");
        return NULL;
    }

    player->sprite = gf2d_sprite_load_all("images/skelebones.png",64,64,9,0);
    player->frame = 0;
    player->position = vector2d(0,0);
    player->isPlayer = 1;
    player->entityName = thePlayerName;

    slog("The Players Chosen Name: %s", thePlayerName);

    thePlayer.player = player;
    thePlayer.playerName = thePlayerName;
    thePlayer.playerSpeed = 2.5;

    return &thePlayer;
}

void player_think(Player *self)
{
    if(!self)return;
    
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL);
    Vector2D up, down, left, right;
    up = vector2d(0,-self->playerSpeed);
    down = vector2d(0,self->playerSpeed);
    left = vector2d(-self->playerSpeed,0);
    right = vector2d(self->playerSpeed,0);

    if (keys[SDL_SCANCODE_W])
    {
        //vector3d_add(self->position,self->position,forward);
        vector2d_add(self->player->position,self->player->position,up);
    }
    if (keys[SDL_SCANCODE_S])
    {
        //vector3d_add(self->position,self->position,-forward);
        vector2d_add(self->player->position,self->player->position,down);
    }
    if (keys[SDL_SCANCODE_D])
    {
        //vector3d_add(self->position,self->position,right);
        vector2d_add(self->player->position,self->player->position,right);
    }
    if (keys[SDL_SCANCODE_A])
    {
        //vector3d_add(self->position,self->position,-right);
        vector2d_add(self->player->position,self->player->position,left);
    }
    if(keys[SDL_SCANCODE_N])
    {
        Entity *temp_ent;
        temp_ent = entity_new();

        temp_ent->sprite = gf2d_sprite_load_all("images/ed210.png",128,128,16,0);
        temp_ent->frame = 0;
        temp_ent->position = self->player->position;
        temp_ent->isEnemy = 1;
    }
    if(keys[SDL_SCANCODE_C])
    {
        entity_clear_all(self->player);
    }
}
void player_update(Player *self)
{
    if(!self)return;
    self->player->frame += 0.1;
    //slog("Frame: %f", self->player->frame);
    if(self->player->frame >= 9)self->player->frame = 0;
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