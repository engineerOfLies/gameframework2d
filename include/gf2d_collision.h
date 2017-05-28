#ifndef __GF2D_COLLISION_H__
#define __GF2D_COLLISION_H__

#include "gf2d_shape.h"
#include "gf2d_list.h"

typedef struct
{
    Vector2D pointOfContact;    /**<point in space that contact was made*/
    Vector2D normal;            /**<normal vector at the point of contact*/
    Shape   *other;             /**<shape information on what what contacted*/
    float    timeStep;          /**<at what time step contact was made*/
}Collision;

typedef struct Body_S
{
    int         inactive;       /**<internal use only*/
    Uint32      layer;          /**<only bodies that share one or more layers will interact*/
    Uint32      team;           /**<bodies that share a team will NOT interact*/
    Vector2D    position;       /**<position of the center of mass*/
    Vector2D    velocity;       /**<rate of change of position over time*/
    float       mass;           /**<used for inertia*/
    Shape      *shape;          /**<which shape data will be used to collide for this body*/
    void       *data;           /**<custom data pointer*/
    int     (*bodyTouch)(struct Body_S *self, struct Body_S *other, Collision *collision);/**< function to call when two bodies collide*/
    int     (*worldTouch)(struct Body_S *self, Collision *collision);/**<function to call when a body collides with a static shape*/
}Body;

typedef struct
{
    List       *bodyList;       /**<list of bodies in the space*/
    List       *staticShapes;   /**<list of shapes that will collide that do not move*/
    int         precision;      /**<number of backoff attempts before giving up*/
    Rect        bounds;         /**<absolute bounds of the space*/
    float       timeStep;       /**<how much each iteration of the simulation progresses time by*/
    Vector2D    gravity;        /**<global gravity pull direction*/
}Space;


#define ALL_LAYERS 0xffffffff

/**
 * @brief initializes a body to zero
 * @warning do not use this on a body in use
 */
void gf2d_body_clear(Body *body);

/**
 * @brief set all parameters for a body
 * @param body the body to set the parameters for
 * @param layer the layer mask
 * @param team the team
 * @param positition the position in space to be added at
 * @param velocity the velocity that the body is moving at
 * @param mass the mass of the body (for momentum purposes)
 * @param shape a pointer to the shape data to use for the body
 * @param data any custom data you want associated with the body
 * @param bodyTouch the callback to invoke when this body touches another body
 * @param worldTouch the callback to invoke when this body touches the world
 */
void gf2d_body_set(
    Body *body,
    Uint32      layer,
    Uint32      team,
    Vector2D    position,
    Vector2D    velocity,
    float       mass,
    Shape      *shape,
    void       *data,
    int     (*bodyTouch)(struct Body_S *self, struct Body_S *other, Collision *collision),
    int     (*worldTouch)(struct Body_S *self, Collision *collision));

/**
 * @brief create a new space
 * @return NULL on error or a new empty space
 */
Space *gf2d_space_new();

/**
 * @brief create a new space and set some defaults
 * @param precision number of retry attempts when movement collides
 * @param bounds the absolute bounds of the space
 * @param timeStep this should be fraction that can add up to 1.  ie: 0.1 or 0.01, etc
 * @param gravity the direction that gravity pulls
 */
Space *gf2d_space_new_full(
    int         precision,
    Rect        bounds,
    float       timeStep,
    Vector2D    gravity);

/**
 * @brief cleans up a space
 */
void gf2d_space_free(Space *space);

/**
 * @brief visualize the space and its contents
 */
void gf2d_space_draw(Space *space);

/**
 * @brief add a body to the space simulation
 * @param space the space to add a body to
 * @param body the body to add to the space
 * @note the space will not free the body, but do not until it has been removed from the space
 */
void gf2d_space_add_body(Space *space,Body *body);

#endif
