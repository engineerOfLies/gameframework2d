#include <simple_logger.h>
#include "planet.h"
#include "regions.h"

Planet *planet_new()
{
    Planet *planet;
    planet = gfc_allocate_array(sizeof(Planet),1);
    if (!planet)
    {
        slog("failed to allocate memory for a new planet");
        return NULL;
    }
    planet->regions = gfc_list_new();
    if (!planet->regions)
    {
        slog("failed to allocate memory for planet regions");
        planet_free(planet);
        return NULL;
    }
    planet->children = gfc_list_new();
    if (!planet->children)
    {
        slog("failed to allocate memory for planetary children");
        planet_free(planet);
        return NULL;
    }
    return planet;
}


void planet_free(Planet* planet)
{
    int i,count;
    Region *region;
    Planet *moon;
    if (!planet)return;
    count = gfc_list_get_count(planet->regions);
    for (i =0 ; i < count; i++)
    {
        region = gfc_list_get_nth(planet->regions,i);
        if (!region)continue;
        region_free(region);
    }
    gfc_list_delete(planet->regions);
    planet->regions = NULL;

    count = gfc_list_get_count(planet->children);
    for (i =0 ; i < count; i++)
    {
        moon = gfc_list_get_nth(planet->children,i);
        if (!moon)continue;
        planet_free(moon);
    }
    gfc_list_delete(planet->children);
    planet->children = NULL;

    free(planet);
}

void planet_list_free(List *list);
List *planet_list_get_from_json(SJson *json);
SJson planet_list_to_json(List *planetList);
Planet *planet_get_by_id(List *planetList, Uint32 id);

Planet *planet_generate(Uint32 id);
Planet *planet_load_from_json(SJson *json);
SJson *planet_save_to_json(Planet *planet);

void planet_draw_system_view(Planet *planet);
void planet_draw_planet_view(Planet *planet);

/*the end of the file*/
