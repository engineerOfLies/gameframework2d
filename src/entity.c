#include "entity.h"

typedef struct
{
    Entity *entity_list;
    Uint32  entity_count;
}EntityManager;

static EntityManager entity_manager = {0};

void entity_system_close()
{
    entity_clear_all(NULL);
    if(entity_manager.entity_list) free(entity_manager.entity_list);
    memset(&entity_manager,0,sizeof(EntityManager));
    slog("entity_system closed");
}

void entity_clear_all(Entity *ignore)
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if(&entity_manager.entity_list[i] == ignore)continue;
        if(!entity_manager.entity_list[i]._inuse)continue;
        entity_free(&entity_manager.entity_list[i]);        
    }
    free(entity_manager.entity_list);
}

void entity_system_init(Uint32 maxEntities)
{
    if(!entity_manager.entity_list)
    {
        slog("An active entity manager already exsists");
        return;
    }
    entity_manager.entity_list = gfc_allocate_array(sizeof(Entity),maxEntities);
    if (entity_manager.entity_list == NULL)
    {
        slog("failed to allocate entity list, cannot allocate ZERO entities");
        return;
    }
    entity_manager.entity_count = maxEntities;
    atexit(entity_system_close);
    slog("entity_system initialized");
}


Entity *entity_new()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet, so we can!
        {
            entity_manager.entity_list[i]._inuse = 1;
            entity_manager.entity_list[i].isPlayer = 0;
            entity_manager.entity_list[i].isEnemy = 0;

            
            return &entity_manager.entity_list[i];
        }
    }
    slog("entity_new: no free space in the entity list");
    return NULL;
}

void entity_free(Entity *self)
{
    if (!self)return;
    //MUST DESTROY
    gf2d_sprite_free(self->sprite);
    if(self->free)self->free(self->customData);
    memset(self,0,sizeof(Entity));
}

void entity_think(Entity *self)
{
    if(!self)return;
    if(self->health <= 0)entity_free(self);
    if(self->think)self->think(self);
}


void entity_think_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)continue;
        entity_think(&entity_manager.entity_list[i]);
    }    
}

void entity_update(Entity *self)
{
    if(!self)return;
    if(self->update)self->update(self);
}


void entity_update_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)continue;
        entity_update(&entity_manager.entity_list[i]);
    }    
}


void entity_draw(Entity *self)
{
    if(!self)return;
    if(self->sprite)
    {
    gf2d_sprite_render(
    self->sprite,
    self->position,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    (Uint32) self->frame);
    }
}

void entity_draw_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)continue;
        entity_draw(&entity_manager.entity_list[i]);
    }    

}


Entity *entity_get_player(void)
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)continue;

        if(entity_manager.entity_list[i].isPlayer == 1)
        {
            return &entity_manager.entity_list[i];
        }
    }
    return NULL;

}