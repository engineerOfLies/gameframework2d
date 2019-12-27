#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <SDL.h>

#include "gfc_vector.h"

#include "gf2d_shape.h"

/**
 * @brief set the position and size of the screen camera
 * @param x the current left position of the camera
 * @param y the current top position of the camera
 * @param w the width of the camera
 * @param h the height of the camera
 */
void camera_set_dimensions(Sint32 x,Sint32 y,Uint32 w,Uint32 h);

/**
 * @brief get the position and size of the camera
 * @return the camera view
 */
Rect camera_get_dimensions();

/**
 * @brief get the current camera position
 * @return the x,y position of the camera
 */

Vector2D camera_get_position();

/**
 * @brief limit the position of the camera by these bounds
 * @param x the minimum x position for the camera
 * @param y the minimum y position for the camera
 * @param w the right bounds
 * @param w the bottom bounds
 */
void camera_set_bounds(Sint32 x,Sint32 y,Uint32 w,Uint32 h);

/**
 * @brief get the current camera bounds
 * @return the camera bounding rect
 */
Rect camera_get_bounds();

/**
 * @brief move the camera, but limit by bounds
 * @param v the vector to move the camera by
 */
void camera_move(Vector2D v);

/**
 * @brief move the camera to new position, but respect bounds
 * @param position the ideal new top left position of the camera
 */
void camera_set_position(Vector2D position);

/**
 * @brief move the camera to ne position, but do not respect bounds
 * @param position the new top left position of the camera
 */
void camera_set_position_absolute(Vector2D position);
#endif
