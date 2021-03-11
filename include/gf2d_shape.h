#ifndef __GF2D_SHAPE_H__
#define __GF2D_SHAPE_H__

#include <SDL.h>
#include "simple_json.h"
#include "gfc_vector.h"
#include "gfc_color.h"

typedef struct
{
    double x1,y1,x2,y2;
}Edge;

typedef struct
{
    double       x,y,r;
}Circle;

typedef struct
{
    double x,y,w,h;
}Rect;

typedef enum
{
    ST_RECT,
    ST_CIRCLE,
    ST_EDGE
}ShapeTypes;


typedef struct
{
    ShapeTypes type;
    union
    {
        Circle c;
        Rect r;
        Edge e;
    }s;
}Shape;

/**
 * @brief parse a shape out of json
 * @param json the json object to parse
 * @param shape pointer to the shape to store the information into
 * @return 0 on error or 1 on success
 */
int gf2d_shape_from_json(SJson *json,Shape *shape);

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
 * @brief convert a rect to a vector4d
 * @param r the rect to convert
 * @returns a vector4f
 */
Vector4D gf2d_rect_to_vector4d(Rect r);

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
Shape gf2d_shape_circle(float x, float y, float r);

/**
 * @brief make a shape based on a gf2d Circle
 * @param c the circle to make the shape with
 * @return the shape
 */
Shape gf2d_shape_from_circle(Circle c);

/**
 * @brief make an edge shape basesd on the points provided
 * @param x1 the X component of starting point
 * @param y1 the Y component of starting point
 * @param x2 the X component of ending point
 * @param y2 the Y component of ending point
 * @return the shape
 */
Shape gf2d_shape_edge(float x1,float y1,float x2,float y2);

/**
 * @brief make a shape based on a gf2d Edge
 * @param e the edge to make the shape with
 * @return the shape
 */
Shape gf2d_shape_from_edge(Edge e);

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
 * @brief determine if the point lies within the rectangle
 * @param p the point to check
 * @param r the rectangle to check
 * @return true if the point is inside the rectangle, false otherwise
 */
Uint8 gf2d_point_in_rect(Vector2D p,Rect r);

/**
 * @brief check if two rectangles are overlapping
 * @param a rect A
 * @param b rect B
 * @return true if there is any overlap, false otherwise
 */
Uint8 gf2d_rect_overlap(Rect a,Rect b);

/**
 * @brief check if two rectangles are overlapping
 * @param a rect A
 * @param b rect B
 * @param poc if set the point of contact is written here
 * @param normal if provided, this will be populated with the normal for the point of impact
 * @return true if there is any overlap, false otherwise
 */
Uint8 gf2d_rect_overlap_poc(Rect a,Rect b,Vector2D *poc, Vector2D *normal);

/**
 * @brief draw a rect to the screen
 * @param r the rectangle to draw
 * @param color the color to draw it with
 */
void gf2d_rect_draw(Rect r,Color color);

void gf2d_rect_draw_filled(Rect r,Color color);

/**
 * @brief make a GF2D Circle
 * @param x the position of the circle center
 * @param y the position of the circle center
 * @param r the radius of the circle
 */
Circle gf2d_circle(float x, float y, float r);

/**
 * @brief set all the parameters of a GF2D circle at once
 * @param
 */
#define gf2d_circle_set(circle,a,b,c) (circle.x = a,circle.y = b, circle.r = c)

/**
 * @brief check if the point lies within the circle c
 * @param p the point to check
 * @param c the circle to check
 * @return true if the point is in the circle, false otherwise
 */
Uint8 gf2d_point_in_cicle(Vector2D p,Circle c);

/**
 * @brief check if two circles are overlapping
 * @param a circle A
 * @param b circle B
 * @param returns true is there is overlap, false otherwise
 */
Uint8 gf2d_circle_overlap(Circle a, Circle b);

/**
 * @brief check if two circles are overlapping and get the point of contact
 * @param a circle A
 * @param b circle B
 * @param poc if set the point of contact is written here
 * @param normal if provided, this will be populated with the normal for the point of impact
 * @param returns true is there is overlap, false otherwise
 */
Uint8 gf2d_circle_overlap_poc(Circle a, Circle b,Vector2D *poc,Vector2D *normal);

/**
 * @brief check if a circle and rect overlap
 * @param a the Circle
 * @param b the Rect
 * @return true if there is any overlap, false otherwise
 */
Uint8 gf2d_circle_rect_overlap(Circle a, Rect b);

/**
 * @brief check if a circle and rect overlap
 * @param a the Circle
 * @param b the Rect
 * @param poc if set the point of contact is written here
 * @param normal if provided, this will be populated with the normal for the point of impact
 * @return true if there is any overlap, false otherwise
 */
Uint8 gf2d_circle_rect_overlap_poc(Circle a, Rect b,Vector2D *poc,Vector2D * normal);

/**@brief check if a shape is overlapping another shape
 * @param a one shape
 * @param b the other shape
 * @return true is there is overlap, false otherwise
 */
Uint8 gf2d_shape_overlap(Shape a, Shape b);

/**@brief check if a shape is overlapping another shape
 * @param a one shape
 * @param b the other shape
 * @return true is there is overlap, false otherwise
 * @param poc if set the point of contact is written here
 * @param normal if provided, this will be populated with the normal for the point of impact
 */
Uint8 gf2d_shape_overlap_poc(Shape a, Shape b, Vector2D *poc, Vector2D *normal);

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
 * @param offset a positional offset to draw the space at
 */
void gf2d_shape_draw(Shape shape,Color color,Vector2D offset);

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

/**
 * @brief make an edge
 * @param x1 the X component of starting point
 * @param y1 the Y component of starting point
 * @param x2 the X component of ending point
 * @param y2 the Y component of ending point
 * @return a set edge
 */
Edge gf2d_edge(float x1, float y1, float x2, float y2);

/**
 * @brief make an edge from two vectors
 * @param a the starting point vector
 * @param b the ending point vector
 * @return a set edge
 */
Edge gf2d_edge_from_vectors(Vector2D a,Vector2D b);

/**
 * @brief set an edge
 * @param e the edge to set
 * @param a the X component of starting point
 * @param b the Y component of starting point
 * @param c the X component of ending point
 * @param d the Y component of ending point
 */
#define gf2d_edge_set(e,a,b,c,d) (e.x1 = a,e.y1 = b, e.x2 = c, e.y2 = d)

/**
 * @brief copy the contents of the src edge into the dst edge
 * @param dst the destination of the copy
 * @param src the source of the copy
 */
#define gf2d_edge_copy(dst,src) (dst.x1 = src.x1,dst.y1 = src.y1,dst.x2 = src.x2,dst.y2 = src.y2)

/**
 * @brief determine if and where two edges intersect
 * @param a edge A
 * @param b edge B
 * @param contact (optional) if provided this will be populated with the intersection point if there was an intersection
 * @param normal (optional) if provided this will be populated with a vector perpendicular to b
 * @return true on intersection, false otherwise
 */
Uint8 gf2d_edge_intersect_poc(
    Edge a,
    Edge b,
    Vector2D *contact,
    Vector2D *normal);

/**
 * @brief determine if and where two edges intersect
 * @param a edge A
 * @param b edge B
 * @return true on intersection, false otherwise
 */
Uint8 gf2d_edge_intersect(Edge a,Edge b);

/**
 * @brief check if an edge intersects a rectangle
 * @param e the edge to test
 * @param r the rect to rest
 * @return true if there is an intersection, false otherwise
 */
Uint8 gf2d_edge_rect_intersection(Edge e, Rect r);

/**
 * @brief check if an edge intersects a rectangle and get the point of contact and normal
 * @param e the edge to test
 * @param r the rect to rest
 * @param contact (optional) if provided this will be populated with the intersection point if there was an intersection
 * @param normal (optional) if provided this will be populated with a vector perpendicular to b
 * @return true if there is an intersection, false otherwise
 */
Uint8 gf2d_edge_rect_intersection_poc(Edge e, Rect r,Vector2D *poc,Vector2D *normal);

/**
 * @brief check if an edge intersects a circle
 * @param e the edge to check
 * @param c the circle to check
 * @return true if there is an intersection, false otherwise
 */
Uint8 gf2d_edge_circle_intersection(Edge e,Circle c);

/**
 * @brief check if an edge intersects a circle and get point of contact
 * @param e the edge to check
 * @param c the circle to check
 * @param poc (optional) if provided this will be populated with the intersection point if there was an intersection
 * @param normal if provided, this will be populated with the normal for the point of impact
 * @return true if there is an intersection, false otherwise
 */
Uint8 gf2d_edge_circle_intersection_poc(Edge e,Circle c,Vector2D *poc,Vector2D *normal);

/**
 * @brief check if the edge intersects the shape
 * @param e the edge the test
 * @param s the shape to test
 * @param true if the shape and edge intersect, false otherwise
 */
Uint8 gf2d_edge_intersect_shape(Edge e,Shape s);

/**
 * @brief check if the edge intersects the shape
 * @param e the edge the test
 * @param s the shape to test
 * @param poc (optional) if provided this will be populated with the intersection point if there was an intersection
 * @param normal if provided, this will be populated with the normal for the point of impact
 * @param true if the shape and edge intersect, false otherwise
 */
Uint8 gf2d_edge_intersect_shape_poc(Edge e,Shape s,Vector2D *poc,Vector2D *normal);

/**
 * @brief echo out the shape information to log (and stdout)
 * @param shape the shape information to echo
 */
void gf2d_shape_slog(Shape shape);

void gf2d_edge_slog(Edge e);
void gf2d_circle_slog(Circle c);

/**
 * @brief echo out the rect information to log (and stdout)
 * @param r the rect information to echo
 */
void gf2d_rect_slog(Rect r);

/**
 * @brief get the minum rectangle that bounds the shape
 * @param shape the shape to get the bounds of
 * @return the bounding rectangle
 */
Rect gf2d_shape_get_bounds(Shape shape);

/**
 * @brief get the normal of the shape relative to a reference shape
 * @param s the shape to get the normal from
 * @param s2 the normal should be pointing towards this shape
 * @return an empty vector if the refPoint is in the shape, a unit vector otherwise
 */
Vector2D gf2d_shape_get_normal_for_shape(Shape s, Shape s2);

/**
 * @brief get the normal of the shape relative to a reference for a given circle
 * @param s the shape to get the normal from
 * @param c the normal should be pointing towards this shape
 * @return an empty vector if the refPoint is in the shape, a unit vector otherwise
 */
Vector2D gf2d_shape_get_normal_for_cirlce(Shape s, Circle c);

/**
 * @brief get the center point of a rect
 * @param r the rectangle to use
 * @return the center point of the rect
 */
Vector2D gf2d_rect_get_center_point(Rect r);

#endif
