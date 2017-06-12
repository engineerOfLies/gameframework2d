#include "gf2d_entity.h"
#include "camera.h"
#include "entity_common.h"

int entity_camera_view(Entity *self)
{
    Rect r,c;
    if (!self)return 0;
    c = camera_get_dimensions();
    r = gf2d_shape_get_bounds(self->shape);
    vector2d_add(r,r,self->position);
    return gf2d_rect_overlap(r,c);
}

/*eol@eof*/
