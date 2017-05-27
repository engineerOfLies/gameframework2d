#include "gf2d_shape.h"
#include "gf2d_draw.h"

Rect gf2d_rect(float x, float y, float w, float h)
{
    Rect r;
    gf2d_rect_set(r,x,y,w,h);
    return r;
}

void gf2d_rect_draw(Rect r,Color color)
{
    gf2d_draw_rect(gf2d_rect_to_sdl_rect(r),gf2d_color_to_vector4(color));
}

void gf2d_shape_draw(Shape shape,Color color)
{
    switch(shape.type)
    {
        case ST_RECT:
            gf2d_rect_draw(shape.s.r,color);
            break;
        case ST_CIRCLE:
            gf2d_draw_circle(shape.s.c.center, shape.s.c.radius,gf2d_color_to_vector4(color));
            break;
    }
}

Circle gf2d_circle(float x, float y, float r)
{
    Circle c;
    gf2d_circle_set(c,x,y,r);
    return c;
}

SDL_Rect gf2d_rect_to_sdl_rect(Rect r)
{
    SDL_Rect r2;
    r2.x = r.x;
    r2.y = r.y;
    r2.w = r.w;
    r2.h = r.h;
    return r2;
}

Rect gf2d_rect_from_sdl_rect(SDL_Rect r)
{
    Rect r2;
    r2.x = r.x;
    r2.y = r.y;
    r2.w = r.w;
    r2.h = r.h;
    return r2;
}

Uint8 gf2d_rect_overlap(Rect a,Rect b)
{
    if ((a.x > b.x+b.w)||
        (b.x > a.x+a.w)||
        (a.y > b.y+b.h)||
        (b.y > a.y+a.h))
    {
        return 0;
    }
    return 1;
}

Uint8 gf2d_circle_overlap(Circle a, Circle b)
{
    Vector2D v;
    vector2d_sub(v,a.center,b.center);
    if (vector2d_magnitude_compare(v,a.radius+b.radius) <= 0) return 1;
    return 0;
}

Uint8 gf2d_circle_rect_overlap(Circle a, Rect b)
{
    Rect centered;
    Vector2D newCenter;
    gf2d_rect_set(centered,-b.w/2,-b.h/2,b.w,b.h);
    vector2d_set(newCenter, fabs(a.center.x + b.x - centered.x), fabs(a.center.y + b.y - centered.y));
    if (vector2d_magnitude_compare(vector2d(newCenter.x - centered.w/2,newCenter.y - centered.y),a.radius) <= 0) return 1;
    if (newCenter.x > centered.x + centered.w + a.radius)return 0;
    if (newCenter.y < centered.y - a.radius)return 0;
    return 1;
}

Shape gf2d_shape_rect(float x, float y, float w, float h)
{
    Shape shape;
    shape.type = ST_RECT;
    shape.s.r.x = x;
    shape.s.r.y = y;
    shape.s.r.w = w;
    shape.s.r.h = h;
    return shape;
}

Shape gf2d_shape_from_rect(Rect r)
{
    Shape shape;
    shape.type = ST_RECT;
    shape.s.r.x = r.x;
    shape.s.r.y = r.y;
    shape.s.r.w = r.w;
    shape.s.r.h = r.h;
    return shape;
}

Shape gf2d_shape_sdl_rect(SDL_Rect r)
{
    Shape shape;
    shape.type = ST_RECT;
    shape.s.r.x = r.x;
    shape.s.r.y = r.y;
    shape.s.r.w = r.w;
    shape.s.r.h = r.h;
    return shape;
}

Shape gf2d_shape_cirlce(float x, float y, float r)
{
    Shape shape;
    shape.type = ST_CIRCLE;
    shape.s.c.center.x = x; 
    shape.s.c.center.y = y;
    shape.s.c.radius = r;
    return shape;
}

Shape gf2d_shape_from_cirlce(Circle c)
{
    Shape shape;
    shape.type = ST_CIRCLE;
    shape.s.c.center.x = c.center.x;
    shape.s.c.center.y = c.center.y;
    shape.s.c.radius = c.radius;
    return shape;
}

void gf2d_shape_copy(Shape *dst,Shape src)
{
    if (!dst)return;
    memcpy(dst,&src,sizeof(Shape));
}

void gf2d_shape_move(Shape *shape,Vector2D move)
{
    if (!shape)return;
    shape->s.r.x += move.x;
    shape->s.r.y += move.y;
}

/*eol@eof*/
