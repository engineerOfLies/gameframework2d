#include <simple_logger.h>
#include "systems.h"
#include "planet.h"

/*typedef struct
{
    TextLine    name;
    Vector2D    position;       
    Uint32      id;             
    Uint32      allegience;     
    List       *planets;        
}System;*/

System *system_new()
{
    System *system;
    system = gfc_allocate_array(sizeof(System),1);
    if (!system)
    {
        slog("failed to allocate memory for a new system");
        return NULL;
    }
    system->planets = gfc_list_new();
    if (!system->planets)
    {
        slog("failed to allocate memory for system planets");
        system_free(system);
        return NULL;
    }
    return system;
}


void system_free(System* system)
{
    int i,count;
    Planet* planet;
    if (!system)return;
    count = gfc_list_get_count(system->planets);
    for (i =0 ; i < count; i++)
    {
        planet = gfc_list_get_nth(system->planets,i);
        if (!planet)continue;
        planet_free(planet);
    }
    gfc_list_delete(system->planets);
    system->planets = NULL;
    free(system);
}

System *system_generate(Uint32 id)
{
    System *system;
    system = system_new();
    if (!system)return NULL;
    
    
    
    //TODO: THIS
    
    return system;
}

System *system_load_from_json(SJson *json);
SJson  *system_save_to_json(System *system);

List *system_list_load_from_json(SJson *json);
SJson *system_list_save_to_json(List *systemList);

void system_draw_galaxy_view(System *system);
void system_draw_system_view(System *system);


/*end of the line*/
