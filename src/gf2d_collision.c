#include "gf2d_collision.h"
#include "simple_logger.h"
#include "gf2d_draw.h"
#include <stdlib.h>

Collision *gf2d_collision_new()
{
    Collision *collision = NULL;
    collision = (Collision *)malloc(sizeof(Collision));
    if (!collision)
    {
        slog("failed to allocate data for a collision object");
        return NULL;
    }
    memset(collision,0,sizeof(Collision));
    return collision;
}

void gf2d_collision_free(Collision *collision)
{
    if (!collision)return;
    free(collision);
}

void gf2d_collision_list_free(List *list)
{
    int i, count;
    Collision *collision;
    if (!list)return;
    count = gf2d_list_get_count(list);
    for (i = 0; i < count;i++)
    {
        collision = (Collision*)gf2d_list_get_nth(list,i);
        if (!collision)continue;
        gf2d_collision_free(collision);
    }
    gf2d_list_delete(list);
}

Collision *gf2d_collision_space_static_shape_clip(Shape a, Shape *s)
{
    Collision *collision;
    Vector2D poc,normal;
    if (!gf2d_shape_overlap_poc(a, *s, &poc, &normal))
    {
        return NULL;
    }
    collision = gf2d_collision_new();
    collision->collided = 1;
    collision->blocked = 1;
    vector2d_copy(collision->pointOfContact,poc);
    vector2d_copy(collision->normal,normal);
    collision->shape = s;
    collision->body = NULL;
    collision->bounds = 0;
    collision->timeStep = 0; 
    return collision;
}

List *gf2d_collision_check_space_shape(Space *space, Shape shape,CollisionFilter filter)
{
    int i,count;
    Shape *staticShape;
    Collision *collision;
    List *collisionList = NULL;
    collisionList = gf2d_list_new();
    if (filter.worldclip)
    {
        count = gf2d_list_get_count(space->staticShapes);
        for (i = 0; i < count;i++)
        {
            staticShape = (Shape*)gf2d_list_get_nth(space->staticShapes,i);
            if (!staticShape)continue;
            // check for layer compatibility
            collision = gf2d_collision_space_static_shape_clip(shape, staticShape);
            if (collision == NULL)continue;
            collisionList = gf2d_list_append(collisionList,(void*)collision);
        }
        //check if the shape clips the level bounds
/*        collision = gf2d_dynamic_body_bounds_collision_check(db,space->bounds,t);
        if (collision != NULL)
        {
            db->collisionList = gf2d_list_append(db->collisionList,(void*)collision);
        }*/
    }

    return collisionList;
}

Collision gf2d_collision_trace_space(Space *space, Vector2D start, Vector2D end ,CollisionFilter filter)
{
    Collision out = {0};
    Collision *collision = NULL;
    Collision *best = NULL;
    double     bestDistance = -1;
    double     distance;
    double     length;
    int count,i;
    List *collisionList;
    collisionList = gf2d_collision_check_space_shape(space, gf2d_shape_from_edge(gf2d_edge_from_vectors(start,end)),filter);
    if (!collisionList)
    {
        return out;
    }
    count = gf2d_list_get_count(collisionList);
    for (i =0; i < count;i++)
    {
        collision = (Collision*)gf2d_list_get_nth(collisionList,i);
        if (!collision)continue;
        if (!best)
        {
            best = collision;
            bestDistance = vector2d_magnitude_between(start,collision->pointOfContact);
            continue;
        }
        distance = vector2d_magnitude_between(start,collision->pointOfContact);
        if (distance < bestDistance)
        {
            best = collision;
            bestDistance = distance;
        }
    }
    if (best != NULL)
    {
        length = vector2d_magnitude_between(start,end);
        if (!length)
        {
            best->timeStep = 0;
        }
        else
        {
            best->timeStep = bestDistance / length;
        }
        memcpy(&out,best,sizeof(Collision));
    }
    gf2d_collision_list_free(collisionList);
    return out;
}


/*eol@eof*/
