#include <simple_logger.h>
#include <simple_json.h>

#include "gfc_list.h"

#include "gf2d_sprite.h"

#include "regions.h"

typedef struct
{
    Sprite *sprite;
}RegionInfo;

typedef struct
{
    SJson *json;        //file handle for cleanup
    SJson *regions;     //handle to the region list
    List *info;
}RegionData;

static RegionData region_data = {0};

void regions_close()
{
    int i,count;
    RegionInfo *info;
    if (region_data.info)
    {
        count = gfc_list_get_count(region_data.info);
        for (i = 0;i < count; i++)
        {
            info = gfc_list_get_nth(region_data.info,i);
            if (!info)continue;
            gf2d_sprite_free(info->sprite);
            free(info);
        }
        gfc_list_delete(region_data.info);
    }
    sj_free(region_data.json);
    memset(&region_data,0,sizeof(RegionData));
}

void regions_init()
{
    int i,count;
    SJson *region;
    RegionInfo *info;
    const char *regionSprite;
    atexit(regions_close);
    slog("initializing region data");
    region_data.json = sj_load("config/regions.cfg");
    if (!region_data.json)
    {
        slog("failed to load region config data!");
        return;
    }
    region_data.regions = sj_object_get_value(region_data.json,"regions");
    if (!region_data.regions)
    {
        slog("region config, missing region info!");
        return;
    }
    region_data.info = gfc_list_new();
    count = sj_array_get_count(region_data.regions);
    for (i = 0;i < count; i++)
    {
        region = sj_array_get_nth(region_data.regions,i);
        if (!region)continue;
        info = gfc_allocate_array(sizeof(RegionInfo),1);
        if (!info)continue;
        regionSprite = sj_get_string_value(sj_object_get_value(region,"sprite"));
        if (!regionSprite)continue;
        info->sprite = gf2d_sprite_load_image((char *)regionSprite);
        if (!info->sprite)
        {
            slog("failed to load region sprite %s",regionSprite);
        }
        region_data.info = gfc_list_append(region_data.info,info);
    }
}

Region *region_new()
{
    Region *region;
    region = gfc_allocate_array(sizeof(Region),1);
    if (!region)
    {
        slog("failed to allocate memory for a new region");
        return NULL;
    }
    return region;
}


void region_free(Region* region)
{
    if (!region)return;
    free(region);
}

RegionBiome region_biome_from_name(char *biomeName)
{
    if (gfc_stricmp(biomeName,"rocky")==0)
        return RB_Rocky;
    if (gfc_stricmp(biomeName,"dessert")==0)
        return RB_Desert;
    if (gfc_stricmp(biomeName,"ocean")==0)
        return RB_Ocean;
    if (gfc_stricmp(biomeName,"icy")==0)
        return RB_Icy;
    if (gfc_stricmp(biomeName,"temperate")==0)
        return RB_Temperate;
    if (gfc_stricmp(biomeName,"tropical")==0)
        return RB_Tropical;
    return RB_MAX;
}

Region *region_generate(Uint32 id,RegionBiome biome,float regionRange, Vector2D position)
{
    Region *region;
    RegionInfo *info;
    SJson *json;
    int     tempInt = 0;

    if (biome == RB_MAX)return NULL;
    region = region_new();
    if (!region)
    {
        slog("faile to allocate new region information");
        return NULL;
    }
    json = sj_array_get_nth(region_data.regions,(int)biome);
    if (!json)
    {
        slog("no region config data for biome %i",biome);
        return NULL;
    }
    info = gfc_list_get_nth(region_data.info,biome);
    if (!info)
    {
        slog("no region info for biome %i",biome);
        return NULL;
    }
    
    region->id = id;
    region->biome = biome;
    region->regionRange = regionRange;
    vector2d_copy(region->drawPosition,position);
    
    region->drawRotation = 360 * gfc_random();
    sj_get_integer_value(sj_object_get_value(json,"minerals"),&tempInt);
    region->minerals = (Uint32)(gfc_random() * tempInt);

    sj_get_integer_value(sj_object_get_value(json,"fertility"),&tempInt);
    region->fertility = (Uint32)(gfc_random() * tempInt);

    sj_get_integer_value(sj_object_get_value(json,"habitable"),&tempInt);
    region->habitable = (Uint32)(gfc_random() * tempInt);
    
    return region;
}

void region_draw_planet_view(Region *region,Vector2D offset)
{
    RegionInfo *info;
    Vector2D drawposition;
    if (!region)return;

    info = gfc_list_get_nth(region_data.info,region->biome);
    if (!info)
    {
        slog("no region info for biome %i",region->biome);
        return;
    }
    slog("regionRange for region: %f, biome %i",region->regionRange,region->biome);
    vector2d_add(drawposition,region->drawPosition,offset);
    if (!info->sprite)return;
    
    drawposition.x -= (info->sprite->frame_w * 0.5);
    drawposition.y -= (info->sprite->frame_h * 0.5);

    gf2d_sprite_draw(
        info->sprite,
        drawposition,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        0);
}


Region *region_load_from_json(SJson *json);
SJson  *region_save_to_json(Region *region);

List *region_list_load_from_json(SJson *json);
SJson *region_list_save_to_json(List *regionList);

void region_draw_region_view(Region *region);

/*END OF THE FILE*/
