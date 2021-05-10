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

Planet *planet_generate(Uint32 *id, int planetType, Uint32 seed)
{
    int moonCount,i;
    Uint32 childPlanetType;
    Planet *planet;
    planet = planet_new();
    if (!planet)return NULL;
    if (!id)
    {
        slog("missing reference to id pool");
        return NULL;
    }
    if (planetType == -1)
    {
        planetType = (gfc_random() * PC_MAX);
    }
    if (planetType >= PC_MAX)
    {
        slog("error: generating a planet(%i) of type %i, out of range",id, planetType);
    }
    planet->id = *id;
    planet->classification = planetType;
    
    srand(*id + seed);

    planet->size = (int)(gfc_random() * 10.0);
    childPlanetType = PC_Moon;
    if (planet->classification == PC_GasGiant)planet->size *= 10;
    else if (planet->classification <= PC_Star)
    {
        planet->size *= 100;
        childPlanetType = (int)(gfc_random() * (PC_Moon - PC_GasGiant)) + PC_GasGiant;
    }
    else if (planet->classification == PC_Moon)planet->size *= 0.1;

    planet->color = gfc_color_hsl((float)planet->classification/PC_MAX,1,(planet->size + 1.0)/1000,1);
    
    if (planet->classification == PC_Moon)
    {
        slog("generating moon %i",planet->id);
        return planet;// moons are done
    }
    moonCount = (int)(gfc_random() * 5);
    slog("generating planet %i of type %i with %i moons",planet->id,planet->classification,moonCount);
    //moons!
    for (i = 0; i < moonCount;i++)
    {
        *id = *id + 1;
        planet->children = gfc_list_append(planet->children,planet_generate(id, childPlanetType, seed));
    }
    return planet;
}

void planet_list_free(List *list);
List *planet_list_get_from_json(SJson *json);
SJson planet_list_to_json(List *planetList);
Planet *planet_get_by_id(List *planetList, Uint32 id);

Planet *planet_load_from_json(SJson *json);
SJson *planet_save_to_json(Planet *planet);

void planet_draw_system_view(Planet *planet);
void planet_draw_planet_view(Planet *planet);

/*the end of the file*/
