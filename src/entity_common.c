#include "gf2d_entity.h"
#include "camera.h"
#include "simple_logger.h"
#include "entity_common.h"
#include "level.h"
#include "player.h"

Collision entity_scan_hit(Entity *self,Vector2D start,Vector2D end, CollisionFilter filter)
{
    Collision c = {0};
    if (!self)return c;
    filter.ignore = &self->body;
    c = gf2d_collision_trace_space(level_get_space(), start, end ,filter);
    gf2d_shape_draw(gf2d_shape_edge(start.x,start.y,end.x,end.y),gf2d_color(255,255,0,255),camera_get_offset());
    return c;
}

void entity_damage(Entity *target,Entity *killer,int damage,float kick)
{
    if ((!target)||(!killer))
    {
        slog("missing entity data");
        return;
    }
    if (target->damage != NULL)
    {
        target->damage(target,damage,killer);
        entity_push(killer,target,kick);
    }
}

void entity_push(Entity *self,Entity *other,float amount)
{
    Vector2D push;
    if ((!self)||(!other))
    {
        slog("missing an entity");
        return;
    }
    vector2d_sub(push,other->position,self->position);
    vector2d_set_magnitude(&push,amount);
    vector2d_add(other->velocity,other->velocity,push);
    vector2d_add(other->body.velocity,other->body.velocity,push);
}

int entity_camera_view(Entity *self)
{
    Rect r,c;
    if (!self)return 0;
    c = camera_get_dimensions();
    r = gf2d_shape_get_bounds(self->shape);
    vector2d_add(r,r,self->position);
    return gf2d_rect_overlap(r,c);
}

void entity_apply_gravity(Entity *self)
{
    self->velocity.y += 0.58;
    if (entity_ground_check(self,2))
    {
        if (self->velocity.y > 0)self->velocity.y = 0;
        self->grounded = 1;
    }
    else
    {
        self->grounded = 0;
    }
}

int entity_roof_check(Entity *self, float width)
{
    Rect r;
    int i,count;
    Collision *c;
    List *collisionList;
    CollisionFilter filter = {
        1,
        0,
        0,
        0,
        &self->body
    };

    if (!self)return 0;
    r = gf2d_shape_get_bounds(self->shape);
    r.x += 0.1;
    r.w -= 0.2;
    r.y -= (0.1 + width);
    r.h = width;
    vector2d_add(r,r,self->position);

    collisionList = gf2d_collision_check_space_shape(level_get_space(), gf2d_shape_from_rect(r),filter);
    if (collisionList != NULL)
    {
        count = gf2d_list_get_count(collisionList);
        for (i = 0;i < count;i++)
        {
            c = (Collision*)gf2d_list_get_nth(collisionList,i);
            if (!c)continue;
            if (!c->shape)continue;
            gf2d_shape_draw(*c->shape,gf2d_color(255,255,0,255),camera_get_offset());
        }
        gf2d_collision_list_free(collisionList);
        return 1;
    }
    return 0;
}


int entity_ground_check(Entity *self, float width)
{
    Shape s;
    int i,count;
    Collision *c;
    List *collisionList = NULL;
    CollisionFilter filter = {
        1,
        0,
        0,
        0,
        &self->body
    };

    if (!self)return 0;    
    s = gf2d_body_to_shape(&self->body);
    gf2d_shape_move(&s,vector2d(0,width));
    
    collisionList = gf2d_collision_check_space_shape(level_get_space(), s,filter);
    if (collisionList != NULL)
    {
        count = gf2d_list_get_count(collisionList);
        for (i = 0;i < count;i++)
        {
            c = (Collision*)gf2d_list_get_nth(collisionList,i);
            if (!c)continue;
            if (!c->shape)continue;
            gf2d_shape_draw(*c->shape,gf2d_color(255,255,0,255),camera_get_offset());
        }
        gf2d_collision_list_free(collisionList);
        return 1;
    }
    return 0;
}

int entity_left_check(Entity *self, float width)
{
    Rect r;
    int i,count;
    Collision *c;
    List *collisionList;
    CollisionFilter filter = {
        1,
        0,
        0,
        0,
        &self->body
    };
    
    if (!self)return 0;
    r = gf2d_shape_get_bounds(self->shape);
    r.x -= (0.1 + width);
    r.w = width;
    r.y += 0.1;
    r.h -= 0.2;
    vector2d_add(r,r,self->position);

    collisionList = gf2d_collision_check_space_shape(level_get_space(), gf2d_shape_from_rect(r),filter);
    if (collisionList != NULL)
    {
        count = gf2d_list_get_count(collisionList);
        for (i = 0;i < count;i++)
        {
            c = (Collision*)gf2d_list_get_nth(collisionList,i);
            if (!c)continue;
            if (!c->shape)continue;
            gf2d_shape_draw(*c->shape,gf2d_color(255,255,0,255),camera_get_offset());
        }
        gf2d_collision_list_free(collisionList);
        return 1;
    }
    return 0;
}

int entity_right_check(Entity *self, float width)
{
    Rect r;
    int i,count;
    Collision *c;
    List *collisionList;
    CollisionFilter filter = {
        1,
        0,
        0,
        0,
        &self->body
    };

    if (!self)return 0;
    r = gf2d_shape_get_bounds(self->shape);
    r.x += r.w + 0.11;
    r.w = width;
    r.y += 0.1;
    r.h -= 0.2;
    vector2d_add(r,r,self->position);

    collisionList = gf2d_collision_check_space_shape(level_get_space(), gf2d_shape_from_rect(r),filter);
    if (collisionList != NULL)
    {
        count = gf2d_list_get_count(collisionList);
        for (i = 0;i < count;i++)
        {
            c = (Collision*)gf2d_list_get_nth(collisionList,i);
            if (!c)continue;
            if (!c->shape)continue;
            gf2d_shape_draw(*c->shape,gf2d_color(255,255,0,255),camera_get_offset());
        }
        gf2d_collision_list_free(collisionList);
        return 1;
    }
    return 0;
}

void entity_world_snap(Entity *self)
{
    if (entity_ground_check(self,0.1))
    {
        self->position.y -= 0.1;
    }
    if (entity_roof_check(self,0.1))
    {
        self->position.y += 0.1;
    }
    if (entity_right_check(self,0.1))
    {
        self->position.x -= 0.1;
    }
    if (entity_left_check(self,0.1))
    {
        self->position.x += 0.1;
    }    
}

void entity_clear_all_but_player()
{
    Entity *ent = NULL;
    for (ent = gf2d_entity_iterate(NULL);ent != NULL;ent = gf2d_entity_iterate(ent))
    {
        if (ent == player_get())continue;
        gf2d_entity_free(ent);
    }
}

Entity *entity_get_from_body(Body *body)
{
    if (!body) return NULL;
    if (!gf2d_entity_validate_entity_pointer(body->data))return NULL;
    return (Entity*)body->data;
}

/*eol@eof*/
