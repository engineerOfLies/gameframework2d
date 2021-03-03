#include "gf2d_shape.h"
#include "camera.h"



static SDL_Rect Camera = {0};

void camera_set_dimensions(Vector2D size)
{
    Camera.w = size.x;
    Camera.h = size.y;
}

Vector2D camera_get_dimensions()
{
    Vector2D size;
    size.x = Camera.w;
    size.y = Camera.h;
    return size;
}


void camera_set_position(Vector2D position)
{
    Camera.x = (int)position.x;
    Camera.y = (int)position.y;
}

void camera_move(Vector2D move)
{
    Camera.x += move.x;
    Camera.y += move.y;
}

Bool camera_point_on_screen(Vector2D point)
{
    return gf2d_point_in_rect(point, gf2d_rect_from_sdl_rect(Camera));
}

Bool camera_rect_on_screen(SDL_Rect rect)
{
    return gf2d_rect_overlap(gf2d_rect_from_sdl_rect(rect), gf2d_rect_from_sdl_rect(Camera));
}

SDL_Rect camera_get_rect()
{
    return Camera;
}

Vector2D camera_get_position()
{
    Vector2D position;
    position.x = Camera.x;
    position.y = Camera.y;
    return position;
}


Vector2D camera_get_offset()
{
    Vector2D position;
    position.x = -Camera.x;
    position.y = -Camera.y;
    return position;
}


/*eol@eof*/
