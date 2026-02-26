#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

Entity *player_entity_new(GFC_Vector2D position);

/**
 * @brief get a pointer to the player, if its exists
 */
Entity *player_entity_get();

#endif
