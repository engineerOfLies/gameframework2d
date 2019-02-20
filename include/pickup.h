#ifndef __PICKUP_H__
#define __PICKUP_H__

#include "gf2d_entity.h"
#include "simple_json.h"

Entity *pickup_spawn(Vector2D position,SJson *args);

#endif
