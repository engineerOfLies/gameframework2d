#ifndef __GF2D_COLLISION_H__
#define __GF2D_COLLISION_H__

#include "gf2d_shape.h"
#include "gf2d_list.h"
#include "gf2d_text.h"

#define ALL_LAYERS 0xffffffff
#define WORLD_LAYER 1
#define PICKUP_LAYER 2
#define PLAYER_LAYER 4
#define MONSTER_LAYER 8

typedef struct Collision_S Collision;

typedef struct Body_S
{
    TextLine    name;           /**<name for debugging purposes*/
    int         inactive;       /**<internal use only*/
    float       gravity;        /**<the factor this body adheres to gravity*/
    Uint32      layer;          /**<only bodies that share one or more layers will interact*/
    Uint32      team;           /**<bodies that share a team will NOT interact*/
    Vector2D    position;       /**<position of the center of mass*/
    Vector2D    velocity;       /**<rate of change of position over time*/
    Vector2D    newvelocity;    /**<after a collision this is the new calculated velocity*/
    float       mass;           /**<used for inertia*/
    float       elasticity;     /**<how much bounce this body has*/
    Shape      *shape;          /**<which shape data will be used to collide for this body*/
    void       *data;           /**<custom data pointer*/
    int       (*bodyTouch)(struct Body_S *self, struct Body_S *other, Collision *collision);/**< function to call when two bodies collide*/
    int       (*worldTouch)(struct Body_S *self, Collision *collision);/**<function to call when a body collides with a static shape*/
}Body;

typedef struct
{
    Uint32      layer;          /**<layer mask to clip against*/
    Uint32      team;           /**<ignore any team ==*/
    Body       *ignore;         /**<if not null, the body will be ignored*/
}ClipFilter;

struct Collision_S
{
    Uint8    collided;          /**<true if the there as a collision*/
    Vector2D pointOfContact;    /**<point in space that contact was made*/
    Vector2D normal;            /**<normal vector at the point of contact*/
    Shape   *shape;             /**<shape information on what what contacted*/
    Body    *body;              /**<body information if a body was collided with*/
    float    timeStep;          /**<at what time step contact was made*/
};

typedef struct
{
    List       *bodyList;       /**<list of bodies in the space*/
    List       *staticShapes;   /**<list of shapes that will collide that do not move*/
    int         precision;      /**<number of backoff attempts before giving up*/
    Rect        bounds;         /**<absolute bounds of the space*/
    float       timeStep;       /**<how much each iteration of the simulation progresses time by*/
    Vector2D    gravity;        /**<global gravity pull direction*/
    float       dampening;      /**<rate of movement degrade  ambient frictions*/
    float       slop;           /**<how much to correct for body overlap*/
}Space;


/**
 * @brief initializes a body to zero
 * @warning do not use this on a body in use
 */
void gf2d_body_clear(Body *body);

/**
 * @brief set all parameters for a body
 * @param body the body to set the parameters for
 * @param name the name of the body
 * @param layer the layer mask
 * @param team the team
 * @param positition the position in space to be added at
 * @param velocity the velocity that the body is moving at
 * @param mass the mass of the body (for momentum purposes)
 * @param gravity the factor this body adheres to gravity
 * @param elasticity how much bounce this body has
 * @param shape a pointer to the shape data to use for the body
 * @param data any custom data you want associated with the body
 * @param bodyTouch the callback to invoke when this body touches another body
 * @param worldTouch the callback to invoke when this body touches the world
 */
void gf2d_body_set(
    Body *body,
    char       *name,
    Uint32      layer,
    Uint32      team,
    Vector2D    position,
    Vector2D    velocity,
    float       mass,
    float       gravity,
    float       elasticity,
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
 * @param dampening the rate of all movemement decay
 * @param slop how much to correct for body overlap
 */
Space *gf2d_space_new_full(
    int         precision,
    Rect        bounds,
    float       timeStep,
    Vector2D    gravity,
    float       dampening,
    float       slop);

/**
 * @brief cleans up a space
 */
void gf2d_space_free(Space *space);

/**
 * @brief visualize the space and its contents
 * @param space the space to draw
 * @param offset a positional offset applied when drawing.
 */
void gf2d_space_draw(Space *space,Vector2D offset);

/**
 * @brief add a body to the space simulation
 * @param space the space to add a body to
 * @param body the body to add to the space
 * @note the space will not free the body, but do not until it has been removed from the space
 */
void gf2d_space_add_body(Space *space,Body *body);

/**
 * @brief removes a body from the space
 * @note this should not be done DURING a space update
 * @param space the space to remove the body from
 * @param body the body to remove
 */
void gf2d_space_remove_body(Space *space,Body *body);

/**
 * @brief apply a force to a body taking into account momentum
 * @param body the body to move
 * @param direction a unit vector for direction (Does not have to be)
 * @param force the amount of force to apply
 */
void gf2d_body_push(Body *body,Vector2D direction,float force);

/**
 * @brief add a statuc shape to the space
 * @note the shape parameters need to be in absolute space, not relative to any body
 * @param space the space to add to
 * @param shape the shape to add.
 */
void gf2d_space_add_static_shape(Space *space,Shape shape);

/**
 * @brief update the bodies in the physics space for one time slice
 * @param space the space to be updated
 */
void gf2d_space_update(Space *space);

/**
 * @brief check if a shape intersects with any static shape within the space
 * @param space the space to test
 * @param shape the shape to test with
 * @return the collision information
 */
Collision gf2d_space_shape_test(Space *space,Shape shape);

/**
 * @brief check if a shape intersects with any body within the space
 * @param space the space to test
 * @param shape the shape to test with
 * @param filter a filter to limit results
 * @return the collision information
 */
void gf2d_space_body_collision_test_filter(Space *space,Shape shape, Collision *collision,ClipFilter filter);

/**
 * @brief get the shape, adjusted for position for the provided body
 * @param a the body to get the shape for
 * @return an empty {0} shape on error, or the body shape information otherwise
 */
Shape gf2d_body_to_shape(Body *a);

#endif
