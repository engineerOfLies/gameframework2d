#include "gf2d_collision.h"
#include "simple_logger.h"
#include "gf2d_draw.h"

void gf2d_body_clear(Body *body)
{
    if (!body)return;
    memset(body,0,sizeof(Body));
}

void gf2d_body_set(
    Body *body,
    Uint32      layer,
    Uint32      team,
    Vector2D    position,
    Vector2D    velocity,
    float       mass,
    Shape      *shape,
    void       *data,
    int     (*bodyTouch)(struct Body_S *self, struct Body_S *other, Collision *collision),
    int     (*worldTouch)(struct Body_S *self, Collision *collision))
{
    if (!body)return;
    body->layer = layer;
    body->team = team;
    vector2d_copy(body->position,position);
    vector2d_copy(body->velocity,velocity);
    body->mass = mass;
    body->shape = shape;
    body->data = data;
    body->bodyTouch = bodyTouch;
    body->worldTouch = worldTouch;
}

void gf2d_free_shapes(void *data,void *context)
{
    Shape *shape;
    if (!data)return;
    shape = (Shape*)data;
    free(shape);
}

void gf2d_space_free(Space *space)
{
    if (!space)return;
    gf2d_list_delete(space->bodyList);// bodies are not owned by the space and so will not be deleted by the space
    
    //static shapes ARE owned by the space, so are deleted when the space goes away
    gf2d_list_foreach(space->staticShapes,gf2d_free_shapes,NULL);
    gf2d_list_delete(space->staticShapes);
    free(space);
}

Space *gf2d_space_new_full(
    int         precision,
    Rect        bounds,
    float       timeStep,
    Vector2D    gravity)
{
    Space *space;
    space = gf2d_space_new();
    if (!space)return NULL;
    gf2d_rect_copy(space->bounds,bounds);
    vector2d_copy(space->gravity,gravity);
    space->timeStep = timeStep;
    space->precision = precision;
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
    space->bodyList = gf2d_list_new();
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
    space->bodyList = gf2d_list_delete_data(space->bodyList,(void *)body);
}

void gf2d_space_add_body(Space *space,Body *body)
{
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
    space->bodyList = gf2d_list_append(space->bodyList,(void *)body);
}

void gf2d_body_draw(Body *body)
{
    Vector4D color;
    Shape shape;
    if (!body)return;
    vector4d_set(color,0,255,255,255);
    // draw center point
    gf2d_draw_pixel(body->position,color);
        
    vector4d_set(color,255,0,255,255);
    gf2d_shape_copy(&shape,*body->shape);
    gf2d_shape_move(&shape,body->position);
    gf2d_shape_draw(shape,gf2d_color_from_vector4(color));
}

void gf2d_space_draw(Space *space)
{
    int i,count;
    if (!space)
    {
        slog("no space provided");
        return;
    }
    gf2d_draw_rect(gf2d_rect_to_sdl_rect(space->bounds),vector4d(255,0,0,255));
    count = gf2d_list_get_count(space->bodyList);
    for (i = 0; i < count;i++)
    {
        gf2d_body_draw((Body *)gf2d_list_get_nth(space->bodyList,i));
    }
    count = gf2d_list_get_count(space->staticShapes);
    for (i = 0; i < count;i++)
    {
        gf2d_shape_draw(*(Shape *)gf2d_list_get_nth(space->staticShapes,i),gf2d_color8(0,255,0,255));
    }
}

Uint8 gf2d_body_collide(Body *a,Body *b)
{
    Shape aS,bS;
    if ((!a)||(!b))return 0;
    // set shapes based on each body's current position
    gf2d_shape_copy(&aS,*a->shape);
    gf2d_shape_move(&aS,a->position);

    gf2d_shape_copy(&bS,*b->shape);
    gf2d_shape_move(&bS,b->position);

    return gf2d_shape_overlap(aS, bS);
}



Body *gf2d_body_step(Body *body,Space *space,float step)
{
    int i= 0;
    int attempts = 0;
    Body *other,*collider = NULL;
    int bodies;
    Vector2D velocity;
    if (!body)return NULL;
    if (!space)return NULL;
    vector2d_scale(velocity,body->velocity,step);
    vector2d_add(body->position,body->position,velocity);
    bodies = gf2d_list_get_count(space->bodyList);
    for (attempts = 0;attempts < space->precision;attempts++)
    {
        //bounds check
        //body/body collision check
        for (i = 0; i < bodies; i++)
        {
            other = (Body*)gf2d_list_get_nth(space->bodyList,i);
            if ((!other)||// error check
                (other == body)||//dont self collide
                !(other->layer & body->layer))continue;// only we share a layer
            if ((body->team)&&(other->team == body->team))// no friendly fire
                continue;
                if (gf2d_body_collide(body,other))
                {
                    collider = other;
                    vector2d_scale(velocity,velocity,0.5);
                    //collision
                    vector2d_sub(body->position,body->position,velocity);
                }
                else break;
        }
    }
    if (collider)
    {
        if (body->bodyTouch != NULL)
        {
            body->bodyTouch(body,other,NULL);
        }
        return collider;
    }
    return NULL;
}

void gf2d_space_step(Space *space,float t)
{
    int i= 0;
    Body *body;
    int bodies;
    if (!space)return;
    bodies = gf2d_list_get_count(space->bodyList);
    for (i = 0; i < bodies; i++)
    {
        body = (Body*)gf2d_list_get_nth(space->bodyList,i);
        if ((!body)||(body->inactive))continue;// body already hit something
        
    }
}

void gf2d_space_update(Space *space)
{
    int i;
    float s;
    Body *body;
    int bodies;
    if (!space)return;
    // reset all body tracking
    bodies = gf2d_list_get_count(space->bodyList);
    for (i = 0; i < bodies;i ++)
    {
        body = (Body*)gf2d_list_get_nth(space->bodyList,i);
        if (!body)continue;
        body->inactive = 0;
    }
    for (s = 0; s < 1; s += space->timeStep)
    {
        gf2d_space_step(space,s);
    }
}

/*eol@eof*/
