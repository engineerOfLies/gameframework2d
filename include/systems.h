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
    Uint32      allegience;     /**<to whom the system is owned*/
    List       *planets;        /**<first one is always a star*/
}System;

System *system_new();
void    system_free(System* system);
System *system_generate(Uint32 id);
System *system_load_from_json(SJson *json);
SJson  *system_save_to_json(System *system);

List *system_list_load_from_json(SJson *json);
SJson *system_list_save_to_json(List *systemList);

void system_draw_galaxy_view(System *system);
void system_draw_system_view(System *system);

#endif
