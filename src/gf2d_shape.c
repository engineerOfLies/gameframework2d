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


Uint8 gf2d_rect_overlap_poc(Rect a,Rect b,Vector2D *poc, Vector2D *normal)
{
    if ((a.x > b.x+b.w)||
        (b.x > a.x+a.w)||
        (a.y > b.y+b.h)||
        (b.y > a.y+a.h))
    {
        return 0;
    }
    if (poc)
    {
        poc->y = poc->x = 0;
        if (normal)normal->x = normal->y = 0;
        if (a.x + 1 >= b.x + b.w)
        {
            poc->x = a.x;
            if (normal)normal->x = -1;
        }
        else if (b.x + 1 >= a.x + a.w)
        {
            poc->x = b.x;
            if (normal)normal->x = 1;
        }
        if (a.y + 1 >= b.y + b.h)
        {
            poc->y = a.y;
            if (normal)normal->y = -1;
        }
        if (b.y + 1 >= a.y + a.h)
        {
            if (normal)normal->y = 1;
            poc->y = b.y;
        }
    }
    return 1;
}

Uint8 gf2d_rect_overlap(Rect a,Rect b)
{
    return gf2d_rect_overlap_poc(a,b,NULL,NULL);
}

Uint8 gf2d_point_in_cicle(Vector2D p,Circle c)
{
    if (vector2d_magnitude_compare(vector2d(c.x-p.x,c.y-p.y),c.r) <= 0)return 1;
    return 0;
}

Uint8 gf2d_circle_overlap_poc(Circle a, Circle b,Vector2D *poc,Vector2D *normal)
{
    Vector2D v;
    vector2d_set(v,a.x - b.x,a.y - b.y);
    if (vector2d_magnitude_compare(v,a.r+b.r) <= 0)
    {
        if (poc)
        {
            vector2d_normalize(&v);
            if (normal)
            {
                normal->x = v.x;
                normal->y = v.y;
            }
            vector2d_scale(v,v,a.r);
            poc->x = a.x + v.x;
            poc->y = a.y + v.y;
            
        }
        return 1;
    }
    return 0;
}

Uint8 gf2d_circle_overlap(Circle a, Circle b)
{
    return gf2d_circle_overlap_poc(a, b,NULL,NULL);
}

Uint8 gf2d_circle_rect_overlap_poc(Circle a, Rect b,Vector2D *poc,Vector2D * normal)
{
    Rect newrect1,newrect2;
    gf2d_rect_set(newrect1,b.x - a.r,b.y,b.w + a.r+ a.r,b.h);
    gf2d_rect_set(newrect2,b.x,b.y - a.r,b.w,b.h + a.r + a.r);
    if (gf2d_point_in_cicle(vector2d(b.x,b.y),a))
    {
        if (poc)
        {
            poc->x = b.x;
            poc->y = b.y;
        }
        if (normal)
        {
            normal->x = a.x-b.x;
            normal->y = a.y-b.y;
            vector2d_normalize(normal);
        }
        return 1;
    }
    if (gf2d_point_in_cicle(vector2d(b.x+b.w,b.y),a))
    {
        if (poc)
        {
            poc->x = b.x+b.w;
            poc->y = b.y;
        }
        if (normal)
        {
            normal->x = a.x-(b.x+b.w);
            normal->y = a.y-b.y;
            vector2d_normalize(normal);
        }
        return 1;
    }
    if (gf2d_point_in_cicle(vector2d(b.x,b.y+b.h),a))
    {
        if (poc)
        {
            poc->x = b.x;
            poc->y = b.y+b.h;
        }
        if (normal)
        {
            normal->x = a.x-b.x;
            normal->y = a.y-(b.y + b.h);
            vector2d_normalize(normal);
        }
        return 1;
    }
    if (gf2d_point_in_cicle(vector2d(b.x+b.w,b.y+b.h),a))
    {
        if (poc)
        {
            poc->x = b.x+b.w;
            poc->y = b.y+b.h;
        }
        if (normal)
        {
            normal->x = a.x-(b.x+b.w);
            normal->y = a.y-(b.y + b.h);
            vector2d_normalize(normal);
        }
        return 1;
    }

    if ((gf2d_point_in_rect(vector2d(a.x,a.y),newrect1))||
        (gf2d_point_in_rect(vector2d(a.x,a.y),newrect2)))
    {
        if (poc)
        {
            if (a.x < b.x)
            {
                poc->y = a.y;
                poc->x = b.x;
                if (normal)
                {
                    normal->x = -1;
                    normal->y = 0;
                }
            }
            else if (a.x > b.x + b.w)
            {
                poc->y = a.y;
                poc->x = b.x + b.w;
                if (normal)
                {
                    normal->x = 1;
                    normal->y = 0;
                }
            }
            if (a.y < b.y)
            {
                poc->y = b.y;
                poc->x = a.x;
                if (normal)
                {
                    normal->x = 0;
                    normal->y = -1;
                }
            }
            else if (a.y > b.y + b.y)
            {
                poc->y = b.y + b.h;
                poc->x = a.x;
                if (normal)
                {
                    normal->x = 0;
                    normal->y = 1;
                }
            }
        }
        return 1;
    }
    return 0;
}

Uint8 gf2d_circle_rect_overlap(Circle a, Rect b)
{
    Rect newrect;
    gf2d_rect_set(newrect,b.x - a.r,b.y,b.w + a.r+ a.r,b.h);
    if (gf2d_point_in_rect(vector2d(a.x,a.y),newrect))return 1;
    gf2d_rect_set(newrect,b.x,b.y - a.r,b.w,b.h + a.r + a.r);
    if (gf2d_point_in_rect(vector2d(a.x,a.y),newrect))return 1;
    
    if (gf2d_point_in_cicle(vector2d(b.x,b.y),a))return 1;
    if (gf2d_point_in_cicle(vector2d(b.x+b.w,b.y),a))return 1;
    if (gf2d_point_in_cicle(vector2d(b.x,b.y+b.h),a))return 1;
    if (gf2d_point_in_cicle(vector2d(b.x+b.w,b.y+b.h),a))return 1;
    return 0;
}

Uint8 gf2d_shape_overlap_poc(Shape a, Shape b, Vector2D *poc, Vector2D *normal)
{
    switch(a.type)
    {
        case ST_CIRCLE:
            switch(b.type)
            {
                case ST_CIRCLE:
                    return gf2d_circle_overlap_poc(a.s.c,b.s.c,poc,normal);
                case ST_RECT:
                    return gf2d_circle_rect_overlap_poc(a.s.c,b.s.r,poc,normal);
                case ST_EDGE:
                    return gf2d_edge_circle_intersection_poc(b.s.e,a.s.c,poc,normal);
            }
        case ST_RECT:
            switch (b.type)
            {
                case ST_RECT:
                    return gf2d_rect_overlap_poc(a.s.r,b.s.r,poc,normal);
                case ST_CIRCLE:
                    return gf2d_circle_rect_overlap_poc(b.s.c,a.s.r,poc,normal);
                case ST_EDGE:
                    return gf2d_edge_rect_intersection_poc(b.s.e, a.s.r,poc,normal);
            }
        case ST_EDGE:
            switch (b.type)
            {
                case ST_EDGE:
                    return gf2d_edge_intersect_poc(a.s.e,b.s.e,poc,normal);
                case ST_CIRCLE:
                    return gf2d_edge_circle_intersection_poc(a.s.e,b.s.c,poc,normal);
                case ST_RECT:
                    return gf2d_edge_rect_intersection_poc(a.s.e, b.s.r,poc,normal);
            }
    }
    return 0;
}

Uint8 gf2d_shape_overlap(Shape a, Shape b)
{
    return gf2d_shape_overlap_poc(a,b,NULL,NULL);
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

Shape gf2d_shape_circle(float x, float y, float r)
{
    Shape shape;
    shape.type = ST_CIRCLE;
    shape.s.c.x = x; 
    shape.s.c.y = y;
    shape.s.c.r = r;
    return shape;
}

Shape gf2d_shape_from_circle(Circle c)
{
    Shape shape;
    shape.type = ST_CIRCLE;
    shape.s.c.x = c.x;
    shape.s.c.y = c.y;
    shape.s.c.r = c.r;
    return shape;
}

Shape gf2d_shape_edge(float x1,float y1,float x2,float y2)
{
    return gf2d_shape_from_edge(gf2d_edge(x1,y1,x2,y2));
}

Shape gf2d_shape_from_edge(Edge e)
{
    Shape shape;
    shape.type = ST_EDGE;
    gf2d_edge_copy(shape.s.e,e);
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

Uint8 gf2d_edge_intersect_poc(
    Edge a,
    Edge b,
    Vector2D *contact,
    Vector2D *normal
)
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
    if (normal != NULL)
    {
        normal->x = b.y2 - b.y1;
        normal->y = b.x2 - b.x1;
        vector2d_normalize(normal);
    }
  
    if((Ua >= 0) && (Ua <= 1) && (Ub >= 0) && ( Ub <= 1))
    {
        return 1;
    }
    return 0;  
}

Uint8 gf2d_edge_intersect(Edge a,Edge b)
{
    return gf2d_edge_intersect_poc(a,b,NULL,NULL);
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

Uint8 gf2d_edge_rect_intersection_poc(Edge e, Rect r,Vector2D *poc,Vector2D *normal)
{
    Uint8 ret = 0;
    if (gf2d_edge_intersect_poc(e,gf2d_edge(r.x,r.y,r.x+r.w,r.y),poc,NULL))//top
    {
        ret = 1;
    }
    if (gf2d_edge_intersect_poc(e,gf2d_edge(r.x,r.y,r.x,r.y+r.h),poc,NULL))//left
    {
        ret |= 2;
    }
    if (gf2d_edge_intersect_poc(e,gf2d_edge(r.x,r.y+r.h,r.x+r.w,r.y+r.h),poc,NULL))//bottom
    {
        ret |= 4;
    }
    if (gf2d_edge_intersect_poc(e,gf2d_edge(r.x+r.w,r.y,r.x+r.w,r.y+r.h),poc,NULL))//right
    {
        ret |= 8;
    }
    if (ret)
    {
        if (normal)
        {
            if ((ret & 5)||(ret == 8))// top & bottom or right
            {
                normal->x = 1;
                normal->y = 0;
            }
            else if ((ret & 10)||(ret == 1))// left & right or top
            {
                normal->x = 0;
                normal->y = -1;
            }
            else if (ret & 2)// left
            {
                normal->x = -1;
                normal->y = 0;
            }
            else
            {
                normal->x = 0;
                normal->y = 1;
            }
        }   
        return 1;
    }
    if ((gf2d_point_in_rect(vector2d(e.x1,e.y1),r))||
        (gf2d_point_in_rect(vector2d(e.x2,e.y2),r)))
    {
        // if either end point is within the rect, we have a collision
        return 1;
    }
    return 0;
}

Uint8 gf2d_edge_rect_intersection(Edge e, Rect r)
{
    return gf2d_edge_rect_intersection_poc(e,r,NULL,NULL);
}

Uint8 gf2d_edge_intersect_shape(Edge e,Shape s)
{
    return gf2d_shape_overlap(gf2d_shape_from_edge(e), s);
}

Uint8 gf2d_edge_intersect_shape_poc(Edge e,Shape s,Vector2D *poc,Vector2D *normal)
{
    return gf2d_shape_overlap_poc(gf2d_shape_from_edge(e), s,poc,normal);
}

Uint8 gf2d_edge_circle_intersection_poc(Edge e,Circle c,Vector2D *poc,Vector2D *normal)
{
    float dy = (e.y2 - e.y1);
    float dx = (e.x1 - e.x2);
    float C1 = (e.y2 - e.y1) * e.x1 + (e.x1 - e.x2) * e.y1;
    
    float C3 = -dx * c.x + dy * c.y;
    float det = (dy * dy - -dx * dx);
    float cx2 = 0;
    float cy2 = 0;
    if (det != 0)
    {
        cx2 = (dy * C1 - dx * C3) / det;
        cy2 = (dy * C3 - -dx * C1) / det;
    }
    if (MIN(e.x1, e.x2) <= cx2
        && cx2 <= MAX(e.x1, e.x2)
        && MIN(e.y1, e.y2) <= cy2
        && cy2 <= MAX(e.y1, e.y2))
    {
        if (fabs((cx2 - c.x) * (cx2 - c.x) + (cy2 - c.y) * (cy2 - c.y)) < c.r * c.r + 1)
        {
            if (poc)
            {
                poc->x = cx2;
                poc->y = cy2;
                if (normal)
                {
                    normal->x = poc->x - c.x;
                    normal->y = poc->y - c.y;
                    vector2d_normalize(normal);
                }
            }
            return 1;
        }
    }
    return 0;
}

Uint8 gf2d_edge_circle_intersection(Edge e,Circle c)
{
    return gf2d_edge_circle_intersection_poc(e,c,NULL,NULL);
}

void gf2d_edge_slog(Edge e)
{
    slog("Edge: (%f,%f),(%f,%f)",e.x1,e.y1,e.x2,e.y2);
}

void gf2d_rect_slog(Rect r)
{
    slog("Rect: (%f,%f,%f,%f)",r.x,r.y,r.w,r.h);
}

void gf2d_circle_slog(Circle c)
{
    slog("Circle: (%f,%f) radius (%f)",c.x,c.y,c.r);
}

void gf2d_shape_slog(Shape shape)
{
    switch(shape.type)
    {
        case ST_EDGE:
            gf2d_edge_slog(shape.s.e);
            break;
        case ST_RECT:
            gf2d_rect_slog(shape.s.r);
            break;
        case ST_CIRCLE:
            gf2d_circle_slog(shape.s.c);
            break;
    }
}

Rect gf2d_edge_get_bounds(Edge e)
{
    Rect r;
    r.x = MIN(e.x1,e.x2);
    r.y = MIN(e.y1,e.y2);
    r.w = fabs(e.x1 - e.x2);
    r.h = fabs(e.y1 - e.y2);
    return r;
}

Rect gf2d_circle_get_bounds(Circle c)
{
    Rect r;
    r.x = c.x-c.r;
    r.y = c.y-c.r;
    r.w = c.r*2;
    r.h = c.r*2;
    return r;
}

Rect gf2d_shape_get_bounds(Shape shape)
{
    Rect r = {0,0,0,0};
    switch(shape.type)
    {
        case ST_EDGE:
            gf2d_edge_get_bounds(shape.s.e);
            break;
        case ST_RECT:
            return shape.s.r;
            break;
        case ST_CIRCLE:
            gf2d_circle_get_bounds(shape.s.c);
            break;
    }
    return r;
}
/*eol@eof*/
