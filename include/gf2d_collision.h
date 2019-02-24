#ifndef __GF2D_COLLISION_H__
#define __GF2D_COLLISION_H__

#include "gf2d_shape.h"
#include "gf2d_list.h"
#include "gf2d_text.h"
#include "gf2d_body.h"

#define ALL_LAYERS 0xffffffff
#define WORLD_LAYER 1
#define PICKUP_LAYER 2
#define PLAYER_LAYER 4
#define MONSTER_LAYER 8
#define OBJECT_LAYER 16

struct Collision_S
{
    Uint8    collided;          /**<true if the there as a collision*/
    Vector2D pointOfContact;    /**<point in space that contact was made*/
    Vector2D normal;            /**<normal vector at the point of contact*/
    Shape   *shape;             /**<shape information on what what contacted*/
    Body    *body;              /**<body information if a body was collided with*/
    float    timeStep;          /**<at what time step contact was made*/
};

#endif
