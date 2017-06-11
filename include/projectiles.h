#ifndef __PROJECTILES_H__
#define __PROJECTILES_H__

#include "gf2d_entity.h"

/**
 * @brief spawn a project
 * @param position where to spawn it
 * @param velocity direction and speed it is moving in
 * @param damage how much damage it will deal
 * @param scale scaling factor for sprite
 * @param parent pointer to the one that shot it
 * @param actor actor file to use for the sprite
 */
Entity *projectile_new(Vector2D position,Vector2D velocity,float damage,float scale,Entity *parent,char *actor);

#endif
