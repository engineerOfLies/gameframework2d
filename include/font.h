#ifndef __FONT_H__
#define __FONT_H__

#include "gfc_text.h"
#include "gfc_color.h"

typedef enum
{
    FS_small,
    FS_medium,
    FS_large,
    FS_MAX
}FontStyles;


/**
 * @brief init the font system
 * auto closes on exit
 */
void font_init();

/**
 * @brief render text to the screen
 * @param text what to render
 * @param style what style of font to render with
 * @param color the color of the text
 * @param positon where on the screen to draw to
 */
void font_draw_text(const char *text,FontStyles style, Color color,Vector2D position);

/**
 * @brief periodically call this to cleanup any internal cache
 */
void font_cleanup();

#endif
