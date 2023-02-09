#include "simple_logger.h"

#include "gfc_shape.h"

#include "gf2d_graphics.h"

#include "camera.h"

static Camera camera = {0};

void camera_set_world_size(Vector2D size)
{
    camera.size = size;
}

void camera_center_at(Vector2D position)
{
    Vector2D cam = {0};
    Vector2D res;
    res = gf2d_graphics_get_resolution();
    vector2d_scale(res,res,0.5);
    vector2d_sub(cam,position,res);
    camera_set_position(cam);
}

void camera_world_snap()
{
    Vector2D res;
    res = gf2d_graphics_get_resolution();
    if (camera.position.x < 0)camera.position.x = 0;
    if (camera.position.y < 0)camera.position.y = 0;
    if (camera.position.x + res.x > camera.size.x)camera.position.x = camera.size.x - res.x;
    if (camera.position.y + res.y > camera.size.y)camera.position.y = camera.size.y - res.y;
}

void camera_set_position(Vector2D position)
{
    camera.position = position;
}

Vector2D camera_get_position()
{
    return camera.position;
}

Vector2D camera_get_draw_offset()
{
    Vector2D offset = {0};
    vector2d_negate(offset,camera.position);
    return offset;
}
