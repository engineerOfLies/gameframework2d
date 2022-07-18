#include "simple_logger.h"
#include "gf2d_entity.h"
#include "gf2d_camera.h"
#include "gf2d_particle_effects.h"

extern int __DebugMode;

typedef struct
{
    Uint32 maxEntities;
    Entity *entityList;
    Uint64 autoincrement;
}EntityManager;

static EntityManager entity_manager = {0};

static const char *damage_types[] = 
{
    "Physical",
    "Arcane",
    "Fire",
    "Ice",
    "Light",
    "Dark"
};

const char *gf2d_entity_damage_type_to_name(DamageTypes type)
{
    if (type >= DT_MAX)return NULL;
    return damage_types[type];
}

EntityState gf2d_entity_state_from_name(const char *state)
{
    if (strcmp(state,"idle")==0)return ES_Idle;
    if (strcmp(state,"activated")==0)return ES_Activated;
    if (strcmp(state,"attacking")==0)return ES_Attacking;
    if (strcmp(state,"locked")==0)return ES_Locked;
    if (strcmp(state,"open")==0)return ES_Open;
    if (strcmp(state,"walking")==0)return ES_Walking;
    if (strcmp(state,"jumpumping")==0)return ES_Jumping;
    if (strcmp(state,"seeking")==0)return ES_Seeking;
    if (strcmp(state,"charging")==0)return ES_Charging;
    if (strcmp(state,"cooldown")==0)return ES_Cooldown;
    if (strcmp(state,"leaving")==0)return ES_Leaving;
    if (strcmp(state,"inactive")==0)return ES_Inactive;
    if (strcmp(state,"pain")==0)return ES_Pain;
    if (strcmp(state,"dying")==0)return ES_Dying;
    if (strcmp(state,"dead")==0)return ES_Dead;
    return ES_Idle;//default
}

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

int gf2d_entity_validate_entity_pointer(void *p)
{
    Entity *ent;
    if (!p)return 0;
    ent = (Entity *)p;
    if (ent < entity_manager.entityList)return 0;
    if (ent >= &entity_manager.entityList[entity_manager.maxEntities])return 0;
    return 1;
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

Entity *gf2d_entity_iterate(Entity *start)
{
    Entity *p = NULL;
    if (!start)p = entity_manager.entityList;
    else 
    {
        p = start;
        p++;
    }
    for (;p != &entity_manager.entityList[entity_manager.maxEntities];p++)
    {
        if (p->inuse)return p;
    }
    return NULL;
}

void gf2d_entity_free(Entity *self)
{
    EntityLink *link;
    int i,c;
    if (!self)return;
    if (self->free)self->free(self);
    gfc_sound_pack_free(self->sound);
    c = gfc_list_get_count(self->links);
    for (i = 0; i < c; i++)
    {
        link = gfc_list_get_nth(self->links,i);
        if (!link)continue;
        free(link);
    }
    gfc_list_delete(self->links);
    gf2d_actor_free(self->actor);
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
            entity_manager.entityList[i].inuse = 1;
            entity_manager.entityList[i].color = gfc_color8(255,255,255,255);
            vector2d_set(entity_manager.entityList[i].scale,1,1);
            return &entity_manager.entityList[i];
        }
    }
    return NULL;
}

void gf2d_entity_draw(Entity *self)
{
    Vector2D drawPosition,drawScale;
    if (!self)return;
    if (!self->inuse)return;
    
    vector2d_add(drawPosition,self->body.position,gf2d_camera_get_offset());

    if (self->figureInstance.figure)
    {
        vector2d_copy(drawScale,self->scale);
        if (self->flip.x)
        {
            drawScale.x *= -1;
        }
        gf2d_figure_instance_draw(
            &self->figureInstance,
            drawPosition,
            &drawScale,
            &self->rotation,
            &self->color,
            1);
        
        if (__DebugMode)
        {
            if (self->figureInstance.figure->armature)
            {
                gf2d_armature_draw_pose(
                    self->figureInstance.figure->armature,
                    self->figureInstance.frame,
                    drawPosition,
                    drawScale,
                    self->rotation,
                    self->color);
            }
        }
    }
    else if (self->actor)
    {
        gf2d_actor_draw(
            self->actor,
            self->figureInstance.frame,
            drawPosition,
            &self->scale,
            &self->center,
            &self->rotation,
            &self->color,
            &self->flip);
    }
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

void gf2d_entity_play_sound(Entity *self, const char *name)
{
    if ((!self)||(!name))return;
    gfc_sound_pack_play(self->sound,name,0,1,-1,-1);
}

int gf2d_entity_get_animation_frames(Entity *self)
{
    if (!self)return -1;
    return gf2d_action_get_animation_frames(self->figureInstance.action);
}

int gf2d_entity_get_action_frame(Entity *self)
{
    if (!self)return -1;
    return gf2d_action_get_action_frame(self->figureInstance.action,self->figureInstance.frame);
}


const char *gf2d_entity_get_action(Entity *self)
{
    if (!self)return NULL;
    if (!self->figureInstance.action)return NULL;
    return self->figureInstance.action->name;
}

Uint32 gf2d_entity_set_action(Entity *self, const char *action)
{
    if (!self)return 0;
    if (self->figureInstance.figure)
    {
        self->figureInstance.action = gf2d_figure_set_action(self->figureInstance.figure,action,&self->figureInstance.frame);
        if (self->figureInstance.action == NULL)
        {
            self->figureInstance.action = gf2d_figure_get_action_by_index(self->figureInstance.figure,0);
            if (self->figureInstance.action)gf2d_figure_set_action(self->figureInstance.figure,action,&self->figureInstance.frame);
        }
    }
    else if (self->actor)
    {
        self->figureInstance.action = gf2d_actor_set_action(self->actor, action,&self->figureInstance.frame);
        if (self->figureInstance.action == NULL)
        {
            self->figureInstance.action = gf2d_actor_get_action_by_index(self->actor,0);
            if (self->figureInstance.action)gf2d_actor_set_action(self->actor, action,&self->figureInstance.frame);
        }
    }
    if (self->figureInstance.action == NULL)
    {
        slog("no valid actions for entity '%s'",self->name);
        return 0;
    }
    return gf2d_action_get_frames_remaining(self->figureInstance.action,self->figureInstance.frame);
}

EntityLink *gf2d_entity_link_get(Entity *self,const char *name)
{
    int i,c;
    EntityLink *link;
    if ((!self)||(!name))return NULL;
    if (!self->links)return NULL;
    c = gfc_list_get_count(self->links);
    for (i =0;i < c;i++)
    {
        link = gfc_list_get_nth(self->links,i);
        if (!link)continue;
        if (strcmp(link->bonename,name)==0)return link;
    }
    return NULL;
}

Vector2D gf2d_entity_link_get_position(Entity *self, const char *name)
{
    Vector2D position = {0};
    EntityLink *link;
    if ((!self)||(!name))return position;
    link = gf2d_entity_link_get(self,name);
    if (!link)return position;
    return link->body.position;
}

void gf2d_entity_link_body_sync(Entity *self)
{
    Vector2D offset;
    EntityLink *link;
    BonePose *bonepose;
    int i,c;
    if (!self)return;
    if (!self->links)return;
    if (!self->figureInstance.figure)return;// only works if there is an armature
    if (!self->figureInstance.figure->armature)return;// only works if there is an armature
    c = gfc_list_get_count(self->links);
    for (i =0;i < c;i++)
    {
        link = gfc_list_get_nth(self->links,i);
        if (!link)continue;
        bonepose = gf2d_armature_get_bone_pose_by_name(self->figureInstance.figure->armature,self->figureInstance.frame, link->bonename);
        if ((!bonepose)||(!bonepose->bone))continue;
/*        link->body.position = gf2d_armature_get_bonepose_tip_by_name(
            self->figureInstance.figure->armature,
            link->bonename,
            self->figureInstance.frame,
            self->scale,
            self->rotation);*/
        link->body.position = gf2d_armature_get_pose_bone_draw_position(bonepose,self->scale,self->rotation);
        vector2d_copy(offset,link->offset);
        if (self->flip.x)
        {
            offset.x *= -1;
            link->body.position.x *= -1;
        }
        vector2d_add(link->body.position,link->body.position,self->body.position);
        vector2d_add(link->body.position,link->body.position,offset);
    }
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
    vector2d_add(self->body.velocity,self->body.velocity,self->acceleration);
    
    self->ar = gf2d_action_next_frame(self->figureInstance.action,&self->figureInstance.frame);
    self->figureInstance.nextFrame = gf2d_action_next_frame_after(self->figureInstance.action,self->figureInstance.frame);

    if (self->ar == ART_END)
    {
        if (self->actionEnd)self->actionEnd(self);
    }
    gf2d_entity_link_body_sync(self);    
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

void gf2d_entity_update_all()
{
    int i;
    for (i = 0; i < entity_manager.maxEntities;i++)
    {
        if (entity_manager.entityList[i].inuse == 0)continue;
        gf2d_entity_update(&entity_manager.entityList[i]);
    }
}

int gf2d_entity_body_touch(Body *self, List *collisionList)
{
    Entity *selfEnt;
    Collision *c;
    int i,count;
    if (!self)return 0;
    selfEnt = (Entity*)self->data;
    if (!selfEnt->touch)return 0;
    count = gfc_list_get_count(collisionList);
    for (i = 0; i < count; i++)
    {
        c = (Collision *)gfc_list_get_nth(collisionList,i);
        if (!c)continue;
        if (!c->body)continue;
        if (!c->body->data)continue;
        selfEnt->touch(selfEnt,(Entity*)c->body->data);
    }
    return 0;
}

void gf2d_entity_remove_from_space(Entity *ent,Space *space)
{
    EntityLink *link;
    int i,c;
    if ((!ent)||(!space))return;
    gf2d_space_remove_body(space,&ent->body);
    c = gfc_list_get_count(ent->links);
    for (i =0;i < c;i++)
    {
        link = gfc_list_get_nth(ent->links,i);
        if (!link)continue;
        gf2d_space_remove_body(space,&link->body);
    }
}

void gf2d_entity_add_to_space(Entity *ent,Space *space)
{
    EntityLink *link;
    int i,c;
    if ((!ent)||(!space))return;
    if (ent->body.touch == NULL)
    {
        ent->body.touch = gf2d_entity_body_touch;
    }
    gf2d_space_add_body(space,&ent->body);
    c = gfc_list_get_count(ent->links);
    for (i =0;i < c;i++)
    {
        link = gfc_list_get_nth(ent->links,i);
        if (!link)continue;
        link->body.touch = gf2d_entity_body_touch;
        gf2d_space_add_body(space,&link->body);
    }
}

Entity *gf2d_entity_get_by_id(Uint32 id)
{
    int i;
    for (i = 0; i < entity_manager.maxEntities;i++)
    {
        if (entity_manager.entityList[i].inuse == 0)continue;
        if (entity_manager.entityList[i].id == id)
        {
            return &entity_manager.entityList[i];
        }
    }
    return NULL;
}

int gf2d_entity_get_percent_complete(Entity *self)
{
    if (!self)return 0;
    return gf2d_action_get_percent_complete(self->figureInstance.action,self->figureInstance.frame);
}

int gf2d_entity_get_action_frames_remaining(Entity *self)
{
    if (!self)return 0;
    return gf2d_action_get_frames_remaining(self->figureInstance.action,self->figureInstance.frame);
}

Entity *gf2d_entity_get_by_name_id(const char *name,Uint32 id)
{
    int i;
    for (i = 0; i < entity_manager.maxEntities;i++)
    {
        if (entity_manager.entityList[i].inuse == 0)continue;
        if ((gfc_line_cmp(entity_manager.entityList[i].name,name) == 0) && (entity_manager.entityList[i].id == id))
        {
            return &entity_manager.entityList[i];
        }
    }
    return NULL;
}


int gf2d_entity_deal_damage(Entity *target, Entity *inflictor, Entity *attacker,float *damage,Vector2D kick)
{
    Vector2D k;
    int inflicted;
    if (!target)return 0;
    if (!inflictor)return 0;
    if (!attacker)return 0;
    if (!target->damage)return 0;// cannot take damage
    if (!damage)return 0;// no damage to deal
    inflicted = target->damage(target,damage, inflictor);
    vector2d_scale(k,kick,inflicted);
    vector2d_add(target->body.velocity,k,target->body.velocity);
    return inflicted;
}

/*eol@eof*/
