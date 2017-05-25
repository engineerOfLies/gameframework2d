#ifndef __GF2D_COLOR_H__
#define __GF2D_COLOR_H__

#include <SDL.h>
#include "gf2d_vector.h"

typedef enum
{
    CT_RGBAf,   /**<RGBA in floating point 0-1*/
    CT_RGBA8,   /**<RGBA in 8 bit int 0-255*/
    CT_HSL,     /**<Hue,Saturation and Lightness*/
    CT_HEX      /**<Hex code 0xFFFFFF*/
}ColorType;

typedef struct
{
    float r;    /**<red value from 0-1, or the HUE*/
    float g;    /**<green value or Saturation*/
    float b;    /**<blue value or Lightness*/
    float a;    /**<alpha, or transparency from 0 (clear) to opaque 1*/
    ColorType ct;   /**<how this structure is used*/
}Color;

#define gf2d_color_copy(dst,src) (dst.r = src.r,dst.g = src.g,dst.b = src.b,dst.a = src.a,dst.ct = src.ct)

/**
 * @brief create and return a color from floating point RGBA values
 * values over 1 are clamped to 1, values under 0 are clamped to zero
 * @param r red value 0 - 1
 * @param g green value 0 - 1
 * @param b blue value 0 - 1
 * @param a alpha value 0 - 1
 * @returns a newly set color
 */
Color gf2d_color(float r,float g,float b,float a);
/**
 * @brief create and return a color from 8 bit integer RGBA values
 * @param r red value 0 - 255
 * @param g green value 0 - 255
 * @param b blue value 0 - 255
 * @param a alpha value 0 - 255
 * @returns a newly set color
 */
Color gf2d_color8(Uint8 r,Uint8 g,Uint8 b,Uint8 a);
/**
 * @brief create and return a color from floating point RGBA values
 * @param h hue value 0 - 1
 * @param s saturation value 0 - 1
 * @param l lightness value 0 - 1
 * @param a alpha value 0 - 1
 * @returns a newly set color
 */
Color gf2d_color_hsl(float h,float s,float l,float a);
/**
 * @brief create and return a color from floating point RGBA values
 * @param hex the hex value of the color
 * @returns a newly set color
 */
Color gf2d_color_hex(Uint32 hex);

/**
 * @brief convert a color to floating point representation
 * @param color the color to convert
 * @return a new color in the format CT_RGBAf
 */
Color gf2d_color_to_float(Color color);

/**
 * @brief convert a color to 8 bit integer representation
 * @param color the color to convert
 * @return a new color in the format CT_RGBA8
 */
Color gf2d_color_to_int8(Color color);

/**
 * @brief convert a color to Hue Saturation Lightness (and Alpha) representation
 * @param color the color to convert
 * @return a new color in the format CT_HSL
 */
Color gf2d_color_to_hsla(Color color);

/**
 * @brief convert a color to Hex representation
 * @param color the color to convert
 * @return a new color in the format CT_HEX
 */
Uint32 gf2d_color_to_hex(Color color);

/**
 * @brief convert a SDL color to gf2d floating point representation
 * @param color the color to convert
 * @return a new color in the format CT_RGBAf
 */
Color gf2d_color_from_sdl(SDL_Color color);

/**
 * @brief convert a gf2d color to SDL color format
 * @param color the color to convert
 * @return a new SDL_Color
 */
SDL_Color gf2d_color_to_sdl(Color color);

/**
 * @brief create a color from a vector where x = r, y = g, z = b, w = a
 * @param vector the vector to convert
 * @return a new color in the format CT_HSL
 */
Color gf2d_color_from_vector4(Vector4D vector);

/**
 * @brief convert a color to a vector where x = r, y = g, z = b, w = a
 * @param color the color to convert
 * @return a vector set to the color provided
 */
Vector4D gf2d_color_to_vector4(Color color);

/**
 * @brief Get the Hue value of a color.  It may convert internally as needed.
 * @param color the color to check
 * @return the value of the Hue of the color (0-360)
 */
float gf2d_color_get_hue(Color color);

/**
 * @brief Sets the hue of the color provided.  Leaves format unchanged.  Saturation and Value are also unchanged
 * if saturation is zero this may not show a change
 * @param hue the hue to change to
 * @param color the color to change
 * @return a new color in the format CT_HSL
 */
void gf2d_color_set_hue(float hue,Color *color);

/**
 * @brief adds colors a and b and saves the result in dst
 * @param dst if NULL this function is a no op.  Result saved here
 * @param a the format for a is used as the resulting format.  Conversion happen before adding
 * @param b the color to add to a
 */
void gf2d_color_add(Color *dst,Color a,Color b);

/**
 * @brief make sure the values in the color are within acceptable ranges
 * Note this will break any colors used as a vector that contain negative values
 * @param color the color to clamp
 * @return the new color within specified range
 */
Color gf2d_color_clamp(Color color);

 #endif
