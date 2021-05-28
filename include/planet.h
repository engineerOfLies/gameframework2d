#ifndef __PLANET_H__
#define __PLANET_H__

#include "simple_json.h"
#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_color.h"
#include "gfc_text.h"
#include "gfc_vector.h"
#include "regions.h"


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
    PC_Moon,
    PC_MAX
}PlanetClassification;

typedef struct
{
    TextLine    name;
    Vector2D    systemPosition;     /**<where to draw in the system view*/
    Uint32      allegience;         /**<to whom planet is owned*/
    Uint32      id;                 /**<unique id for the planet*/
    Uint32      size;               /**<how large the body is*/
    Vector2D    area;       // how much draw area is needed for planet view
    Uint32      drawSize;           /**<in pixels*/
    Color       color;              /**<base planetary color*/
    Uint32      classification;     /**<planetary classification for climate*/
    List       *regions;            /**<list of regions on the planet*/
    List       *children;           /**<child planetary bodies (moons)*/
}Planet;

void planet_init();

void planet_list_free(List *list);
List *planet_list_get_from_json(SJson *json);
SJson planet_list_to_json(List *planetList);
Planet *planet_get_by_id(List *planetList, Uint32 id);

Planet *planet_generate(Uint32 *id, int planetType, Uint32 seed, Vector2D position,Vector2D *bottomRight,char *name);
Planet *planet_new();
void    planet_free(Planet* planet);

Planet *planet_load_from_json(SJson *json);
SJson *planet_save_to_json(Planet *planet);

/**
 * @brief get a region based on a position
 * @param planet the planet to check
 * @param position the position to check
 * @return NULL on error or no regions, a pointer to the region otherwise
 */
Region *planet_get_region_by_position(Planet *planet,Vector2D position);
Region *planet_get_next_region(Planet *planet, Region *from,Region *ignore);
Planet *planet_get_next_child_in_range(Planet *planet, Planet *from,Planet *ignore,Vector2D position,float radius,Uint8 *fromHit);
void planet_draw_system_view(Planet *planet,Vector2D offset);
void planet_draw_system_view_lines(Planet *planet,Vector2D offset);
void planet_draw_planet_view(Planet *planet,Vector2D offset);

#endif
