#include <simple_logger.h>
#include <simple_json.h>


#include "galaxy.h"
#include "systems.h"
#include "planet.h"

#include "empire.h"

typedef struct
{
    Uint32  idPool;
    List   *list;
}EmpireData;

static EmpireData empire_data = {0};


void empire_close()
{
    Empire *empire;
    int i,count;
    count = gfc_list_get_count(empire_data.list);
    for (i = 0;i < count;i++)
    {
        empire = gfc_list_get_nth(empire_data.list,i);
        if (!empire)continue;
        empire_free(empire);
    }
    gfc_list_delete(empire_data.list);
    memset(&empire_data,0,sizeof(EmpireData));
    slog("empire system closed");
}

void empire_init()
{
    empire_data.idPool = 1;
    empire_data.list = gfc_list_new();
    atexit(empire_close);
    slog("empire system initialized");
}

Empire *empire_new()
{
    Empire *empire;
    
    empire = gfc_allocate_array(sizeof(Empire),1);
    
    if (!empire)
    {
        slog("failed to allocate memory for a new empire");
        return NULL;
    }
    empire->id = empire_data.idPool++;
    empire->systems = gfc_list_new();
    empire->planets = gfc_list_new();
    empire->armada = gfc_list_new();
    empire->ships = gfc_list_new();
    empire->technologies = gfc_list_new();
    empire_data.list = gfc_list_append(empire_data.list,empire);
    slog("generated new empire");
    return empire;
}

void empire_give_system(Empire *empire,System *system)
{
    if ((!empire)||(!system))return;
    empire->systems = gfc_list_append(empire->systems,system);
}

void empire_give_planet(Empire *empire,Planet *planet)
{
    if ((!empire)||(!planet))return;
    empire->planets = gfc_list_append(empire->planets,planet);
}

void empire_set_home_system(Empire *empire,System *system)
{
    if ((!empire)||(!system))return;
    empire->homeSystem = system;
}

void empire_set_home_planet(Empire *empire,Planet *planet)
{
    if ((!empire)||(!planet))return;
    empire->homeWorld = planet;
}

Empire *empire_generate()
{
    Empire *empire;
    empire = empire_new();
    if (!empire)return NULL;
    
    return empire;
}

void empire_free(Empire *empire)
{
    if (!empire)return;
    slog("freeing empire");
    gfc_list_delete(empire->systems);
    gfc_list_delete(empire->planets);
    gfc_list_delete(empire->armada);
    gfc_list_delete(empire->ships);
    free(empire);
}


/*eol@eof*/
