#include "gf2d_collision.h"
#include "simple_logger.h"
#include "gf2d_draw.h"
#include <stdlib.h>

Collision *gf2d_collision_new()
{
    Collision *collision = NULL;
    collision = (Collision *)malloc(sizeof(Collision));
    if (!collision)
    {
        slog("failed to allocate data for a collision object");
        return NULL;
    }
    memset(collision,0,sizeof(Collision));
    return collision;
}

void gf2d_collision_free(Collision *collision)
{
    if (!collision)return;
    free(collision);
}


/*eol@eof*/
