#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "gfc_vector.h"
#include "gfc_shape.h"

/**
 * @brief get the camera's position in world space
 * @return the position
 */
Vector2D camera_get_position();

/**
 * @brief get the camera's size
 * @return the size
 */
void camera_set_size(Vector2D size);


/**
 * @brief get the offset to draw things relative to the camera
 * @return the offset
 */
Vector2D camera_get_offset();

/**
 * @brief set the camera's position in world space
 */
void camera_set_postition(Vector2D position);

/**
 * @brief snap the camera into world bounds
 */
void camera_apply_bounds();

void camera_set_bounds(Rect bounds);

void camera_enable_binding(Bool bindCamera);

void camera_center_on(Vector2D target);

#endif
