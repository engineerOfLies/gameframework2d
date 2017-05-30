#include "gf2d_collision.h"
#include "simple_logger.h"
#include "gf2d_draw.h"

void gf2d_body_clear(Body *body)
{
    if (!body)return;
    memset(body,0,sizeof(Body));
}

void gf2d_body_set(
    Body       *body,
    char       *name,
    Uint32      layer,
    Uint32      team,
    Vector2D    position,
    Vector2D    velocity,
    float       mass,
    float       gravity,
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
    body->gravity = gravity;
    body->shape = shape;
    body->data = data;
    body->bodyTouch = bodyTouch;
    body->worldTouch = worldTouch;
    gf2d_word_cpy(body->name,name);
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
    Vector2D    gravity,
    float       dampening)
{
    Space *space;
    space = gf2d_space_new();
    if (!space)return NULL;
    gf2d_rect_copy(space->bounds,bounds);
    vector2d_copy(space->gravity,gravity);
    space->timeStep = timeStep;
    space->precision = precision;
    space->dampening = dampening;
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

Vector2D gf2d_body_normal(Body *body,Vector2D poc, Vector2D *normal)
{
    Vector2D n = {0,0};
    // given the point of contact make sure the normal points Away from the body
    if (!body)return n;
    if (!normal)
    {
        // calculate based on relative position of POC
        n.x = poc.x - body->position.x;
        n.y = poc.y - body->position.y;
        vector2d_normalize(&n);
        return n;
    }
    vector2d_copy(n,(*normal));
    if ((normal->x > 0) && (body->position.x < poc.x))
    {
        n = vector2d_rotate(n,GF2D_PI);
        return n;
    }
    if ((normal->x < 0) && (body->position.x > poc.x))
    {
        n = vector2d_rotate(n,GF2D_PI);
        return n;
    }
    if ((normal->y > 0) && (body->position.y < poc.y))
    {
        n = vector2d_rotate(n,GF2D_PI);
        return n;
    }
    if ((normal->y < 0) && (body->position.y > poc.y))
    {
//        vector2d_negate(n,n);
        n = vector2d_rotate(n,GF2D_PI);
        return n;
    }
    return n;
}

void gf2d_body_adjuct_bounds_collision_velocity(Body *a,Vector2D poc, Vector2D normal)
{
    Vector2D epsilon;

    normal = gf2d_body_normal(a,poc, &normal);

    vector2d_copy(epsilon,normal);
    vector2d_set_magnitude(&epsilon,GF2D_EPSILON);
    vector2d_add(a->position,a->position,epsilon);
    if (normal.x > GF2D_EPSILON)
    {
        a->newvelocity.x = fabs(a->newvelocity.x);
    }
    else if (normal.x < -GF2D_EPSILON)
    {
        a->newvelocity.x = -fabs(a->newvelocity.x);
    }
    if (normal.y > GF2D_EPSILON)
    {
        a->newvelocity.y = fabs(a->newvelocity.y);
    }
    else if (normal.y < -GF2D_EPSILON)
    {
        a->newvelocity.y = -fabs(a->newvelocity.y);
    }
}

void gf2d_body_adjust_collision_velocity(Body *a,Body *b,Vector2D poc, Vector2D normal)
{
    double phi;//contact angle
    double theta1,theta2;// movement angles
    double v1,v2;
    double part1;
    Vector2D nv;
    if ((!a)||(!b))return;
    normal = gf2d_body_normal(a,poc, &normal);
    v1 = vector2d_magnitude(a->velocity);
    v2 = vector2d_magnitude(b->velocity);
    theta1 = vector2d_angle(a->velocity)*GF2D_DEGTORAD;
    theta2 = vector2d_angle(b->velocity)*GF2D_DEGTORAD;
    phi = vector2d_angle(normal)*GF2D_DEGTORAD;
    part1 = (v1*cos(theta1-phi)*(a->mass - b->mass) + 2 *b->mass*v2*cos(theta2 - phi))/(a->mass+b->mass);
    nv.x = part1*cos(phi)+v1*sin(theta1-phi)*cos(phi + GF2D_HALF_PI);
    nv.y = part1*sin(phi)+v1*sin(theta1-phi)*sin(phi + GF2D_HALF_PI);
    vector2d_copy(a->newvelocity,nv);
}

Uint8 gf2d_body_collide(Body *a,Body *b,Vector2D *poc, Vector2D *normal)
{
    Shape aS,bS;
    if ((!a)||(!b))return 0;
    // set shapes based on each body's current position
    gf2d_shape_copy(&aS,*a->shape);
    gf2d_shape_move(&aS,a->position);

    gf2d_shape_copy(&bS,*b->shape);
    gf2d_shape_move(&bS,b->position);
    
    return gf2d_shape_overlap_poc(aS, bS,poc,normal);
}

Uint8 gf2d_body_check_bounds(Body *body,Rect bounds,Vector2D *poc,Vector2D *normal)
{
    Shape aS,bs;
    if (!body)return 0;
    bs = gf2d_shape_from_rect(bounds);

    // set shapes based on each body's current position
    gf2d_shape_copy(&aS,*body->shape);
    gf2d_shape_move(&aS,body->position);
    
    if (!gf2d_shape_overlap(aS, bs))
    {// we should DEFINITELY be inside the bounds, so return true here
        return 1;
    }
    if (gf2d_edge_intersect_shape_poc(gf2d_edge(bounds.x,bounds.y,bounds.x+bounds.w,bounds.y),aS,poc,normal)||
        gf2d_edge_intersect_shape_poc(gf2d_edge(bounds.x,bounds.y,bounds.x,bounds.y+bounds.h),aS,poc,normal)||
        gf2d_edge_intersect_shape_poc(gf2d_edge(bounds.x,bounds.y+bounds.h,bounds.x+bounds.w,bounds.y+bounds.h),aS,poc,normal)||
        gf2d_edge_intersect_shape_poc(gf2d_edge(bounds.x+bounds.w,bounds.y,bounds.x+bounds.w,bounds.y+bounds.h),aS,poc,normal))
    {
        return 1;
    }
    return 0;
}

void gf2d_body_pre_step(Body *body,Space *space)
{
    if (!body)return;
    if (vector2d_magnitude_squared(body->velocity)< GF2D_EPSILON)
    {
        body->inactive = 1;
    }
    vector2d_copy(body->newvelocity,body->velocity);
}

void gf2d_body_post_step(Body *body,Space *space)
{
    if (!body)return;
    vector2d_scale(body->newvelocity,body->newvelocity,space->dampening);
    vector2d_copy(body->velocity,body->newvelocity);
}

void gf2d_body_step(Body *body,Space *space,float step)
{
    int i= 0;
    int attempts = 0;
    int collided = 0;
    Collision collision;
    Vector2D poc, normal;
    Vector2D pocB, normalB;
    Body *other,*collider = NULL;
    int bodies;
    Vector2D velocity;
    if (!body)return;
    if (!space)return;
    vector2d_scale(velocity,body->velocity,space->timeStep);
    vector2d_add(body->position,body->position,velocity);
    bodies = gf2d_list_get_count(space->bodyList);

    if (space->precision)
    {
        vector2d_scale(velocity,velocity,1.0/space->precision);
    }

    for (attempts = 0;attempts < space->precision;attempts++)
    {
        //bounds check
        if (gf2d_body_check_bounds(body,space->bounds,&pocB,&normalB))
        {
            collided = 1;
            goto attempt;
        }
        //body/body collision check
        for (i = 0; i < bodies; i++)
        {
            other = (Body*)gf2d_list_get_nth(space->bodyList,i);
            if ((!other)||// error check
                (other == body)||//dont self collide
                !(other->layer & body->layer))continue;// only we share a layer
            if ((body->team)&&(other->team == body->team))// no friendly fire
                continue;
            if (gf2d_body_collide(body,other,&poc,&normal))
            {
                collider = other;
                goto attempt;
            }
        }
        break;
attempt:
        //collision
        vector2d_sub(body->position,body->position,velocity);
    }
    if ((collider)||(collided))
    {
        body->inactive = 1;
    }
    if (collider)
    {
        vector2d_set_magnitude(&velocity,-GF2D_EPSILON);
        vector2d_add(body->position,body->position,velocity);
        if (body->bodyTouch != NULL)
        {
            collision.other = collider->shape;
            vector2d_copy(collision.pointOfContact,poc);
            vector2d_copy(collision.normal,normal);
            collision.timeStep = step;
            body->bodyTouch(body,other,&collision);
        }
        gf2d_body_adjust_collision_velocity(body,collider,poc, normal);
        gf2d_body_adjust_collision_velocity(collider,body,poc, normal);
    }
    if (collided)
    {
        if (body->worldTouch != NULL)
        {
            body->worldTouch(body,NULL);
        }
        gf2d_body_adjuct_bounds_collision_velocity(body,pocB,normalB);
    }
}

void gf2d_space_step(Space *space,float t)
{
    int i= 0;
    Body *body;
    int bodies;
    Vector2D gravityStep;
    Vector2D gravityFactor;
    if (!space)return;
    vector2d_scale(gravityStep,space->gravity,space->timeStep);
    bodies = gf2d_list_get_count(space->bodyList);
    for (i = 0; i < bodies; i++)
    {
        body = (Body*)gf2d_list_get_nth(space->bodyList,i);
        if (!body)continue;// body already hit something
        vector2d_scale(gravityFactor,gravityStep,body->gravity);
        vector2d_add(body->velocity,body->velocity,gravityFactor);
        gf2d_body_pre_step(body,space);
    }
    for (i = 0; i < bodies; i++)
    {
        body = (Body*)gf2d_list_get_nth(space->bodyList,i);
        if ((!body)||(body->inactive))continue;// body already hit something
        // apply gravity
        gf2d_body_step(body,space,t);
    }
    for (i = 0; i < bodies; i++)
    {
        body = (Body*)gf2d_list_get_nth(space->bodyList,i);
        if (!body)continue;// body already hit something
        gf2d_body_post_step(body,space);
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
