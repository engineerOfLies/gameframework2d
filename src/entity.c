#include "simple_logger.h"

#include "gf2d_draw.h"

#include "camera.h"
#include "entity.h"

typedef struct
{
    Entity *entityList;
    Uint32  entityMax;
    Uint32  entityPool;
}EntityManager;

static EntityManager entityManager = {0};

void entity_manager_close();

void entity_manager_init(Uint32 max)
{
    if (!max)
    {
        slog("cannot initialize entity system with zero entities");
        return;
    }
    entityManager.entityList = gfc_allocate_array(sizeof(Entity),max);
    if (!entityManager.entityList)
    {
        slog("failed to allocate %i entities",max);
        return;
    }
    entityManager.entityMax = max;
    atexit(entity_manager_close);
    slog("initialized entity system");
}

void entity_manager_close()
{
    int i;
    if (!entityManager.entityList)return;
    for (i = 0; i < entityManager.entityMax;i++)
    {
        entity_free(&entityManager.entityList[i]);
    }
    free(entityManager.entityList);
    memset(&entityManager,0,sizeof(EntityManager));
    slog("closed entity system");
}

Entity *entity_new()
{
    int i;
    if (!entityManager.entityList)
    {
        slog("entity system has not been initialized!");
        return NULL;
    }
    for (i = 0;i < entityManager.entityMax;i++)
    {
        if (entityManager.entityList[i]._inuse)continue;
        entityManager.entityList[i]._inuse = 1;
        entityManager.entityList[i].id = ++entityManager.entityPool;
        entityManager.entityList[i].color = GFC_COLOR_WHITE;
        //set detaults
        entityManager.entityList[i].scale.x = 1;
        entityManager.entityList[i].scale.y = 1;
        return &entityManager.entityList[i];
    }
    return NULL;
}

Entity *entity_get_by_id(Uint32 id)
{
    int i;
    if (!entityManager.entityList)
    {
        slog("entity system has not been initialized!");
        return NULL;
    }
    for (i = 0;i < entityManager.entityMax;i++)
    {
        if (!entityManager.entityList[i]._inuse)continue;
        if (entityManager.entityList[i].id == id)return &entityManager.entityList[i];
    }
    return NULL;
}

void entity_free(Entity *self)
{
    if (!self)return;
    if (self->free)self->free(self);
    if (self->sprite)gf2d_sprite_free(self->sprite);
    memset(self,0,sizeof(Entity));
}

void entity_draw(Entity *self)
{
    GFC_Rect bounds;
    GFC_Vector2D position,offset;
    if (!self)return;
    offset = camera_get_offset();
    gfc_vector2d_add(position,self->position,offset);
    gf2d_sprite_draw(
        self->sprite,
        position,
        &self->scale,
        &self->rotationCenter,
        &self->rotation,
        NULL,
        &self->color,
        (Uint32)self->frame);
    bounds = self->bounds;
    gfc_vector2d_add(bounds,bounds,position);
    gf2d_draw_rect(bounds,GFC_COLOR_RED);
}

Uint8 entity_collision_test(Entity *self,Entity *other)
{
    GFC_Rect bounds1,bounds2;
    if ((!self)||(!other)||(self == other))return 0;
    bounds1 = self->bounds;
    bounds2 = other->bounds;
    gfc_vector2d_add(bounds1,self->bounds,self->position);
    gfc_vector2d_add(bounds2,other->bounds,other->position);
    //now in same space frame of reference
    return gfc_rect_overlap(bounds1,bounds2);
}

Uint8 entity_collision_test_world(Entity *self)
{
    int i;
    if (!self)return 0;
    for (i = 0;i < entityManager.entityMax;i++)
    {
        if (!entityManager.entityList[i]._inuse)continue;
        if (entity_collision_test(self,&entityManager.entityList[i]))
        {
            if (self->touch)self->touch(self,&entityManager.entityList[i]);
            return 1;
        }
    }
    return 0;
}

void entity_manager_draw_all()
{
    int i;
    if (!entityManager.entityList)
    {
        slog("entity system has not been initialized!");
        return;
    }
    for (i = 0;i < entityManager.entityMax;i++)
    {
        if (!entityManager.entityList[i]._inuse)continue;
        entity_draw(&entityManager.entityList[i]);
    }
}

void entity_manager_think_all()
{
    int i;
    if (!entityManager.entityList)
    {
        slog("entity system has not been initialized!");
        return;
    }
    for (i = 0;i < entityManager.entityMax;i++)
    {
        if (!entityManager.entityList[i]._inuse)continue;
        if (!entityManager.entityList[i].think)continue;
        entityManager.entityList[i].think(&entityManager.entityList[i]);
    }
}

void entity_update(Entity *self)
{
    if (!self)return;
    gfc_vector2d_add(self->position,self->position,self->velocity);
    
    if (gfc_vector2d_magnitude(self->velocity) > GFC_EPSILON)
    {
        gfc_vector2d_scale(self->velocity,self->velocity,0.5);
    }
    else gfc_vector2d_clear(self->velocity);
    
    if (self->update)self->update(self);
    entity_collision_test_world(self);
}

void entity_manager_update_all()
{
    int i;
    if (!entityManager.entityList)
    {
        slog("entity system has not been initialized!");
        return;
    }
    for (i = 0;i < entityManager.entityMax;i++)
    {
        if (!entityManager.entityList[i]._inuse)continue;
        if (!entityManager.entityList[i].update)continue;
        entity_update(&entityManager.entityList[i]);
    }
}
/*eol@eof*/
