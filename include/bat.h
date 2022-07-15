#ifndef __BAT_H__
#define __BAT_H__

#include "simple_json.h"

#include "gf2d_entity.h"

Entity *bat_spawn(Vector2D position,Uint32 id, SJson *args,const char *entityDef);

#endif
