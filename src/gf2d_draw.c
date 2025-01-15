#include "simple_logger.h"

#include "gfc_list.h"

#include "gf2d_draw.h"
#include "gf2d_graphics.h"

void gf2d_draw_shape(GFC_Shape shape,GFC_Color color,GFC_Vector2D offset)
{
    switch(shape.type)
    {
        case ST_RECT:
            gfc_vector2d_add(shape.s.r,shape.s.r,offset);
            gf2d_draw_rect(shape.s.r,color);
            break;
        case ST_CIRCLE:
            gf2d_draw_circle(gfc_vector2d(shape.s.c.x + offset.x,shape.s.c.y + offset.y), shape.s.c.r,color);
            break;
        case ST_EDGE:
            gf2d_draw_line(gfc_vector2d(shape.s.e.x1 + offset.x,shape.s.e.y1 + offset.y),gfc_vector2d(shape.s.e.x2 + offset.x,shape.s.e.y2 + offset.y),color);
            break;
    }
}

GFC_List *gf2d_draw_get_bezier_points(GFC_Vector2D p0, GFC_Vector2D p1, GFC_Vector2D p2)
{
    GFC_List *points;
    GFC_Vector2D *point;
    GFC_Vector2D qp,qp2,qpv; /*approximation line starting point and vector*/
    GFC_Vector2D p0v,p1v,temp; /*vectors from point to next point*/
    GFC_Vector2D dp; /*draw point*/
    float t = 0;  /*time segment*/
    float tstep;
    float totalLength;
    gfc_vector2d_sub(p0v,p1,p0);
    gfc_vector2d_sub(p1v,p2,p1);
    totalLength = gfc_vector2d_magnitude(p0v)+gfc_vector2d_magnitude(p1v);
    if (totalLength == 0)
    {
        return NULL;
    }
    tstep = fabs(1.0/(totalLength * 0.9));
    gfc_vector2d_sub(p0v,p1,p0);
    gfc_vector2d_sub(p1v,p2,p1);
    points = gfc_list_new();
    for (t = 0; t <= 1;t += tstep)
    {
        /*calculate Q*/
        gfc_vector2d_scale(temp,p0v,t);
        gfc_vector2d_add(qp,p0,temp);
        
        gfc_vector2d_scale(temp,p1v,t);
        gfc_vector2d_add(qp2,p1,temp);
        
        gfc_vector2d_sub(qpv,qp2,qp);
        
        gfc_vector2d_scale(temp,qpv,t);
        gfc_vector2d_add(dp,qp,temp);
        point = gfc_allocate_array(sizeof(GFC_Vector2D),1);
        if (!point)continue;
        gfc_vector2d_copy((*point),dp);
        gfc_list_append(points,point);
    }
    return points;
}

SDL_Point *gf2d_draw_point_list_to_array(GFC_List *points)
{
    int i,c;
    GFC_Vector2D *point;
    SDL_Point *array;
    if (!points)return NULL;
    c = gfc_list_get_count(points);
    if (!c)return NULL;
    array = gfc_allocate_array(sizeof(SDL_Point),c);
    for (i = 0; i < c; i++)
    {
        point = gfc_list_get_nth(points,i);
        gfc_vector2d_copy(array[i],(*point));
    }
    return array;
}

void gf2d_draw_polygon(GFC_List *points,GFC_Color color)
{
    int c;
    GFC_Color drawColor;
    SDL_Point *array;
    c = gfc_list_get_count(points);
    if (c < 2)return;

    array = gf2d_draw_point_list_to_array(points);
    if (!array)return;
    drawColor = gfc_color_to_int8(color);
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           drawColor.r,
                           drawColor.g,
                           drawColor.b,
                           drawColor.a);
    SDL_RenderDrawLines(gf2d_graphics_get_renderer(),
                        array,
                        c);
    SDL_RenderDrawLine(gf2d_graphics_get_renderer(),
                    array[0].x,
                    array[0].y,
                    array[c-1].x,
                    array[c-1].y);

    free(array);
}

void gf2d_draw_free_points(void *data)
{
    GFC_Vector2D *vec;
    if (data)
    {
        vec = data;
        free(vec);
    }
}

void gf2d_draw_point_list(GFC_List *points,GFC_Color color)
{
    int c;
    SDL_Point *array;
    GFC_Color drawColor;
    if (!points)return;
    drawColor = gfc_color_to_int8(color);
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           drawColor.r,
                           drawColor.g,
                           drawColor.b,
                           drawColor.a);
    array = gf2d_draw_point_list_to_array(points);
    if (!array)return;
    c = gfc_list_get_count(points);
    SDL_RenderDrawPoints(gf2d_graphics_get_renderer(),
                        array,
                        c);
    free(array);
}

void gf2d_draw_bezier_curve(GFC_Vector2D p0, GFC_Vector2D p1, GFC_Vector2D p2,GFC_Color color)
{
    GFC_List *points;
    points = gf2d_draw_get_bezier_points(p0, p1, p2);
    if (!points)return;
    gf2d_draw_point_list(points,color);
    gfc_list_foreach(points,gf2d_draw_free_points);
    gfc_list_delete(points);
}

void gf2d_draw_bezier4_curve(GFC_Vector2D p0,GFC_Vector2D r0,GFC_Vector2D r1,GFC_Vector2D p1,GFC_Color color)
{
    GFC_Vector2D *points;
    int length;
    int index = 0;
    float t,step;
    GFC_Color drawColor;
    //source: https://programmerbay.com/c-program-to-draw-bezier-curve-using-4-control-points/
    drawColor = gfc_color_to_int8(color);
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           drawColor.r,
                           drawColor.g,
                           drawColor.b,
                           drawColor.a);
    length = gfc_vector2d_magnitude(gfc_vector2d(p1.x - r1.x,p1.y - r1.y))
            +gfc_vector2d_magnitude(gfc_vector2d(r1.x - r0.x,r1.y - r0.y))
            +gfc_vector2d_magnitude(gfc_vector2d(p0.x - r0.x,p0.y - r0.y));
    if (length == 0)return;
    points = gfc_allocate_array(sizeof(GFC_Vector2D),length);
    if (!points)return;
    step = 1/(float)length;
    for (t= 0,index = 0; index < length;t += step,index++)
    {
        points[index].x = pow(1-t,3)*p0.x + 3*t*pow(1-t,2)*r0.x + 3*t*t*(1-t)*r1.x + pow(t,3)*p1.x;
        points[index].y = pow(1-t,3)*p0.y + 3*t*pow(1-t,2)*r0.y + 3*t*t*(1-t)*r1.y + pow(t,3)*p1.y;
    }
    SDL_RenderDrawPointsF(gf2d_graphics_get_renderer(),
                          (SDL_FPoint*)points,
                          length);
    free(points);
}


void gf2d_draw_lines(GFC_Vector2D *p1,GFC_Vector2D *p2, Uint32 lines,GFC_Color color);

void gf2d_draw_diamond(GFC_Vector2D center, int radius, GFC_Color color)
{
    int i;
    GFC_Vector2D p1[4] = {0},p2[4] = {0};
    p1[0].y -= radius;
    p2[0].x += radius;

    p1[1].x += radius;
    p2[1].y += radius;
    
    p1[2].y += radius;
    p2[2].x -= radius;
    
    p1[3].x -= radius;
    p2[3].y -= radius;
    for (i = 0;i < 4;i++)
    {
        gfc_vector2d_add(p1[i],p1[i],center);
        gfc_vector2d_add(p2[i],p2[i],center);
    }
    gf2d_draw_lines(p1,p2, 4,color);
}

void gf2d_draw_lines(GFC_Vector2D *p1,GFC_Vector2D *p2, Uint32 lines,GFC_Color color)
{
    int i;
    GFC_Color drawColor;
    drawColor = gfc_color_to_int8(color);
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           drawColor.r,
                           drawColor.g,
                           drawColor.b,
                           drawColor.a);
    for (i = 0; i < lines;i++)
    {
        SDL_RenderDrawLine(gf2d_graphics_get_renderer(),
                           p1[i].x,
                           p1[i].y,
                           p2[i].x,
                           p2[i].y);
    }
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           255,
                           255,
                           255,
                           255);
}

void gf2d_draw_line(GFC_Vector2D p1,GFC_Vector2D p2, GFC_Color color)
{
    GFC_Color drawColor;
    drawColor = gfc_color_to_int8(color);
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           drawColor.r,
                           drawColor.g,
                           drawColor.b,
                           drawColor.a);
    SDL_RenderDrawLine(gf2d_graphics_get_renderer(),
                       p1.x,
                       p1.y,
                       p2.x,
                       p2.y);
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           255,
                           255,
                           255,
                           255);
}

void gf2d_draw_rect(GFC_Rect rect,GFC_Color color)
{
    GFC_Color drawColor;
    SDL_Rect drawrect;
    drawrect = gfc_rect_to_sdl_rect(rect);
    drawColor = gfc_color_to_int8(color);
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           drawColor.r,
                           drawColor.g,
                           drawColor.b,
                           drawColor.a);
    SDL_RenderDrawRect(gf2d_graphics_get_renderer(),(const struct SDL_Rect *)&drawrect);
}

void gf2d_draw_rect_filled(GFC_Rect rect,GFC_Color color)
{
    GFC_Color drawColor;
    SDL_Rect drawrect;
    drawrect = gfc_rect_to_sdl_rect(rect);
    drawColor = gfc_color_to_int8(color);
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           drawColor.r,
                           drawColor.g,
                           drawColor.b,
                           drawColor.a);
    SDL_RenderFillRect(gf2d_graphics_get_renderer(),(const struct SDL_Rect *)&drawrect);
}

void gf2d_draw_rects(SDL_Rect *rects,Uint32 count,GFC_Color color)
{
    GFC_Color drawColor;
    drawColor = gfc_color_to_int8(color);
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           drawColor.r,
                           drawColor.g,
                           drawColor.b,
                           drawColor.a);
    SDL_RenderDrawRects(gf2d_graphics_get_renderer(),rects,count);
}

void gf2d_draw_pixel(GFC_Vector2D pixel,GFC_Color color)
{
    GFC_Color drawColor;
    drawColor = gfc_color_to_int8(color);
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           drawColor.r,
                           drawColor.g,
                           drawColor.b,
                           drawColor.a);
    SDL_RenderDrawPoint(gf2d_graphics_get_renderer(),
                        pixel.x,
                        pixel.y);
}

void gf2d_draw_pixel_list(SDL_Point * pixels,Uint32 count,GFC_Color color)
{
    GFC_Color drawColor;
    drawColor = gfc_color_to_int8(color);
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           drawColor.r,
                           drawColor.g,
                           drawColor.b,
                           drawColor.a);
    SDL_RenderDrawPoints(gf2d_graphics_get_renderer(),
                        pixels,
                        count);
}

/*
 * credit circle drawing algorith:
 * http://groups.csail.mit.edu/graphics/classes/6.837/F98/Lecture6/circle.html
 */

static int gf2d_draw_circle_points(SDL_Point *p,GFC_Vector2D center, GFC_Vector2D point)
{  
  if (point.x == 0)
  {
    gfc_vector2d_set(p[0],center.x, center.y + point.y);
    gfc_vector2d_set(p[1],center.x, center.y - point.y);
    gfc_vector2d_set(p[2],center.x + point.y, center.y);
    gfc_vector2d_set(p[3],center.x - point.y, center.y);
    return 4;
  }
  if (point.x == point.y)
  {
    gfc_vector2d_set(p[0],center.x + point.x, center.y + point.y);
    gfc_vector2d_set(p[1],center.x - point.x, center.y + point.y);
    gfc_vector2d_set(p[2],center.x + point.x, center.y - point.y);
    gfc_vector2d_set(p[3],center.x - point.x, center.y - point.y);
    return 4;
  }
  if (point.x < point.y)
  {
    gfc_vector2d_set(p[0],center.x + point.x, center.y + point.y);
    gfc_vector2d_set(p[1],center.x - point.x, center.y + point.y);
    gfc_vector2d_set(p[2],center.x + point.x, center.y - point.y);
    gfc_vector2d_set(p[3],center.x - point.x, center.y - point.y);
    gfc_vector2d_set(p[4],center.x + point.y, center.y + point.x);
    gfc_vector2d_set(p[5],center.x - point.y, center.y + point.x);
    gfc_vector2d_set(p[6],center.x + point.y, center.y - point.x);
    gfc_vector2d_set(p[7],center.x - point.y, center.y - point.x);
    return 8;
  }
  return 0;
}

void gf2d_draw_circle(GFC_Vector2D center, int radius, GFC_Color color)
{
    SDL_Point *pointArray;
    int i = 0;
    GFC_Vector2D point = {0,0};
    int p = (5 - radius*4)/4;
    GFC_Color drawColor;
    drawColor = gfc_color_to_int8(color);
    point.y = radius;
    pointArray = (SDL_Point*)malloc(sizeof(SDL_Point)*radius*8);
    if (!pointArray)
    {
        slog("gf2d_draw_circle: failed to allocate points for circle drawing");
        return;
    }
    i = gf2d_draw_circle_points(&pointArray[i],center, point);
    while (point.x < point.y)
    {
        point.x++;
        if (p < 0)
        {
            p += 2*point.x+1;
        }
        else
        {
            point.y--;
            p += 2*(point.x-point.y)+1;
        }
        i += gf2d_draw_circle_points(&pointArray[i],center, point);
        if (i + 8 >= radius*8)
        {
            break;
        }
    }
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           drawColor.r,
                           drawColor.g,
                           drawColor.b,
                           drawColor.a);
    SDL_RenderDrawPoints(gf2d_graphics_get_renderer(),pointArray,i);
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                            255,
                            255,
                            255,
                            255);
    free(pointArray);
}

GFC_List *gf2d_draw_get_bezier4_points(
    GFC_Vector2D ep1,
    GFC_Vector2D rp1,
    GFC_Vector2D rp2,
    GFC_Vector2D ep2)
{
/*    List *points;
    GFC_Vector2D *point;
    GFC_Vector2D qp,qp2,qpv; //approximation line starting point and vector
    GFC_Vector2D p0v,p1v,p2v,temp; //vectors from point to next point
    GFC_Vector2D dp; //draw point
    float t = 0;  //time segment
    float tstep;
    float totalLength;
    gfc_vector2d_sub(p0v,rp1,ep1);
    gfc_vector2d_sub(p1v,rp2,rp1);
    gfc_vector2d_sub(p2v,ep2,rp2);//segments between each
    totalLength = gfc_vector2d_magnitude(p0v)+gfc_vector2d_magnitude(p1v)+gfc_vector2d_magnitude(p2v);
    if (totalLength == 0)
    {
        return NULL;
    }
    tstep = fabs(1.0/(totalLength * 0.9));
    points = gfc_list_new();
    for (t = 0; t <= 1;t += tstep)
    {
        gfc_vector2d_scale(temp,p0v,t);
        gfc_vector2d_add(qp,ep0,temp);
        
        gfc_vector2d_scale(temp,p1v,t);
        gfc_vector2d_add(qp2,p1,temp);
        
        gfc_vector2d_sub(qpv,qp2,qp);
        
        gfc_vector2d_scale(temp,qpv,t);
        gfc_vector2d_add(dp,qp,temp);
        point = gfc_allocate_array(sizeof(GFC_Vector2D),1);
        if (!point)continue;
        gfc_vector2d_copy((*point),dp);
        points = gfc_list_append(points,point);
    }
    return points;
        */return NULL;
}

void gf2d_bezier4_draw(
    GFC_Vector2D ep1,
    GFC_Vector2D rp1,
    GFC_Vector2D rp2,
    GFC_Vector2D ep2,
    GFC_Color color)
{
    GFC_List *points;
    points = gf2d_draw_get_bezier4_points(ep1,rp1,rp2,ep2);
    if (!points)return;
    gf2d_draw_point_list(points,color);
    gfc_list_foreach(points,gf2d_draw_free_points);
    gfc_list_delete(points);
}

/*eol@eof*/
