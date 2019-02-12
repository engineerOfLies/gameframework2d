#ifndef __GF2D_SPRITE_H__
#define __GF2D_SPRITE_H__

#include <SDL.h>
#include "gf2d_types.h"
#include "gf2d_vector.h"
#include "gf2d_text.h"

typedef struct Sprite_S
{
    int ref_count;
    TextLine filepath;
    SDL_Texture *texture;
    SDL_Surface *surface;
    Uint32 frames_per_line;
    Uint32 frame_w,frame_h;
}Sprite;

/**
 * @brief initializes the sprite manager 
 * @param max the maximum number of sprites the system will handle at once
 */
void gf2d_sprite_init(Uint32 max);

/**
 * @brief loads a simple image using the sprite system
 * @param filename the image file to load
 * @returns NULL on error or the sprite loaded
 */
Sprite *gf2d_sprite_load_image(char *filename);

/**
 * @brief draw a simple image to screen at the position provided
 * @param image the sprite image to draw
 * @param position the x and y position to draw the image at (top left corner)
 */
void gf2d_sprite_draw_image(Sprite *image,Vector2D position);

/**
 * @brief loads a sprite from file using the sprite system
 * @param filename the sprite sheet to load
 * @param frameWidth the width of an individual sprite frame
 * @param frameHeigh the height of an individual sprite frame
 * @param framesPerLine how many frames go in a row in the sprite sheet
 * @param keepSurface if you plan on doing surface editing with this sprite, set to true otherwise the surface data is cleaned up
 */
Sprite *gf2d_sprite_load_all(
    char *filename,
    Sint32 frameWidth,
    Sint32 frameHeigh,
    Sint32 framesPerLine,
    Bool    keepSurface
);

/**
 * @brief draw a sprite to the screen
 * @param sprite the sprite to draw
 * @param position here on the screen to draw it
 * @param scale (optional) if you want to scale the sprite
 * @param scaleCenter (optional) scale the sprite from the position in the sprite
 * @param rotation (optional) the position of the rotation center and the angle in degrees to rotate
 * @param flip (optional) set to 1 if you want to flip in the horizontal,vertical axis
 * @param colorShift (optional) if you want to gamma shift the sprite or set an alpha value
 * @param frame which frame to draw
 */
void gf2d_sprite_draw(
    Sprite * sprite,
    Vector2D position,
    Vector2D * scale,
    Vector2D * scaleCenter,
    Vector3D * rotation,
    Vector2D * flip,
    Vector4D * colorShift,
    Uint32 frame);

/**
 * @brief free a sprite back to the sprite manager
 * Stays in memory until the space is needed
 * @param sprite the sprite to free
 */
void gf2d_sprite_free(Sprite *sprite);

/**
 * @brief delete all loaded sprites from memory
 * does not close the sprite system
 */
void gf2d_sprite_clear_all();

/**
 * @brief draw a sprite to a surface instead of to the screen.
 * @note sprite must have been loaded with surface data preserved
 * @param sprite the sprite to draw
 * @param position where on the target surface to draw it to
 * @param scale (optional) if provided the sprite will be scaled by this factor
 * @param scaleCenter (optional) if provided, this will be used to determin the scaling point, (0,0) is default
 * @param frame the frame to draw to the surface
 * @param surface the surface to draw to
 */
void gf2d_sprite_draw_to_surface(
    Sprite *sprite,
    Vector2D position,
    Vector2D * scale,
    Vector2D * scaleCenter,
    Uint32 frame,
    SDL_Surface *surface
);

/**
 * @brief allocate space for a sprite
 * @note both texture and sprite data is left blank
 * @return NULL on error or out of memory, a blank sprite otherwise
 */
Sprite *gf2d_sprite_new();


#endif
