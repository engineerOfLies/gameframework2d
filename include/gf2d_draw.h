#ifndef __GF2D_DRAW_H__
#define __GF2D_DRAW_H__

#include <SDL.h>
#include "gf2d_vector.h"
/**
 * @brief draw a pixel to the current rendering context
 * @param pixel the screen position to draw at
 * @param color the color to draw with
 */
void gf2d_draw_pixel(Vector2D pixel,Vector4D color);

void gf2d_draw_circle(Vector2D center, int radius, Vector4D color);

void gf2d_draw_rect(SDL_Rect rect,Vector4D color);


#endif
