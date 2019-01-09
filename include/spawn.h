#ifndef __SPAWN_H__
#define __SPAWN_H__

#include "gf2d_entity.h"
#include "simple_json.h"

typedef struct
{
    const char *name;
    void (*spawn)(Vector2D,SJson *args);
}Spawn;

void spawn_entity(const char *name,Vector2D position,SJson *args);

#endif
