#ifndef __PLANET_H__
#define __PLANET_H__

#include "simple_json.h"
#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_color.h"
#include "gfc_text.h"


typedef enum
{
    PC_TerniaryStar,
    PC_BinaryStar,
    PC_Star,      //this and above are stars.  
    PC_GasGiant,
    PC_Gaseous,
    PC_Temperate,
    PC_Thin,
    PC_Barren,
    PC_Moon
}PlanetClassification;

typedef struct
{
    TextLine    name;
    Uint32      allegience;         /**<to whom planet is owned*/
    Uint32      id;                 /**<unique id for the planet*/
    Uint32      size;               /**<how large the body is*/
    Color       color;              /**<base planetary color*/
    float       classification;     /**<planetary classification for climate*/
    List       *regions;            /**<list of regions on the planet*/
    List       *children;           /**<child planetary bodies (moons)*/
}Planet;

void planet_list_free(List *list);
List *planet_list_get_from_json(SJson *json);
SJson planet_list_to_json(List *planetList);
Planet *planet_get_by_id(List *planetList, Uint32 id);

Planet *planet_generate(Uint32 id);
Planet *planet_new();
void    planet_free(Planet* planet);

Planet *planet_load_from_json(SJson *json);
SJson *planet_save_to_json(Planet *planet);

void planet_draw_system_view(Planet *planet);
void planet_draw_planet_view(Planet *planet);

#endif
