#ifndef __SPACE_H__
#define __SPACE_H__

#include "gf2d_shape.h"
#include "gf2d_body.h"

typedef struct
{
    List       *dynamicBodyList;       /**<list of bodies in the space*/
    List       *staticShapes;   /**<list of shapes that will collide that do not move*/
    int         precision;      /**<number of backoff attempts before giving up*/
    Rect        bounds;         /**<absolute bounds of the space*/
    float       timeStep;       /**<how much each iteration of the simulation progresses time by*/
    Vector2D    gravity;        /**<global gravity pull direction*/
    float       dampening;      /**<rate of movement degrade  ambient frictions*/
    float       slop;           /**<how much to correct for body overlap*/
    Uint32      idpool;
}Space;

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
 * @brief attempt to fix any bodies that are overlapping static shapes or clipping the space bounds
 * @param space the space to update
 * @param tries the number of tries to get everything clear before giving up
 */
void gf2d_space_fix_overlaps(Space *space,Uint8 tries);


#endif
