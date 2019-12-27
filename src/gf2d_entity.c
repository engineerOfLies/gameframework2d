#include "simple_logger.h"

#include "camera.h"

#include "gf2d_entity.h"


typedef struct
{
    Uint32 maxEntities;
    Entity *entityList;
    Uint64 autoincrement;
}EntityManager;

static EntityManager entity_manager = {0};

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
    int i;
    if (!self)return;
    if (self->free)self->free(self);
    for (i = 0;i < EntitySoundMax;i++)
    {
        gfc_sound_free(self->sound[i]);
    }
    gf2d_actor_free(&self->actor);
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
            memset(&entity_manager.entityList[i],0,sizeof(Entity));
            entity_manager.entityList[i].id = entity_manager.autoincrement++;
            entity_manager.entityList[i].inuse = 1;
            vector2d_set(entity_manager.entityList[i].scale,1,1);
            entity_manager.entityList[i].actor.color = vector4d(1,1,1,1);// no color shift, opaque
            return &entity_manager.entityList[i];
        }
    }
    return NULL;
}

void gf2d_entity_draw(Entity *self)
{
    Vector2D drawPosition;
    if (!self)return;
    if (!self->inuse)return;
    
    vector2d_sub(drawPosition,self->position,camera_get_position());

    gf2d_particle_emitter_draw(self->pe);

    gf2d_sprite_draw(
        self->actor.sprite,
        drawPosition,
        &self->scale,
        &self->scaleCenter,
        &self->rotation,
        &self->flip,
        &self->actor.color,
        (Uint32) self->actor.frame);
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
    vector2d_copy(self->body.position,self->position);
}

void gf2d_entity_post_sync_body(Entity *self)
{
    if (!self)return;// nothin to do
//    slog("entity %li : %s old position(%f,%f) => new position (%f,%f)",self->id,self->name,self->position,self->body.position);
    vector2d_copy(self->position,self->body.position);
    vector2d_copy(self->velocity,self->body.velocity);
}

void gf2d_entity_update(Entity *self)
{
    if (!self)return;
    if (!self->inuse)return;

    if (self->dead != 0)
    {
        gf2d_entity_free(self);
        return;
    }
    /*collision handles position and velocity*/
    vector2d_add(self->velocity,self->velocity,self->acceleration);

    gf2d_particle_emitter_update(self->pe);

    gf2d_actor_next_frame(&self->actor);

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

int gf2d_entity_deal_damage(Entity *target, Entity *inflictor, Entity *attacker,int damage,Vector2D kick)
{
    Vector2D k;
    int inflicted;
    if (!target)return 0;
    if (!inflictor)return 0;
    if (!attacker)return 0;
    if (!target->damage)return 0;// cannot take damage
    if (!damage)return 0;// no damage to deal
    inflicted = target->damage(target,damage, inflictor);
    vector2d_scale(k,kick,(float)inflicted/(float)damage);
    vector2d_add(target->velocity,k,target->velocity);
    return inflicted;
}

/*eol@eof*/
