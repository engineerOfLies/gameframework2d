#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

typedef struct Player_S
{
    Uint8 exsits;
    Entity *player;
    const char *playerName;
}Player;

void player_think(Entity *self);
void player_update(Entity *self);
void player_free(Player *self);

Player *player_new(const char *thePlayerName);



#endif