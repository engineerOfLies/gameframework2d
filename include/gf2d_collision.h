#ifndef __GF2D_COLLISION_H__
#define __GF2D_COLLISION_H__

#include "gf2d_shape.h"

typedef struct Body_S
{
    int         active;
    Vector2D    position;
    float       mass;
    Shape      *shape;
    void       *data;
    int     (*touch)(struct Body_S *self, struct Body_S *other);
}Body;



#endif
