#ifndef __SYSTEMS_H__
#define __SYSTEMS_H__

#include "simple_json.h"

#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_text.h"
#include "planet.h"

/*
 * For each star system, determine center planet
 * 
 * For each star of a certain size determine a number of planets - 0 - 10, some of which MIGHT be smaller stars
 * 
 * For each planet determine a number of moons (0 - 10)
 * 
 */

typedef struct
{
    TextLine    name;
    Vector2D    position;       /**<relative to the galactic center*/
    Uint32      id;             /**<unique system ID*/
    float       size;           /**<scale for drawing for the galactic view*/
    Color       color;          /**<color to shift it to when drawing*/
    Uint32      idPool;
    Uint32      allegience;     /**<to whom the system is owned*/
    List       *planets;        /**<first one is always a star*/
}System;

/**
 * @brief load internal accounting for stars
 */
void system_init();

System *system_new();

void    system_free(System* system);

System *system_load_from_json(SJson *json);
SJson  *system_save_to_json(System *system);

List *system_list_load_from_json(SJson *json);
SJson *system_list_save_to_json(List *systemList);

Planet *system_get_nearest_planet(System *system,Planet *ignore,Vector2D position,float radius);
Planet *system_get_next_planet_in_range(System *system, Planet *from,Planet *ignore,Vector2D position,float radius);

void system_draw_system_background(System *system, Vector2D offset);
void system_draw_galaxy_view(System *system,Vector2D offset,float scale);
void system_draw_system_view(System *system, Vector2D offset);
void system_draw_system_lines(System *system, Vector2D offset);

#endif
