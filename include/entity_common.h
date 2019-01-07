#ifndef __ENTITY_COMMON_H__
#define __ENTITY_COMMON_H__

int entity_camera_view(Entity *self);
int entity_roof_check(Entity *self, float width);
int entity_ground_check(Entity *self, float width);
int entity_left_check(Entity *self, float width);
int entity_right_check(Entity *self, float width);
void entity_world_snap(Entity *self);

#endif
