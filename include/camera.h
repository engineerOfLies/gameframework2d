#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "gfc_vector.h"

typedef struct
{
    Vector2D position;  //screen offset
    Vector2D size;      // world size
}Camera;

void camera_set_position(Vector2D position);

Vector2D camera_get_position();

void camera_set_world_size(Vector2D size);

Vector2D camera_get_draw_offset();// used to draw relative to camera position

void camera_world_snap();

void camera_center_at(Vector2D position);

#endif
