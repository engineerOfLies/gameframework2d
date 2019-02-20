#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "gf2d_entity.h"
#include "simple_json.h"

Entity *monster_spawn(Vector2D position,SJson *args);

#endif
