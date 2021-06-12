#include <simple_logger.h>
#include <simple_json.h>

#include "gfc_list.h"

#include "gf2d_sprite.h"
#include "gf2d_shape.h"

#include "message_buffer.h"
#include "installations.h"



typedef struct
{
    Uint32 idPool;
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
    int tempInt;
    float tempFloat;
    SJson *resources;
    const char *actorFile,*action;
    Installation *inst;
    if (config == NULL)
    {
        slog("installation config not provided");
        return NULL;
    }
    sj_get_integer_value(sj_object_get_value(config,"iType"),&tempInt);
    if ((tempInt <= IT_None)||(tempInt >= IT_MAX))
    {
        slog("installation type outside of range");
        return NULL;
    }
    inst = installation_new();
    if (!inst)
    {
        return NULL;
    }
    inst->id = ++installation_data.idPool;
    inst->iType = tempInt;
    vector2d_copy(inst->position,position);
    inst->region = region;
    inst->empire = empire;
    gfc_line_cpy(inst->name,sj_get_string_value(sj_object_get_value(config,"name")));
    actorFile = sj_get_string_value(sj_object_get_value(config,"actor"));
    action = sj_get_string_value(sj_object_get_value(config,"action"));
    
    resources = sj_object_get_value(config,"upkeep");
    if (sj_get_float_value(sj_object_get_value(resources,"minerals"),&tempFloat))
    {
        inst->upkeep.minerals = tempFloat;
    }
    if (sj_get_float_value(sj_object_get_value(resources,"credits"),&tempFloat))
    {
        inst->upkeep.credits = tempFloat;
    }
    if (sj_get_float_value(sj_object_get_value(resources,"personnel"),&tempFloat))
    {
        inst->upkeep.population = tempFloat;
    }
    if (sj_get_float_value(sj_object_get_value(resources,"agriculture"),&tempFloat))
    {
        inst->upkeep.agriculture = tempFloat;
    }
    if (sj_get_float_value(sj_object_get_value(resources,"approval"),&tempFloat))
    {
        inst->upkeep.approval = tempFloat;
    }

    resources = sj_object_get_value(config,"production");
    if (sj_get_float_value(sj_object_get_value(resources,"minerals"),&tempFloat))
    {
        inst->production.minerals = tempFloat;
    }
    if (sj_get_float_value(sj_object_get_value(resources,"credits"),&tempFloat))
    {
        inst->production.credits = tempFloat;
    }
    if (sj_get_float_value(sj_object_get_value(resources,"personnel"),&tempFloat))
    {
        inst->production.population = tempFloat;
    }
    if (sj_get_float_value(sj_object_get_value(resources,"agriculture"),&tempFloat))
    {
        inst->production.agriculture = tempFloat;
    }
    if (sj_get_float_value(sj_object_get_value(resources,"approval"),&tempFloat))
    {
        inst->production.approval = tempFloat;
    }
    
    
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
    int upkeepFail = 0;
    Empire *empire;
    Region *region;
    EmpireResources resources;
    float produce = 0;
    if (!inst)return;
    if (!inst->region)return;
    if (!inst->empire)return;
    empire = inst->empire;
    region = inst->region;
    memcpy(&resources,&empire->resources,sizeof(EmpireResources));
    // TODO: check state, and dispatch correct action
    
    // do upkeep
    if (inst->upkeep.credits > 0)
    {
        //we care about credits, so check available credits
        if (resources.credits < inst->upkeep.credits)
        {
            upkeepFail = 1;
        }
        empire_change_credits(empire,-inst->upkeep.credits);
    }
    if (inst->upkeep.agriculture > 0)
    {
        if (resources.agriculture < inst->upkeep.agriculture)
        {
            upkeepFail = 1;
        }
        empire_change_agriculture(empire,-inst->upkeep.agriculture);
    }
    if (inst->upkeep.minerals > 0)
    {
        if (resources.minerals < inst->upkeep.minerals)
        {
            upkeepFail = 1;
        }
        empire_change_minerals(empire,-inst->upkeep.minerals);
    }

    
    
    //do production
    if (upkeepFail)
    {
        inst->iState = RS_Starved;
        message_printf("not enough resources to run facility %s:%i!",inst->name,inst->id);
        return;
    }
    if (inst->production.minerals > 0)
    {
        produce += region->minerals * inst->production.minerals;
        empire_change_minerals(empire,produce);
    }
    if (inst->production.agriculture > 0)
    {
        produce += region->fertility * inst->production.agriculture;
        empire_change_agriculture(empire,region->fertility * inst->production.agriculture);
    }
    if (inst->production.population > 0)
    {
        produce += region->habitable * inst->production.population;
        empire_change_population(empire,region->habitable * inst->production.population);
    }
    if (inst->production.credits > 0)
    {
        produce = produce * inst->production.credits;
        empire_change_credits(empire,produce);
    }
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
