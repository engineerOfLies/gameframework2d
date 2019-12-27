#ifndef __GF2D_FONT_H__
#define __GF2D_FONT_H__
/**
 * gf2d_font
 * @license The MIT License (MIT)
   @copyright Copyright (c) 2015 EngineerOfLies
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

/**
 * @purpose the GF2d text library provides common string support for fixed length
 *  strings.  For use with unlimited strings mgl will use glib's GString type
 */
#include <string.h>
#include <SDL_ttf.h>

#include "gfc_types.h"
#include "gfc_color.h"

typedef enum
{
    FT_Normal,
    FT_Small,
    FT_H1,
    FT_H2,
    FT_H3,
    FT_H4,
    FT_H5,
    FT_H6,
    FT_MAX
}FontTypes;


typedef struct
{
    TextLine filename;
    TTF_Font *font;
    Uint32  pointSize;
}Font;

/**
 * @brief initialized text drawing system
 * @param configFile the file to load font information from
 */
void gf2d_font_init(char *configFile);

void gf2d_font_draw_line_named(char *text,char *filename,Color color, Vector2D position);
void gf2d_font_draw_line_tag(char *text,FontTypes tag,Color color, Vector2D position);
void gf2d_font_draw_line(char *text,Font *font,Color color, Vector2D position);

Vector2D gf2d_font_get_bounds_tag(char *text,FontTypes tag);
Vector2D gf2d_font_get_bounds(char *text,Font *font);

#endif
