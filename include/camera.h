#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "gfc_types.h"
#include "gfc_vector.h"


/**
 * @brief Set the camera size in width and height (this provably matches the screen resolution
 * @param size the width and height of the camera
 */
void camera_set_dimensions(Vector2D size);

/**
 * @brief get the camera's current dimenstions (extent)
 * @return a vector containing the width and height of the camera
 */
Vector2D camera_get_dimensions();

/**
 * @brief check if a point is on screen
 */
Bool camera_point_on_screen(Vector2D point);


/**
 * @brief check if the rectangle is on screen
 */
Bool camera_rect_on_screen(SDL_Rect rect);

/**
 * @brief set the camera position relative to the gameworld
 * @param position the vector to set the camera to
 */
void camera_set_position(Vector2D position);

/**
 * @brief move the camera relative to its current position
 * @param move the ammount to move the camera by
 */
void camera_move(Vector2D move);

/**
 * @brief get the camera's world position
 * @return the camera's position in world coordinates
 */
Vector2D camera_get_position();

/**
 * @brief return the rectangle that describes the camera;
 * @return the rectangle
 * @note hehe
 */
SDL_Rect camera_get_rect();

/**
 * @brief return the offset for things to be drawn relative to the camera
 * @return the inverse of the camera coordinates
 */
Vector2D camera_get_offset();

#endif
