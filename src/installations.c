#include <simple_logger.h>
#include <simple_json.h>

#include "gfc_list.h"

#include "gf2d_sprite.h"
#include "gf2d_shape.h"

#include "installations.h"



typedef struct
{
    SJson *config;
    SJson *items;
}InstallationData;

static InstallationData installation_data = {0};

void installations_close()
{
    if (installation_data.config)
    {
        sj_free(installation_data.config);
    }
    memset(&installation_data,0,sizeof(InstallationData));
}

void installations_init()
{
    installations_close();
    
    installation_data.config = sj_load("config/developments.json");
    installation_data.items = sj_object_get_value(installation_data.config,"items");
    atexit(installations_close);
}

Installation *installation_create_from_config(SJson *config,Vector2D position,Region *region,Empire *empire)
{
    const char *actorFile,*action;
    Installation *inst;
    inst = installation_new();
    if (!inst)
    {
        return NULL;
    }
    vector2d_copy(inst->position,position);
    inst->region = region;
    inst->empire = empire;
    
    gfc_line_cpy(inst->name,sj_get_string_value(sj_object_get_value(config,"name")));
    actorFile = sj_get_string_value(sj_object_get_value(config,"actor"));
    action = sj_get_string_value(sj_object_get_value(config,"action"));
    
    gf2d_actor_load(&inst->actor,actorFile);
    gf2d_actor_set_action(&inst->actor,action);
    
    
    return inst;
}

SJson *installations_get_config_by_type(InstallationType iType)
{
    if (!installation_data.items)return NULL;
    if ((iType <= IT_None)||(iType >= IT_MAX))
    {
        slog("installation type outside of range");
        return NULL;
    }
    return sj_array_get_nth(installation_data.items,iType - 1);
}

Installation *installation_create_by_type(InstallationType iType,Vector2D position,Region *region,Empire *empire)
{
    return installation_create_from_config(installations_get_config_by_type(iType),position,region,empire);
}

Installation *installation_new()
{
    Installation *inst;
    
    inst = gfc_allocate_array(sizeof(Installation),1);
    return inst;
}

void installation_free(Installation *inst)
{
    if (!inst)return;
    free(inst);
}

void installation_update(Installation *inst)
{
    if (!inst)return;
    
}

void installation_draw(Installation *inst,Vector2D offset)
{
    Vector2D scale = {0.75,0.75};
    Vector2D scaleCenter;
    Vector2D drawPosition;
    if (!inst)return;
    vector2d_add(drawPosition,offset,inst->position);
    scaleCenter.x = inst->actor.al->frameWidth *0.5;
    scaleCenter.y = inst->actor.al->frameHeight *0.5;
    gf2d_actor_draw(
        &inst->actor,
        drawPosition,
        &scale,
        &scaleCenter,
        NULL,
        NULL);
}

/*eol@eof*/
