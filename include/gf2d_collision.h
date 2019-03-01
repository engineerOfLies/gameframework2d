#ifndef __GF2D_COLLISION_H__
#define __GF2D_COLLISION_H__

#include "gf2d_shape.h"
#include "gf2d_space.h"
#include "gf2d_list.h"
#include "gf2d_text.h"
#include "gf2d_body.h"

#define ALL_LAYERS 0xffffffff
#define WORLD_LAYER 1
#define PICKUP_LAYER 2
#define PLAYER_LAYER 4
#define MONSTER_LAYER 8
#define OBJECT_LAYER 16

typedef struct Collision_S
{
    Uint8    collided;          /**<true if the there as a collision*/
    Uint8    blocked;           /**<true if this blocked any further movement.  Default for non elastic collisions*/
    Vector2D pointOfContact;    /**<point in space that contact was made*/
    Vector2D normal;            /**<normal vector at the point of contact*/
    Shape   *shape;             /**<shape information on what what contacted*/
    Body    *body;              /**<body information if a body was collided with*/
    Uint8    bounds;            /**<true if this collision was with the space bounds*/
    float    timeStep;          /**<at what time step contact was made*/
}Collision;

typedef struct CollisionFilter_S
{
    Uint8       worldclip;      /**<if this body should clip the world bounds and static shapes*/
    Uint32      cliplayer;      /**<only bodies that share one or more layers will collide with each other*/
    Uint32      touchlayer;     /**<only bodies that share one or more layers will have their touch functions called*/
    Uint32      team;           /**<bodies that share a team will NOT interact*/
    Body       *ignore;         /**<this body will specifically be skipped in checks*/
}CollisionFilter;

/**
 * @brief allocate data for a new collision
 * @return a new initialized collision or NULL on error
 */
Collision *gf2d_collision_new();

/**
 * @brief free data allocated for a collision
 * @param collision the collision to free
 */
void gf2d_collision_free(Collision *collision);

/**
 * @brief free all the collisions and the list containing it.
 * @param list must contain a list of collisions
 */
void gf2d_collision_list_free(List *list);

/**
 * @brief check if the provided shape intersects anything in the space
 * @param space the space to test
 * @param shape the shape to check with
 * @param filter the filter to use for testing
 * @return a List of collisions data for the test
 */
List *gf2d_collision_check_space_shape(Space *space, Shape shape,CollisionFilter filter);

/**
 * @brief perform a linear trace through the space
 * @param space the space to test
 * @param start the starting position of the trace
 * @param end the end point of the trace
 * @param filter the filter to apply to the test
 * @return a collision structure.  Note the timeStep will be the percentage of the trace that was completed before a collision was triggered
 */
Collision gf2d_collision_trace_space(Space *space, Vector2D start, Vector2D end ,CollisionFilter filter);

#endif
