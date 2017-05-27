#ifndef __GF2D_SHAPE_H__
#define __GF2D_SHAPE_H__

#include "gf2d_vector.h"
#include "gf2d_color.h"
#include <SDL.h>

typedef struct
{
    Vector2D    center;
    float       radius;
}Circle;

typedef struct
{
    float x,y,w,h;
}Rect;

typedef enum
{
    ST_RECT,
    ST_CIRCLE
}ShapeTypes;


typedef struct
{
    ShapeTypes type;
    union
    {
        Circle c;
        Rect r;
    }s;
}Shape;

/**
 * @brief make a GF2D Rect
 * @param x the left position of the rect
 * @param y the top position of the rect
 * @param w the width of the rect
 * @param h the height of the rect
 * @return a GF2D rect
 */
Rect gf2d_rect(float x, float y, float w, float h);

/**
 * @brief make a shape based on a rect
 * @param x the left side
 * @param y the top of the rect
 * @param w the width
 * @param h the height
 */
Shape gf2d_shape_rect(float x, float y, float w, float h);

/**
 * @brief make a shape based on a gf2d rect
 * @param r the rect to base it on
 */
Shape gf2d_shape_from_rect(Rect r);

/**
 * @brief make a shape based on a SDL rect
 * @param r the rect to base it on
 */
Shape gf2d_shape_from_sdl_rect(SDL_Rect r);

/**
 * @brief make a shape based on a circle
 * @param x the center x
 * @param y the center y
 * @param r the radius
 */
Shape gf2d_shape_cirlce(float x, float y, float r);

/**
 * @brief make a shape based on a gf2d Circle
 * @param c the circle to make the shape with
 */
Shape gf2d_shape_from_cirlce(Circle c);

/**
 * @brief set all parameters of a GF2D rect at once
 * @param r the rectangle to set
 * @param a the x component
 * @param b the y component
 * @param c the width
 * @param d the height
 */
#ifndef gf2d_rect_set
#define gf2d_rect_set(r,a,b,c,d) (r.x = a,r.y = b, r.w =c, r.h = d)
#endif

/**
 * @brief copy the contents of one rect into another
 * @param dst the rect to be copied into
 * @param src the rect to be copied from
 */
#define gf2d_rect_copy(dst,src) (dst.x = src.x,dst.y = src.y,dst.w = src.w,dst.h = src.h)
/**
 * @brief check if two rectangles are overlapping
 * @param a rect A
 * @param b rect B
 * @return true if there is any overlap, false otherwise
 */
Uint8 gf2d_rect_overlap(Rect a,Rect b);

/**
 * @brief draw a rect to the screen
 * @param r the rectangle to draw
 * @param color the color to draw it with
 */
void gf2d_rect_draw(Rect r,Color color);

/**
 * @brief make a GF2D Circle
 * @param x the position of the cirlce center
 * @param y the position of the cirlce center
 * @param r the radius of the cirlce
 */
Circle gf2d_circle(float x, float y, float r);

/**
 * @brief set all the parameters of a GF2D circle at once
 * @param
 */
#define gf2d_circle_set(circle,a,b,c) (circle.center.x = a,circle.center.y = b, circle.radius =c)

/**
 * @brief check if two circles are overlapping
 * @param a circle A
 * @param b circle B
 * @param returns true is there is overlap, false otherwise
 */
Uint8 gf2d_circle_overlap(Circle a, Circle b);

/**
 * @brief check if a circle and rect overlap
 * @param a the Circle
 * @param b the Rect
 * @return true if there is any overlap, false otherwise
 */
Uint8 gf2d_circle_rect_overlap(Circle a, Rect b);

/**
 * @brief convert a GF2D rect to an SDL rect
 * @param r the GF2D rect to convert
 * @return an SDL rect
 */
SDL_Rect gf2d_rect_to_sdl_rect(Rect r);

/**
 * @brief convert an SDL Rect to a GF2D rect
 * @param r the SDL Rect to convert
 * @return a GF2D rect
 */
Rect gf2d_rect_from_sdl_rect(SDL_Rect r);

/**
 * @brief draw the shape with the color specified
 * @param shape the shape to draw
 * @param color the color to draw with
 */
void gf2d_shape_draw(Shape shape,Color color);

/**
 * @brief change the position of the shape based on the movement vector
 * @param shape a pointer to the shape to move
 * @param move the amount to move the shape
 */
void gf2d_shape_move(Shape *shape,Vector2D move);

/**
 * @brief copy one shape into another
 * @param dst a pointer to the shape you want to copy into
 * @param src the shape you want to copy FROM
 */
void gf2d_shape_copy(Shape *dst,Shape src);

#endif
