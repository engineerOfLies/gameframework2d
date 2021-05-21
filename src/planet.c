#include <simple_logger.h>
#include <simple_json.h>

#include "gf2d_draw.h"
#include "gf2d_sprite.h"
#include "gf2d_config.h"

#include "planet.h"
#include "regions.h"

typedef struct
{
    Sprite *sprite;
    Sprite *background; // when drawing planet view, this will be the background fill image
    Color   color;
    Uint32  drawSize;   //in pixels
    float   drawScale;  //to scale the drawing
    int     regionsMin,regionsMax;  //amount of regions to be found on a planet
}PlanetData;

typedef struct
{
    Uint32 planetCount;
    SJson *config;
    SJson *planetList;
    PlanetData *planet;
}PlanetManager;

static PlanetManager planet_manager = {0};

void planet_close()
{
    int i;
    slog("freeing planet data");
    for (i = 0;i < planet_manager.planetCount;i++)
    {
        gf2d_sprite_free(planet_manager.planet[i].sprite);
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
    Vector4D color;
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
    planet_manager.planet = gfc_allocate_array(sizeof(PlanetData),count);
    for (i = 0;i < count;i++)
    {
        planetData = sj_array_get_nth(planet_manager.planetList,i);
        if (!planetData)continue;
        planetSprite = sj_get_string_value(sj_object_get_value(planetData,"sprite"));
        if (!planetSprite)continue;
        planet_manager.planet[i].sprite = gf2d_sprite_load_image((char *)planetSprite);
        if (!planet_manager.planet[i].sprite)
        {
            slog("failed to load planet sprite %s",planetSprite);
        }
        planetSprite = sj_get_string_value(sj_object_get_value(planetData,"background"));
        if (!planetSprite)continue;
        planet_manager.planet[i].background = gf2d_sprite_load_image((char *)planetSprite);
        if (!planet_manager.planet[i].background)
        {
            slog("failed to load planet background sprite %s",planetSprite);
        }
        sj_value_as_vector4d(sj_object_get_value(planetData,"colorbase"),&color);
        planet_manager.planet[i].color = gfc_color_hsl(color.x,color.y,color.z,color.w);
        sj_get_float_value(sj_object_get_value(planetData,"drawScale"),&planet_manager.planet[i].drawScale);
        planet_manager.planet[i].drawSize = planet_manager.planet[i].sprite->frame_w * planet_manager.planet[i].drawScale;
        sj_get_integer_value(sj_object_get_value(planetData,"regionMin"),&planet_manager.planet[i].regionsMin);
        sj_get_integer_value(sj_object_get_value(planetData,"regionMax"),&planet_manager.planet[i].regionsMax);
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

void planet_generate_regions(Planet *planet)
{
    int startHeight;
    int i, r, amount = 0;
    int count;
    float regionRange = 0;
    float regionChoice;
    const char *str;
    SJson *planetData;
    SJson *regionData;
    SJson *frequency;
    Vector2D position;
    RegionBiome biome;

    if (!planet)return;
    
    amount = (gfc_random() * (planet_manager.planet[planet->classification].regionsMax - planet_manager.planet[planet->classification].regionsMin)) + planet_manager.planet[planet->classification].regionsMin;
    startHeight = 250 - ((amount / 10) * 100);
    for (i = 0; i < amount;i++)
    {
        position.x = 128 + (i % 5) * 200;
        position.y = startHeight + (i / 5) * 200;
        if ((i / 5) % 2)position.x += 128;
        regionChoice = gfc_random();
        planetData = sj_array_get_nth(planet_manager.planetList,planet->classification);
        count = sj_array_get_count(sj_object_get_value(planetData,"regions"));
        for (r = 0; r < count;r++)
        {
            regionData = sj_array_get_nth(sj_object_get_value(planetData,"regions"),r);
            if (!regionData)continue;
            frequency = sj_object_get_value(regionData,"frequency");
            if (!frequency)slog("NO FREQUENCY FOUND");
            sj_get_float_value(frequency,&regionRange);
            if (regionChoice < regionRange)
            {
                str = sj_get_string_value(sj_object_get_value(regionData,"type"));
                slog("generating region %s",str);
                break;
            }
        }
        biome = region_biome_from_name((char *)str);
        planet->regions = gfc_list_append(planet->regions,region_generate(i,biome,regionChoice , position));
    }
}

Planet *planet_generate(Uint32 *id, int planetType, Uint32 seed, Vector2D position,Vector2D *bottomRight)
{
    int moonCount,i;
    Vector2D newPosition = {0}, childPosition= {0};
    Uint32 childPlanetType;
    Vector2D childBR = {0};
    Vector2D maxBR= {0};
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
    planet->drawSize = planet_manager.planet[planet->classification].drawSize;
    vector2d_copy(newPosition,position);
    srand(*id + seed);

    
    childPlanetType = PC_Moon;// DEFAULT
    childPosition.x = 0;
    childPosition.y = 2;
    if (planet->classification <= PC_Star)
    {
        childPosition.x = 2;
        childPosition.y = 0;
    }
    planet->color = planet_manager.planet[planet->classification].color;
    //    planet->color = gfc_color_hsl(360 * ((float)planet->classification/PC_MAX) - 30,1,0.5,1);
    
    
    planet_generate_regions(planet);
    
    maxBR.x = position.x + (planet_manager.planet[planet->classification].drawSize * 2);
    maxBR.y = position.y + (planet_manager.planet[planet->classification].drawSize * 2);
    if (planet->classification == PC_Moon)
    {
        slog("generating moon %i",planet->id);
        if (bottomRight)
        {
        bottomRight->x = maxBR.x;
        bottomRight->y = maxBR.y;
        }
        return planet;// moons are done
    }
    moonCount = (int)(gfc_random() * 5);
    slog("generating planet %i of type %i with %i moons",planet->id,planet->classification,moonCount);
    //moons!
    newPosition.x = position.x + (planet_manager.planet[planet->classification].drawSize * childPosition.x);
    newPosition.y = position.y + (planet_manager.planet[planet->classification].drawSize * childPosition.y);
    for (i = 0; i < moonCount;i++)
    {
        *id = *id + 1;
        if (planet->classification <= PC_Star)
        {
            childPlanetType = (int)(gfc_random() * (PC_Moon - PC_GasGiant)) + PC_GasGiant;
        }
        planet->children = gfc_list_append(planet->children,planet_generate(id, childPlanetType, seed,newPosition,&childBR));
        if (childBR.x > maxBR.x)maxBR.x = childBR.x;
        if (childBR.y > maxBR.y)maxBR.y = childBR.y;
        if (childPosition.x)newPosition.x = maxBR.x;
        if (childPosition.y)newPosition.y = maxBR.y;
    }
    if (bottomRight)
    {
        bottomRight->x = maxBR.x;
        bottomRight->y = maxBR.y;
    }
    return planet;
}

List *planet_list_get_from_json(SJson *json);
SJson planet_list_to_json(List *planetList);
Planet *planet_get_by_id(List *planetList, Uint32 id);

Planet *planet_load_from_json(SJson *json);
SJson *planet_save_to_json(Planet *planet);


Planet *planet_get_next_child_in_range(Planet *planet, Planet *from,Planet *ignore,Vector2D position,float radius,Uint8 *fromHit)
{
    int i,count;
    Planet *child,*moon;//search item
    if (!planet)
    {
        slog("no planet provided");
        return NULL;
    }
    count = gfc_list_get_count(planet->children);
    if (from == NULL)
    {
        i = 0;
    }
    else
    {
        for (i =0 ; i < count; i++)
        {
            child = gfc_list_get_nth(planet->children,i);
            if (!child)continue;
            if (child == from)
            {
                if (fromHit)*fromHit = 1;
                i++;
                break;
            }
            moon = planet_get_next_child_in_range(child, from,ignore,position,radius,fromHit);
            if (moon != NULL)return moon;
            if ((fromHit)&&(*fromHit == 1))break;
        }
    }
    for (;i < count; i++)
    {
        child = gfc_list_get_nth(planet->children,i);
        if (!child)continue;
        if ((child != ignore) && (vector2d_magnitude_between(child->systemPosition,position) <= radius))
        {
            return child;
        }
        moon = planet_get_next_child_in_range(child, from,ignore,position,radius,NULL);
        if (moon != NULL)return moon;
    }
    return NULL;

}

void planet_draw_system_view_lines(Planet *planet,Vector2D offset)
{
    int i,count;
    Planet *moon;
    Vector2D moonposition;
    Vector2D drawposition;
    if (!planet)return;
    vector2d_add(drawposition,planet->systemPosition,offset);

    count = gfc_list_get_count(planet->children);
    for (i =0 ; i < count; i++)
    {
        moon = gfc_list_get_nth(planet->children,i);
        if (!moon)continue;
        vector2d_add(moonposition,moon->systemPosition,offset);
        gf2d_draw_line(drawposition,moonposition, vector4d(255,255,255,255));
        planet_draw_system_view_lines(moon,offset);
    }
    
}

void planet_draw_system_view(Planet *planet,Vector2D offset)
{
    int i,count;
    Planet *moon;
    Vector4D color;
    Vector2D scale,scalecenter;
    Vector2D drawposition;
    if (!planet)return;
    vector2d_add(drawposition,planet->systemPosition,offset);
    scale = vector2d(
            planet_manager.planet[planet->classification].drawScale,
            planet_manager.planet[planet->classification].drawScale);
    color = gfc_color_to_vector4(planet->color);
    scalecenter.x = planet_manager.planet[planet->classification].sprite->frame_w * 0.5;
    scalecenter.y = planet_manager.planet[planet->classification].sprite->frame_h * 0.5;
    drawposition.x -= (scalecenter.x * scale.x);
    drawposition.y -= (scalecenter.y * scale.y);

    gf2d_sprite_draw(
        planet_manager.planet[planet->classification].sprite,
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

void planet_draw_planet_view(Planet *planet,Vector2D offset)
{
    int i,count;
    Region *region;
    if (!planet)return;
    gf2d_sprite_draw_image(planet_manager.planet[planet->classification].background,vector2d(0,40));
    count = gfc_list_get_count(planet->regions);
    slog("drawing planet type: %i",planet->classification);
    for (i = 0;i < count; i++)
    {
        region = gfc_list_get_nth(planet->regions,i);
        if (!region)continue;
        //slog("drawing region biome: %i",region->biome);
        region_draw_planet_view(region,offset);
    }
}

/*the end of the file*/
