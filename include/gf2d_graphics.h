#ifndef __GF2D_GRAPHICS_C__
#define __GF2D_GRAPHICS_C__

#include <SDL.h>

#include "gfc_types.h"
#include "gfc_vector.h"

/**
 * @brief initializes SDL and setups up basic window and rendering contexts
 * @param windowName the name that you would want displayed on the game window
 * @param viewWidth how wide you want your game window to be
 * @param viewHeight how high you want your game window to be
 * @param renderWidth How much draw width you want to work with logically
 * @param renderHeight How much draw height you want to work with logically
 * @param bgcolor what you want the default background color to be
 * @param fullscreen if you want the window to render full screen or not
 */
void gf2d_graphics_initialize(
    char *windowName,
    int viewWidth,
    int viewHeight,
    int renderWidth,
    int renderHeight,
    Vector4D bgcolor,
    Bool fullscreen
);

/**
 * @brief sets the amount of delay to aim for between frames.
 * @param frameDelay the amount of time, in milliseconds, that each frame should take
 */
void gf2d_graphics_set_frame_delay(Uint32 frameDelay);

/**
 * @brief gets the functional number of frames rendered per second
 * @return the current frame rate
 */
float gf2d_graphics_get_frames_per_second();

/**
 * @brief get the current rendering context
 * @return NULL on error or the current rendering context
 */
SDL_Renderer *gf2d_graphics_get_renderer();

/**
 * @brief render the current frame to screen
 */
void gf2d_grahics_next_frame();

/**
 * @brief clears drawing buffer.  Should be called each frame before drawing
 */
void gf2d_graphics_clear_screen();

/*drawing support functions*/

/**
 * @brief creates an SDL_Surface that is compatible with the current drawing context
 * @param w the width of the surface to create
 * @param h the height of the surface to create
 * @return NULL on error or the SDL_Surface created
 */
SDL_Surface *gf2d_graphics_create_surface(Uint32 w,Uint32 h);

/**
 * @brief draw a surface to the current drawing context
 * @param surface the surface to draw
 * @param srcRect the clip rect to draw from (if NULL, whole surface is used)
 * @param dstRect the position data of the rext is used for the destination position
 */
void gf2d_graphics_blit_surface_to_screen(SDL_Surface *surface,const SDL_Rect * srcRect,SDL_Rect * dstRect);

/**
 * @brief convert an SDL Surface to the format compatible with the rendering context
 * @param surface a pointer to your surface pointer.  The surface is automatically freed upon success
 * @returns NULL on error, or the new SDL Surface upon success
 */
SDL_Surface *gf2d_graphics_screen_convert(SDL_Surface **surface);

#endif
