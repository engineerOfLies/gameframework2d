#include "camera.h"
#include "simple_logger.h"

typedef struct
{
    Rect view;
    Rect bounds;
}Camera;
static Camera _camera = {0};

void camera_set_dimensions(Sint32 x,Sint32 y,Uint32 w,Uint32 h)
{
    gf2d_rect_set(_camera.view,x,y,w,h);
}

Rect camera_get_bounds()
{
    return _camera.bounds;
}

Rect camera_get_dimensions()
{
    return _camera.view;
}

Vector2D camera_get_position()
{
    return vector2d(_camera.view.x,_camera.view.y);
}

void camera_set_bounds(Sint32 x,Sint32 y,Uint32 w,Uint32 h)
{
    gf2d_rect_set(_camera.bounds,x,y,w,h);
}

void camera_bind()
{
    if (_camera.view.x < _camera.bounds.x)_camera.view.x = _camera.bounds.x;
    if (_camera.view.x + _camera.view.w > _camera.bounds.x + _camera.bounds.w)_camera.view.x = _camera.bounds.x + _camera.bounds.w - _camera.view.w;
    if (_camera.view.y < _camera.bounds.y)_camera.view.y = _camera.bounds.y;
    if (_camera.view.y + _camera.view.h > _camera.bounds.y + _camera.bounds.h)_camera.view.y = _camera.bounds.y + _camera.bounds.h - _camera.view.h;
}

void camera_move(Vector2D v)
{
    vector2d_add(_camera.view,v,_camera.view);
    camera_bind();
}

void camera_set_position(Vector2D position)
{
    vector2d_copy(_camera.view,position);
    camera_bind();
}

void camera_set_position_absolute(Vector2D position)
{
    vector2d_copy(_camera.view,position);
}

/*eol@eof*/
