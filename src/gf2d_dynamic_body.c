#include "gf2d_dynamic_body.h"
#include "gf2d_collision.h"
#include "simple_logger.h"
#include <stdlib.h>

Shape gf2d_dynamic_body_to_shape(DynamicBody *a)
{
    Shape aS = {0};
    if (!a)return aS;
    gf2d_shape_copy(&aS,*a->body->shape);
    gf2d_shape_move(&aS,a->position);
    return aS;
}

Vector2D gf2d_dynamic_body_bounce(DynamicBody *dba,Vector2D normal)
{
    Vector2D nv = {0};
    vector2d_reflect(&nv, normal,dba->velocity);
    return nv;
}

Collision *gf2d_dynamic_body_bounds_collision_check(DynamicBody *dba,Rect bounds,float timeStep)
{
    Collision *collision = NULL;
    Rect dbBounds;
    if (!dba)return NULL;
    if (!dba->body)
    {
        slog("cannot do collision check, body missing from DynamicBody");
        return NULL;
    }
    dbBounds = gf2d_shape_get_bounds(gf2d_dynamic_body_to_shape(dba));
    if ((dbBounds.x > bounds.x)&&(dbBounds.x + dbBounds.w < bounds.x + bounds.w)&&
        (dbBounds.y > bounds.y)&&(dbBounds.y + dbBounds.h < bounds.y + bounds.h))
    {
        // No collision with the level bounds
        return NULL;
    }
    collision = gf2d_collision_new();
    if (!collision)return NULL;
    collision->body = NULL;
    collision->timeStep = timeStep;
    //TODO: collision->pointOfContact;
    if (dbBounds.x <= bounds.x)collision->normal.x = 1;
    if (dbBounds.y <= bounds.y)collision->normal.y = 1;
    if (dbBounds.x + dbBounds.w >= bounds.x + bounds.w)collision->normal.x = -1;
    if (dbBounds.y + dbBounds.h >= bounds.y + bounds.h)collision->normal.y = -1;
    vector2d_normalize(&collision->normal);
    collision->shape = NULL;
    collision->bounds = 1;
    dba->blocked = 1;
    return collision;
}

Collision *gf2d_dynamic_body_shape_collision_check(DynamicBody *dba,Shape *shape,float timeStep)
{
    Collision *collision = NULL;
    if (!dba)return NULL;
    if ((!dba->body)||(!shape))
    {
        slog("cannot do collision check, body or shape shape missing");
        return NULL;
    }
    if (!gf2d_shape_overlap(gf2d_dynamic_body_to_shape(dba),*shape))
    {
        return NULL;
    }
    collision = gf2d_collision_new();
    if (!collision)return NULL;
    collision->body = NULL;
    collision->timeStep = timeStep;
    //TODO: collision->pointOfContact;
    collision->normal = gf2d_shape_get_normal_for_shape(*shape, gf2d_dynamic_body_to_shape(dba));
    collision->shape = shape;
    dba->blocked = 1;
    return collision;
}

Collision *gf2d_dynamic_body_collision_check(DynamicBody *dba,DynamicBody *dbb,float timeStep)
{
    Collision *collision = NULL;
    if ((!dba)||(!dbb))return NULL;
    if ((!dba->body)||(!dbb->body))
    {
        slog("cannot do collision check, body missing from one or more DynamicBody");
        return NULL;
    }
    if ((dba->body->team)&&(dbb->body->team)&&(dba->body->team != dbb->body->team))
    {
        return NULL;
    }
    if (!dba->body->cliplayer)
    {
        return NULL;
    }
    if (!gf2d_shape_overlap(gf2d_dynamic_body_to_shape(dba),gf2d_dynamic_body_to_shape(dbb)))
    {
        return NULL;
    }
    collision = gf2d_collision_new();
    if (!collision)return NULL;
    collision->body = dbb->body;
    collision->timeStep = timeStep;
    //TODO: collision->pointOfContact;
    collision->normal = gf2d_shape_get_normal_for_shape(gf2d_dynamic_body_to_shape(dbb),gf2d_dynamic_body_to_shape(dba));
    collision->shape = dbb->body->shape;
    if (dba->body->cliplayer & dbb->body->cliplayer)
    {
        dba->blocked = 1;
    }
    return collision;
}

DynamicBody *gf2d_dynamic_body_new()
{
    DynamicBody *db;
    db = (DynamicBody *)malloc(sizeof(DynamicBody));
    if (!db)
    {
        slog("failed to allocation data for a new dynamic body");
        return NULL;
    }
    memset(db,0,sizeof(DynamicBody));
    db->collisionList = gf2d_list_new();
    return db;
}

void gf2d_dynamic_body_clear_collisions(DynamicBody *db)
{
    int i, count;
    Collision *collision;
    if (!db)return;
    count = gf2d_list_get_count(db->collisionList);
    for (i = 0; i < count;i++)
    {
        collision = (Collision*)gf2d_list_get_nth(db->collisionList,i);
        if (!collision)continue;
        gf2d_collision_free(collision);
    }
    if (db->collisionList!= NULL)
    {
        gf2d_list_delete(db->collisionList);
        db->collisionList = NULL;
    }
    db->collisionList = gf2d_list_new();
}

void gf2d_dynamic_body_free(DynamicBody *db)
{
    if (!db)return;
    //cleanup collionList
    gf2d_dynamic_body_clear_collisions(db);
    free(db);
}

void gf2d_dynamic_body_update(DynamicBody *db,float factor)
{
    if (!db)return;
    if (!db->body)return;
    vector2d_copy(db->body->position,db->position);
    vector2d_scale(db->body->velocity,db->velocity,factor);
}

void gf2d_dynamic_body_reset(DynamicBody *db,float factor)
{
    if (!db)return;
    db->blocked = 0;
    gf2d_dynamic_body_clear_collisions(db);
    vector2d_copy(db->position,db->body->position);
    vector2d_scale(db->velocity,db->body->velocity,factor);
    db->speed = vector2d_magnitude(db->velocity);
}

void gf2d_dynamic_body_resolve_overlap(DynamicBody *db,float backoff)
{
    int i,count;
    Collision *collision;
    Vector2D total = {0};
    if (!db)return;
    count = gf2d_list_get_count(db->collisionList);
    for (i = 0; i < count; i++)
    {
        collision = (Collision*)gf2d_list_get_nth(db->collisionList,i);
        if (!collision)continue;
        vector2d_add(total,total,collision->normal);
    }
    vector2d_add(db->body->position,db->body->position,total);
}

/*eol@eof*/
