#include "gf2d_space.h"
#include "gf2d_draw.h"
#include "simple_logger.h"
#include "gf2d_dynamic_body.h"
#include <stdlib.h>


Uint8 gf2d_body_shape_collide(Body *a,Shape *s,Vector2D *poc, Vector2D *normal);

void gf2d_free_shapes(void *data,void *context)
{
    Shape *shape;
    if (!data)return;
    shape = (Shape*)data;
    free(shape);
}

void gf2d_free_dynamic_bodies(void *data,void *context)
{
    if (!data)return;
    gf2d_dynamic_body_free((DynamicBody*)data);
}

void gf2d_space_free(Space *space)
{
    if (!space)return;
    
    //static shapes ARE owned by the space, so are deleted when the space goes away
    gf2d_list_foreach(space->staticShapes,gf2d_free_shapes,NULL);
    gf2d_list_foreach(space->dynamicBodyList,gf2d_free_dynamic_bodies,NULL);
    gf2d_list_delete(space->staticShapes);
    free(space);
}

Space *gf2d_space_new_full(
    int         precision,
    Rect        bounds,
    float       timeStep,
    Vector2D    gravity,
    float       dampening,
    float       slop)
{
    Space *space;
    space = gf2d_space_new();
    if (!space)return NULL;
    gf2d_rect_copy(space->bounds,bounds);
    vector2d_copy(space->gravity,gravity);
    space->timeStep = timeStep;
    space->precision = precision;
    space->dampening = dampening;
    space->slop = slop;
    return space;
}

Space *gf2d_space_new()
{
    Space *space;
    space = (Space *)malloc(sizeof(Space));
    if (!space)
    {
        slog("failed to allocate space for Space");
        return NULL;
    }
    memset(space,0,sizeof(Space));
    space->dynamicBodyList = gf2d_list_new();
    space->staticShapes = gf2d_list_new();
    return space;
}

void gf2d_space_add_static_shape(Space *space,Shape shape)
{
    Shape *newShape;
    if (!space)
    {
        slog("no space provided");
        return;
    }
    newShape = (Shape*)malloc(sizeof(shape));
    if (!newShape)
    {
        slog("failed to allocate new space for the shape");
        return;
    }
    memcpy(newShape,&shape,sizeof(Shape));
    space->staticShapes = gf2d_list_append(space->staticShapes,(void *)newShape);
}

void gf2d_space_remove_body(Space *space,Body *body)
{
    int i,count;
    DynamicBody *db = NULL;
    if (!space)
    {
        slog("no space provided");
        return;
    }
    if (!body)
    {
        slog("no body provided");
        return;
    }
    if (space->dynamicBodyList)
    {
        count = gf2d_list_get_count(space->dynamicBodyList);
        for (i = 0; i < count;i++)
        {
            db = (DynamicBody*)gf2d_list_get_nth(space->dynamicBodyList,i);
            if (!db)continue;
            if (db->body != body)continue;
            gf2d_dynamic_body_free(db);
            gf2d_list_delete_nth(space->dynamicBodyList,i);
            break;
        }
    }
}

void gf2d_space_add_body(Space *space,Body *body)
{
    DynamicBody *db = NULL;
    if (!space)
    {
        slog("no space provided");
        return;
    }
    if (!body)
    {
        slog("no body provided");
        return;
    }
    db = gf2d_dynamic_body_new();
    if (!db)return;
    db->body = body;
    db->id = space->idpool++;
    space->dynamicBodyList = gf2d_list_append(space->dynamicBodyList,(void *)db);
}

void gf2d_space_draw(Space *space,Vector2D offset)
{
    int i,count;
    SDL_Rect r;
    DynamicBody *db = NULL;
    if (!space)
    {
        slog("no space provided");
        return;
    }
    r = gf2d_rect_to_sdl_rect(space->bounds);
    vector2d_add(r,r,offset);    
    gf2d_draw_rect(r,vector4d(255,0,0,255));
    count = gf2d_list_get_count(space->dynamicBodyList);
    for (i = 0; i < count;i++)
    {
        db = (DynamicBody*)gf2d_list_get_nth(space->dynamicBodyList,i);
        if (!db)continue;
        gf2d_body_draw(db->body,offset);
    }
    count = gf2d_list_get_count(space->staticShapes);
    for (i = 0; i < count;i++)
    {
        gf2d_shape_draw(*(Shape *)gf2d_list_get_nth(space->staticShapes,i),gf2d_color8(0,255,0,255),offset);
    }
}

void gf2d_space_dynamic_bodies_world_clip(Space *space,DynamicBody *db, float t)
{
    int i,count;
    Shape *shape;
    Collision *collision;
    count = gf2d_list_get_count(space->staticShapes);
    for (i = 0; i < count;i++)
    {
        shape = (Shape*)gf2d_list_get_nth(space->staticShapes,i);
        if (!shape)continue;
        // check for layer compatibility
        collision = gf2d_dynamic_body_shape_collision_check(db,shape,t);
        if (collision == NULL)continue;
        db->collisionList = gf2d_list_append(db->collisionList,(void*)collision);
    }
    //check if the dynamic body is leaving the level bounds
    collision = gf2d_dynamic_body_bounds_collision_check(db,space->bounds,t);
    if (collision != NULL)
    {
        db->collisionList = gf2d_list_append(db->collisionList,(void*)collision);
    }
}

void gf2d_space_dynamic_bodies_step(Space *space,DynamicBody *db, float t)
{
    DynamicBody *other;
    Collision *collision;
    Vector2D oldPosition;
    Vector2D reflected,total;
    int normalCount;
    int i,count;
    if ((!space)||(!db))return;
    // save our place in case of collision
    vector2d_copy(oldPosition,db->position);
    vector2d_add(db->position,db->position,db->velocity);
    
    gf2d_dynamic_body_clear_collisions(db);    
    // check against dynamic bodies
    count = gf2d_list_get_count(space->dynamicBodyList);
    for (i = 0; i < count;i++)
    {
        other = (DynamicBody*)gf2d_list_get_nth(space->dynamicBodyList,i);
        if (!other)continue;
        if (other == db)continue;   // skip checking outself
        // check for layer compatibility
        collision = gf2d_dynamic_body_collision_check(db,other,t);
        if (collision == NULL)continue;
        db->collisionList = gf2d_list_append(db->collisionList,(void*)collision);
    }

    if (db->body->worldclip)
    {
        gf2d_space_dynamic_bodies_world_clip(space,db, t);
    }
    if (db->blocked)
    {
        vector2d_copy(db->position,oldPosition);
        gf2d_dynamic_body_resolve_overlap(db,space->slop);
        if (db->body->elasticity > 0)
        {
            count = gf2d_list_get_count(db->collisionList);
            vector2d_clear(total);
            normalCount = 0;
            for (i = 0; i < count; i++)
            {
                collision = (Collision*)gf2d_list_get_nth(db->collisionList,i);
                if (!collision)continue;
                vector2d_add(db->position,db->position,collision->normal);
                reflected = gf2d_dynamic_body_bounce(db,collision->normal);
                if (vector2d_magnitude_squared(reflected) != 0)
                {
                    vector2d_add(total,total,reflected);
                    normalCount++;
                }
            }
            if (normalCount)
            {
                vector2d_scale(total,total,1.0/normalCount);
                db->velocity = total;
                vector2d_set_magnitude(&db->velocity,db->speed);
            }
        }
    }
}

void gf2d_space_step(Space *space,float t)
{
    DynamicBody *db = NULL;
    int i,count;
    if (!space)return;
    count = gf2d_list_get_count(space->dynamicBodyList);
    for (i = 0; i < count;i++)
    {
        db = (DynamicBody*)gf2d_list_get_nth(space->dynamicBodyList,i);
        if (!db)continue;
        if (db->blocked)
        {
            continue;// no need to move something that has already collided
        }
        gf2d_space_dynamic_bodies_step(space,db, t);
    }
}

void gf2d_space_reset_bodies(Space *space)
{
    int i,count;
    if (!space)return;
    count = gf2d_list_get_count(space->dynamicBodyList);
    for (i = 0; i < count;i++)
    {
        gf2d_dynamic_body_reset((DynamicBody*)gf2d_list_get_nth(space->dynamicBodyList,i),space->timeStep);
    }
}

void gf2d_space_update_bodies(Space *space,float loops)
{
    DynamicBody *db = NULL;
    int i,count;
    if (!space)return;
    count = gf2d_list_get_count(space->dynamicBodyList);
    for (i = 0; i < count;i++)
    {
        db = (DynamicBody*)gf2d_list_get_nth(space->dynamicBodyList,i);
        if (!db)continue;
        gf2d_dynamic_body_update(db,loops);
    }
}

void gf2d_space_update(Space *space)
{
    float s;
    float loops = 0;
    if (!space)return;
    gf2d_space_fix_overlaps(space,8);
    gf2d_space_reset_bodies(space);
    // reset all body tracking
    for (s = 0; s <= 1; s += space->timeStep)
    {
        gf2d_space_step(space,s);
        loops = loops + 1;
    }
    gf2d_space_update_bodies(space,loops);
}

Uint8 gf2d_space_resolve_overlap(Space *space)
{
    DynamicBody *db = NULL;
    int i,count;
    int clipped = 0;
    if (!space)return 1;
    gf2d_space_reset_bodies(space);
    // for each dynamic body, get list of staic shapes that are clipped
    count = gf2d_list_get_count(space->dynamicBodyList);
    for (i = 0; i < count;i++)
    {
        db = (DynamicBody*)gf2d_list_get_nth(space->dynamicBodyList,i);
        if (!db)continue;
        gf2d_space_dynamic_bodies_world_clip(space,db, 0);
        if (gf2d_list_get_count(db->collisionList))
        {
            gf2d_dynamic_body_resolve_overlap(db,space->slop);
        }
    }
    return clipped;
}

void gf2d_space_fix_overlaps(Space *space,Uint8 tries)
{
    int i = 0;
    int done = 0;
    for (i = 0; (i < tries) & (done != 1);i++)
    {
        done = gf2d_space_resolve_overlap(space);
    }
}

/*eol@eof*/
