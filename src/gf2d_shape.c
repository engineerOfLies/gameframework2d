#include "gf2d_shape.h"
#include "gf2d_draw.h"
#include "simple_logger.h"

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
            gf2d_draw_circle(vector2d(shape.s.c.x,shape.s.c.y), shape.s.c.r,gf2d_color_to_vector4(color));
            break;
        case ST_EDGE:
            gf2d_draw_line(vector2d(shape.s.e.x1,shape.s.e.y1),vector2d(shape.s.e.x2,shape.s.e.y2), gf2d_color_to_vector4(color));
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

Uint8 gf2d_point_in_rect(Vector2D p,Rect r)
{
    if ((p.x >= r.x) && (p.x <= r.x + r.w)&&
        (p.y >= r.y) && (p.y <= r.y + r.h))
        return 1;
    return 0;
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

Uint8 gf2d_point_in_cicle(Vector2D p,Circle c)
{
    if (vector2d_magnitude_compare(vector2d(c.x-p.x,c.y-p.y),c.r) <= 0)return 1;
    return 0;
}

Uint8 gf2d_circle_overlap(Circle a, Circle b)
{
    Vector2D v;
    vector2d_set(v,a.x - b.x,a.y - b.y);
    if (vector2d_magnitude_compare(v,a.r+b.r) <= 0) return 1;
    return 0;
}

Uint8 gf2d_circle_rect_overlap(Circle a, Rect b)
{
    Rect centered;
    Vector2D newCenter;
    gf2d_rect_set(centered,-b.w/2,-b.h/2,b.w,b.h);
    vector2d_set(newCenter, fabs(a.x + b.x - centered.x), fabs(a.y + b.y - centered.y));
    if (vector2d_magnitude_compare(vector2d(newCenter.x - centered.w/2,newCenter.y - centered.y),a.r) <= 0) return 1;
    if (newCenter.x > centered.x + centered.w + a.r)return 0;
    if (newCenter.y < centered.y - a.r)return 0;
    return 1;
}

Uint8 gf2d_shape_overlap(Shape a, Shape b)
{
    switch(a.type)
    {
        case ST_CIRCLE:
            switch(b.type)
            {
                case ST_CIRCLE:
                    return gf2d_circle_overlap(a.s.c,b.s.c);
                case ST_RECT:
                    return gf2d_circle_rect_overlap(a.s.c,b.s.r);
                case ST_EDGE:
                    return gf2d_edge_circle_intersection(b.s.e,a.s.c);
            }
        case ST_RECT:
            switch (b.type)
            {
                case ST_RECT:
                    return gf2d_rect_overlap(a.s.r,b.s.r);
                case ST_CIRCLE:
                    return gf2d_circle_rect_overlap(b.s.c,a.s.r);
                case ST_EDGE:
                    return gf2d_edge_rect_intersection(b.s.e, a.s.r);
            }
        case ST_EDGE:
            switch (b.type)
            {
                case ST_EDGE:
                    return gf2d_edge_intersect(a.s.e,b.s.e,NULL);
                case ST_CIRCLE:
                    return gf2d_edge_circle_intersection(a.s.e,b.s.c);
                case ST_RECT:
                    return gf2d_edge_rect_intersection(a.s.e, b.s.r);
            }
    }
    return 0;
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
    shape.s.c.x = x; 
    shape.s.c.y = y;
    shape.s.c.r = r;
    return shape;
}

Shape gf2d_shape_from_cirlce(Circle c)
{
    Shape shape;
    shape.type = ST_CIRCLE;
    shape.s.c.x = c.x;
    shape.s.c.y = c.y;
    shape.s.c.r = c.r;
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

Uint8 gf2d_edge_intersect(
  Edge a,
  Edge b,
  Vector2D *contact)
{
  float testx, testy;
  float Ua,Ub,Uden;
  
  Uden = ((b.y2 - b.y1)*(a.x2 - a.x1)) - ((b.x2 - b.x1)*(a.y2 - a.y1));
  if(Uden == 0)
  {
    return 0;/*parallel, can't hit*/
  }
    
  Ua = (((b.x2 - b.x1)*(a.y1 - b.y1))-((b.y2 - b.y1)*(a.x1 - b.x1))) / Uden;
  Ub = (((a.x2 - a.x1)*(a.y1 - b.y1))-((a.y2 - a.y1)*(a.x1 - b.x1))) / Uden;
  
  testx = a.x1 + (Ua * (a.x2 - a.x1));
  testy = a.y1 + (Ua * (a.y2 - a.y1));
  
  if(contact != NULL)
  {
    contact->x = testx;
    contact->y = testy;
  }
  
  if((Ua >= 0) && (Ua <= 1) && (Ub >= 0) && ( Ub <= 1))
  {
    return 1;
  }
  return 0;  
}

Edge gf2d_edge(float x1, float y1, float x2, float y2)
{
    Edge e;
    gf2d_edge_set(e,x1,y1,x2,y2);
    return e;
}

Edge gf2d_edge_from_vectors(Vector2D a,Vector2D b)
{
    Edge e;
    gf2d_edge_set(e,a.x,a.y,b.x,b.y);
    return e;
}

Uint8 gf2d_edge_rect_intersection(Edge e, Rect r)
{
    if ((gf2d_point_in_rect(vector2d(e.x1,e.y1),r))||
        (gf2d_point_in_rect(vector2d(e.x2,e.y2),r)))
    {
        // if either end point is within the rect, we have a collision
        return 1;
    }
    if ((gf2d_edge_intersect(e,gf2d_edge(r.x,r.y,r.x+r.w,r.y),NULL))||//top
        (gf2d_edge_intersect(e,gf2d_edge(r.x,r.y,r.x,r.y+r.h),NULL))||//left
        (gf2d_edge_intersect(e,gf2d_edge(r.x,r.y+r.h,r.x+r.w,r.y+r.h),NULL))||//bottom
        (gf2d_edge_intersect(e,gf2d_edge(r.x+r.w,r.y,r.x+r.w,r.y+r.h),NULL)))//right
        return 1;
    return 0;
}

Uint8 gf2d_edge_circle_intersection(Edge e,Circle c)
{
    float A1 = (e.y2 - e.y1);
    float B1 = (e.x1 - e.x2);
    float C1 = (e.y2 - e.y1) * e.x1
                    + (e.x1 - e.x2) * e.y1;
    float C3 = -B1 * c.x + A1 * c.y;
    float det2 = (A1 * A1 - -B1 * B1);
    float cx2 = 0;
    float cy2 = 0;
    if (det2 != 0)
    {
        cx2 = (A1 * C1 - B1 * C3) / det2;
        cy2 = (A1 * C3 - -B1 * C1) / det2;
    }
    if (MIN(e.x1, e.x2) <= cx2
        && cx2 <= MAX(e.x1, e.x2)
        && MIN(e.y1, e.y2) <= cy2
        && cy2 <= MAX(e.y1, e.y2))
    {
        if (fabs((cx2 - c.x) * (cx2 - c.x) + (cy2 - c.y) * (cy2 - c.y)) < c.r * c.r + 1)
        {
            return 1;
        }
    }
    return 0;
}
/*eol@eof*/
