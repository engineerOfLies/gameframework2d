#include "gf2d_entity.h"
#include "camera.h"
#include "simple_logger.h"


typedef struct
{
    Uint32 maxEntities;
    Entity *entityList;
}EntityManager;

static EntityManager entity_manager;

void gf2d_entity_system_close()
{
    int i;
    if (entity_manager.entityList != NULL)
    {
        for (i = 0;i < entity_manager.maxEntities;i++)
        {
            gf2d_entity_free(&entity_manager.entityList[i]);
        }
        free(entity_manager.entityList);
    }
    memset(&entity_manager,0,sizeof(EntityManager));
    slog("entity system closed");
}

void gf2d_entity_system_init(Uint32 maxEntities)
{
    if (!maxEntities)
    {
        slog("cannot initialize entity system for zero entities");
        return;
    }
    memset(&entity_manager,0,sizeof(EntityManager));
    
    entity_manager.entityList = (Entity*)malloc(sizeof(Entity)*maxEntities);
    if (!entity_manager.entityList)
    {
        slog("failed to allocate entity list");
        gf2d_entity_system_close();
        return;
    }
    memset(entity_manager.entityList,0,sizeof(Entity)*maxEntities);
    entity_manager.maxEntities = maxEntities;
    atexit(gf2d_entity_system_close);
    slog("entity system initialized");
}

void gf2d_entity_free(Entity *self)
{
    if (!self)return;
    gf2d_action_list_free(self->al);
    gf2d_sprite_free(self->sprite);
    gf2d_particle_emitter_free(self->pe);
    memset(self,0,sizeof(Entity));
}

Entity *gf2d_entity_new()
{
    int i;
    for (i = 0; i < entity_manager.maxEntities;i++)
    {
        if (entity_manager.entityList[i].inuse == 0)
        {
            entity_manager.entityList[i].inuse = 1;
            vector2d_set(entity_manager.entityList[i].scale,1,1);
            entity_manager.entityList[i].color = gf2d_color(1,1,1,1);// no color shift, opaque
            return &entity_manager.entityList[i];
        }
    }
    return NULL;
}

void gf2d_entity_draw(Entity *self)
{
    Vector4D color;
    Vector2D drawPosition;
    if (!self)return;
    if (!self->inuse)return;
    
    vector2d_sub(drawPosition,self->position,camera_get_position());

    gf2d_particle_emitter_draw(self->pe);

    color = gf2d_color_to_vector4(self->color);
    gf2d_sprite_draw(
        self->sprite,
        drawPosition,
        &self->scale,
        &self->scaleCenter,
        &self->rotation,
        &self->flip,
        &color,
        (Uint32) self->frame);
    if (self->draw != NULL)
    {
        self->draw(self);
    }
}

void gf2d_entity_draw_all()
{
    int i;
    for (i = 0; i < entity_manager.maxEntities;i++)
    {
        if (entity_manager.entityList[i].inuse == 0)continue;
        gf2d_entity_draw(&entity_manager.entityList[i]);
    }
}

void gf2d_entity_pre_sync_body(Entity *self)
{
    if (!self)return;// nothin to do
    vector2d_copy(self->body.velocity,self->velocity);
}

void gf2d_entity_post_sync_body(Entity *self)
{
    if (!self)return;// nothin to do
    vector2d_copy(self->position,self->body.position);
}

void gf2d_entity_update(Entity *self)
{
    if (!self)return;
    if (!self->inuse)return;

    /*collision handles position and velocity*/
    vector2d_add(self->velocity,self->velocity,self->acceleration);

    gf2d_particle_emitter_update(self->pe);

    gf2d_action_list_get_next_frame(self->al,&self->frame,self->action);
    
    if (self->update != NULL)
    {
        self->update(self);
    }
}

void gf2d_entity_think_all()
{
    int i;
    for (i = 0; i < entity_manager.maxEntities;i++)
    {
        if (entity_manager.entityList[i].inuse == 0)continue;
        if (entity_manager.entityList[i].think != NULL)
        {
            entity_manager.entityList[i].think(&entity_manager.entityList[i]);
        }
    }
}

void gf2d_entity_pre_sync_all()
{
    int i;
    for (i = 0; i < entity_manager.maxEntities;i++)
    {
        if (entity_manager.entityList[i].inuse == 0)continue;
        gf2d_entity_pre_sync_body(&entity_manager.entityList[i]);
    }
}

void gf2d_entity_post_sync_all()
{
    int i;
    for (i = 0; i < entity_manager.maxEntities;i++)
    {
        if (entity_manager.entityList[i].inuse == 0)continue;
        gf2d_entity_post_sync_body(&entity_manager.entityList[i]);
    }
}

void gf2d_entity_update_all()
{
    int i;
    for (i = 0; i < entity_manager.maxEntities;i++)
    {
        if (entity_manager.entityList[i].inuse == 0)continue;
        gf2d_entity_update(&entity_manager.entityList[i]);
    }
}

/*eol@eof*/
