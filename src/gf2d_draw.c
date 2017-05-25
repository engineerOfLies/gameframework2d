#include "gf2d_draw.h"
#include "gf2d_graphics.h"

void gf2d_draw_lines(Vector2D *p1,Vector2D *p2, Uint32 lines,Vector4D color)
{
    int i;
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
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

void gf2d_draw_line(Vector2D p1,Vector2D p2, Vector4D color)
{
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
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

void gf2d_draw_rect(SDL_Rect rect,Vector4D color)
{
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
    SDL_RenderDrawRect(gf2d_graphics_get_renderer(),(const struct SDL_Rect *)&rect);
}

void gf2d_draw_rects(SDL_Rect *rects,Uint32 count,Vector4D color)
{
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
    SDL_RenderDrawRects(gf2d_graphics_get_renderer(),rects,count);
}

void gf2d_draw_pixel(Vector2D pixel,Vector4D color)
{
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
    SDL_RenderDrawPoint(gf2d_graphics_get_renderer(),
                        pixel.x,
                        pixel.y);
}

void gf2d_draw_pixel_list(SDL_Point * pixels,Uint32 count,Vector4D color)
{
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
    SDL_RenderDrawPoints(gf2d_graphics_get_renderer(),
                        pixels,
                        count);
}


/*eol@eof*/
