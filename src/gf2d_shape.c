#include "gf2d_shape.h"


Rect gf2d_rect(float x, float y, float w, float h)
{
    Rect r;
    gf2d_rect_set(r,x,y,w,h);
    return r;
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

/*eol@eof*/
