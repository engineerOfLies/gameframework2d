#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

typedef struct Player_S
{
    Entity player;
    const char *playerName;
}Player;

void player_think(Player *self);
void player_update(Player *self);
void player_free(Player *self);

Player *player_new();



#endif