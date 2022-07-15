#ifndef __GF2D_LIGHTING_H__
#define __GF2D_LIGHTING_H__

#include "gf2d_sprite.h"

typedef struct
{
    Color darkLevel;    /**<color for the darkness fill*/
    Sprite *layer;      /**<lighting mask layer*/
}LightingMask;

/**
 * @brief initialize the lighting system
 * @param file a json file containing the names and filepaths to the lights
 */
void gf2d_lighting_system_init(const char *file);

/**
 * @brief create a lighting layer mask of the specified size
 * @param size the width of height of the layer. must be non-zero
 * @return NULL on error, or the mask created, but blank
 */
LightingMask *gf2d_lighting_layer_new(Vector2D size);

/**
 * @brief free a previously created lighting layer mask
 * @param mask the layer mask to free
 */
void gf2d_lighting_layer_free(LightingMask *mask);

/**
 * @brief draw the lighting mask to the screen
 * @param mask the lighting mask
 * @param offset for rendering (probably the camera)
 */
void gf2d_lighting_layer_draw(LightingMask *mask,Vector2D offset);

/**
 * @brief add an area light (circle) to the lighting mask
 * @param mask the lighting mask to update
 * @param position where to render the light
 * @param color the color of the light
 * @param size the scale of the light.  
 */
void gf2d_lighting_set_area_light(LightingMask *mask,Vector2D position,Color color, float size);

/**
 * @brief reset a lighting mask back to the darkness level
 * @param mask the mask to reset
 */
void gf2d_lighting_reset_layer(LightingMask *mask);

#endif
