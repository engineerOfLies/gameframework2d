#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

typedef struct Player_S
{
    Uint8 exsits;
    Entity *player;
    const char *playerName;
    float playerSpeed;
}Player;

void player_think(Player *self);
void player_update(Player *self);
void player_free(Player *self);

Player *player_new(const char *thePlayerName);



#endif