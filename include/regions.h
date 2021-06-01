#ifndef __REGIONS_H__
#define __REGIONS_H__

#include "simple_json.h"

#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_text.h"
#include "gfc_vector.h"

typedef enum
{
    RS_Plotted,
    RS_Construction,
    RS_Complete,
    RS_Damaged,
    RS_Destroyed,
    RS_MAX
}RegionState;

typedef enum
{
    RB_Rocky,
    RB_Desert,
    RB_Ocean,
    RB_Icy,
    RB_Temperate,
    RB_Tropical,
    RB_MAX
}RegionBiome;

typedef enum
{
    ST_Fertility,
    ST_Minerals,
    ST_Habitable,
    ST_MAX
}SurveyType;

typedef enum
{
    SS_Unserveyed,
    SS_Started,
    SS_Underway,
    SS_Completed,
    SS_Max
}SurveyState;

typedef enum
{
    IT_None,
    IT_Population,
    IT_Agriculture,
    IT_Military,
    IT_Mining,
    IT_Research,
    IT_Cultural,
    IT_Travel,
    IT_University,
    IT_MAX
}InstallationType;

typedef struct
{
    Uint32      id;                 /**<unique id for the region*/
    TextLine    name;
    RegionBiome biome;              /**<terrain this region is made up of*/
    float       regionRange;        /**<*/
    Vector2D    drawPosition;       /**<where on the planet to draw this region*/
    float       drawRotation;       /**<each region will be drawn at a custom rotation to provide variety*/
    Uint32      minerals;           /**<mining quality here*/
    Uint32      fertility;          /**<agriculture quality here*/
    Uint32      habitable;          /**<quality of life factor, population limit for settlements placed here*/
    Uint32      installation;       /**<which development, if any has been installed to this region*/
    Uint32      state;              /**<state of development for the installation*/
    Uint32      strength;           /**<generic score for the strength of the development, if any*/
    Uint32      commanderId;        /**<if the installation has a commander, this is their ID*/
}Region;

void regions_init();

RegionBiome region_biome_from_name(char *biomeName);
const char *region_name_from_biome(RegionBiome biome);

Region *region_new();
void    region_free(Region* region);
Region *region_generate(Uint32 id,RegionBiome biome,float regionRange, Vector2D position);
Region *region_load_from_json(SJson *json);
SJson  *region_save_to_json(Region *region);

/**
 * @brief check if the point in question is overlapping the region
 * @param region the region to cehck against
 * @param position the position to check
 * @return 0 on error or no clip.  1 if there is clip
 */
int region_point_check(Region *region,Vector2D position);


List *region_list_load_from_json(SJson *json);
SJson *region_list_save_to_json(List *regionList);

void region_draw_planet_view(Region *region,Vector2D offset);
void region_draw_region_view(Region *region);

#endif
