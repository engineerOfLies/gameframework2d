#include <simple_logger.h>
#include "systems.h"
#include "galaxy.h"


Galaxy *galaxy_new()
{
    Galaxy *galaxy;
    galaxy = gfc_allocate_array(sizeof(Galaxy),1);
    if (!galaxy)
    {
        slog("failed to allocate memory for a new galaxy");
        return NULL;
    }
    galaxy->systemList = gfc_list_new();
    if (!galaxy->systemList)
    {
        slog("failed to allocate memory for galactic systems");
        galaxy_free(galaxy);
        return NULL;
    }
    return galaxy;
}

void galaxy_free(Galaxy *galaxy)
{
    int i,count;
    System* system;
    if (!galaxy)return;
    count = gfc_list_get_count(galaxy->systemList);
    for (i =0 ; i < count; i++)
    {
        system = gfc_list_get_nth(galaxy->systemList,i);
        if (!system)continue;
        system_free(system);
    }
    gfc_list_delete(galaxy->systemList);
    galaxy->systemList = NULL;
    free(galaxy);
}

Galaxy *galaxy_generate(Uint32 seed, Uint32 count)
{
    Uint32 i;
    Galaxy *galaxy;
    galaxy = galaxy_new();
    if (!galaxy)return NULL;
    galaxy->seed = seed;
    srand(galaxy->seed);
    slog("generating galaxy with seed (%i) with %i star systems",galaxy->seed,count);
    for (i = 0; i < count;i++)
    {
        galaxy->systemList = gfc_list_append(galaxy->systemList,system_generate(++galaxy->idPool,galaxy->seed));
    }
    return galaxy;
}

Galaxy *galaxy_load_from_json(SJson *json);

SJson  *galaxy_save_to_json(Galaxy *galaxy);

void    galaxy_draw(Galaxy *galaxy)
{
    System *system;
    int count,i;
    if (!galaxy)return;
    count = gfc_list_get_count(galaxy->systemList);
    for (i =0 ; i < count; i++)
    {
        system = gfc_list_get_nth(galaxy->systemList,i);
        if (!system)continue;
        system_draw_galaxy_view(system);
    }
}



/*eol@eof*/
