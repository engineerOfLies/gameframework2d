#ifndef __PROJECTILES_H__
#define __PROJECTILES_H__

#include "gf2d_entity.h"

Entity *projectile_new(Vector2D position,Vector2D velocity,Entity *parent);

#endif
