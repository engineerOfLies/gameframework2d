#ifndef __WALKMASK_H__
#define __WALKMASK_H__

#include "gfc_list.h"
#include "gfc_vector.h"

typedef struct
{
    Vector2D position;      /**<relative position of this point in the background*/
    Vector2D scale;         /**<drawscale for the player near this point*/
    Uint8    layer;         /**<layer that a player near this point should be considered part of*/
    Uint32   nextPoint;     /**<next point in the list*/
}PointData;

typedef struct
{
    List    *points;        /**<sequence of points that will make a walk mask by its outline*/
    Uint8    exterior;      /**<if this polygon should be considered an interior or exterior mask.  IE: interior masks require the player to be within it, while exterior ones require the player to be outside of it*/
}Walkmask;

/**
 * @brief make a new walk mask
 * @return a blank walkmask or NULL or error
 */
Walkmask *walkmask_new();

/**
 * @brief make a walk mask by the rectangle provided
 * @param r the rectangle to feed the walkmask within
 */
Walkmask *walkmask_new_by_rect(Rect r);

/**
 * @brief clean up a walkmask
 */
void walkmask_free(Walkmask *walkmask);

/**
 * @brief check if a point is within a walkmask or not
 */
int walkmask_point_in_check(Walkmask *mask, Vector2D queryPoint);

/**
 * @brief insert a walkmask point into the walk mask
 * @note if its not the first, you NEED to make sure that previous is specified or it won't work
 * @param mask the mask to add a point to
 * @param position the position in the background to add the point to
 * @param previous if this is the first point added, it can be null, otherwise it needs to be a point already in the mask
 * @return NULL on error or the newly added point otherwise
 */
PointData *walkmask_insert_point(Walkmask *mask,Vector2D position,PointData *previous);

/**
 * @brief insert a walkmask point between the reference point and the next point
 * @param mask the mask to add a point to
 * @param previous the reference point.  New point will be between this point and the next point
 */
PointData *walkmask_subdivide_point(Walkmask *mask,PointData *previous);

/**
 * @brief draws the walkmask to the screen
 * @param walkmask the mask to draw
 * @param color the color to draw the mask as
 * @param offset an offset to draw the mask at
 */
void walkmask_draw(Walkmask *walkmask,Color color, Vector2D offset);

/**
 * @brief move a whole mask by the offset provided
 * @param mask the mask to move
 * @param offset the vector to move the mask by
 */
void walkmask_move(Walkmask *mask,Vector2D offset);

/**
 * @brief given the refPoint, find the nearest point in the walk mask
 * @param mask the mask to search
 * @param refPoint the point to search by
 * @return NULL on error or the closest PointData otherwise
 */
PointData *walkmask_get_nearest_point(Walkmask *mask, Vector2D refPoint);

/**
 * @brief get the rectangular bounds described by the walkmask
 * @param mask the mask to query
 * @return an empty rect on error or missing information, a set rectangle otherwise
 */
Rect walkmask_get_bounds(Walkmask *mask);

/**
 * @brief save a walkmask to json for writing to disk
 * @param mask the mask to convert
 * @return NULL on error or a json struct otherwise
 */
SJson *walkmask_to_json(Walkmask *mask);

/**
 * @brief loat a walkmask from json data
 * @param json the json to parse
 * @return NULL on error or a configured walkmask otherwise
 */
Walkmask *walkmask_load_from_json(SJson *json);

/**
 * @brief find out if the line traced from start to end clips any edge in the given mask.
 * @param mask the walkmask to check against
 * @param start the start point of the trace
 * @param end the end point of the trace
 * @param contact (optional) if there is a collision, this will be populated with the contact point of the collision
 * @return 0 if there is no clip, 1 if there is.  If you provided a contact point it will be set on true only.
 */
int walkmask_edge_clip(Walkmask *mask,Vector2D start, Vector2D end,Vector2D *contact);

#endif
