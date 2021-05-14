#include <simple_logger.h>
#include <simple_json.h>

#include "gf2d_sprite.h"

#include "planet.h"
#include "regions.h"

typedef struct
{
    Uint32 planetCount;
    SJson *config;
    SJson *planetList;
    Sprite **planet;
}PlanetManager;

static PlanetManager planet_manager = {0};

void planet_close()
{
    int i;
    slog("freeing system data");
    for (i = 0;i < planet_manager.planetCount;i++)
    {
        gf2d_sprite_free(planet_manager.planet[i]);
    }
    free(planet_manager.planet);
    sj_free(planet_manager.config);
    memset(&planet_manager,0,sizeof(PlanetManager));
}

void planet_init()
{
    SJson *planetData;
    const char *planetSprite;
    int i,count;
    slog("initializing planet data");
    planet_manager.config = sj_load("config/planets.json");
    if (!planet_manager.config)
    {
        slog("failed to load plant config data!");
        return;
    }
    planet_manager.planetList = sj_object_get_value(planet_manager.config,"planets");
    if (!planet_manager.planetList)
    {
        slog("failed to parse planet config data, missing planets");
        sj_free(planet_manager.config);
        planet_manager.config  = NULL;
        return;
    }
    count = sj_array_get_count(planet_manager.planetList);
    if (!count)
    {
        slog("failed to parse planet config data, missing planets");
        sj_free(planet_manager.config);
        planet_manager.config  = NULL;
        return;
    }
    planet_manager.planet = gfc_allocate_array(sizeof(Sprite *),count);
    for (i = 0;i < count;i++)
    {
        planetData = sj_array_get_nth(planet_manager.planetList,i);
        if (!planetData)continue;
        planetSprite = sj_get_string_value(sj_object_get_value(planetData,"sprite"));
        if (!planetSprite)continue;
        planet_manager.planet[i] = gf2d_sprite_load_image((char *)planetSprite);
    }
    planet_manager.planetCount = count;
    atexit(planet_close);
}


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

Planet *planet_generate(Uint32 *id, int planetType, Uint32 seed, Vector2D position)
{
    int moonCount,i;
    Vector2D newPosition = {0}, childPosition= {0};
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
    vector2d_copy(planet->systemPosition,position);    
    planet->classification = planetType;

    vector2d_copy(newPosition,position);
    srand(*id + seed);

    planet->size = (int)(gfc_random() * 10.0);
    childPlanetType = PC_Moon;
    childPosition.x = 128;
    if (planet->classification == PC_GasGiant)planet->size *= 10;
    else if (planet->classification <= PC_Star)
    {
        childPosition.x = 0;
        childPosition.y = 256;
        planet->size *= 100;
        childPlanetType = (int)(gfc_random() * (PC_Moon - PC_GasGiant)) + PC_GasGiant;
    }
    else if (planet->classification == PC_Moon)planet->size *= 0.1;
    planet->color = gfc_color_hsl(360 * ((float)planet->classification/PC_MAX) - 30,1,0.5,1);

    
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
        vector2d_add(newPosition,newPosition,childPosition);
        planet->children = gfc_list_append(planet->children,planet_generate(id, childPlanetType, seed,newPosition));
    }
    return planet;
}

void planet_list_free(List *list);
List *planet_list_get_from_json(SJson *json);
SJson planet_list_to_json(List *planetList);
Planet *planet_get_by_id(List *planetList, Uint32 id);

Planet *planet_load_from_json(SJson *json);
SJson *planet_save_to_json(Planet *planet);

void planet_draw_system_view(Planet *planet,Vector2D offset)
{
    int i,count;
    float drawScale;
    Planet *moon;
    SJson *planetData = NULL;
    Vector4D color;
    Vector2D scale,scalecenter;
    Vector2D drawposition;
    if (!planet)return;
    vector2d_add(drawposition,planet->systemPosition,offset);
    planetData = sj_array_get_nth(planet_manager.planetList,planet->classification);
    sj_get_float_value(sj_object_get_value(planetData,"drawScale"),&drawScale);
    scale = vector2d(
            drawScale,
            drawScale);
    color = gfc_color_to_vector4(planet->color);
    scalecenter.x = planet_manager.planet[planet->classification]->frame_w * 0.5;
    scalecenter.y = planet_manager.planet[planet->classification]->frame_h * 0.5;
    drawposition.x -= (scalecenter.x * scale.x);
    drawposition.y -= (scalecenter.y * scale.y);

    gf2d_sprite_draw(
        planet_manager.planet[planet->classification],
        drawposition,
        &scale,
        NULL,
        NULL,
        NULL,
        &color,
        0);
    count = gfc_list_get_count(planet->children);
    for (i =0 ; i < count; i++)
    {
        moon = gfc_list_get_nth(planet->children,i);
        if (!moon)continue;
        planet_draw_system_view(moon,offset);
    }
}

void planet_draw_planet_view(Planet *planet);

/*the end of the file*/
