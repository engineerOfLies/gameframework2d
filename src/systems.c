#include <simple_logger.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
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

Sprite *starSprite = NULL;


void system_init()
{
    starSprite = gf2d_sprite_load_image("images/star.png");
}

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

System *system_generate(Uint32 id, Uint32 seed)
{
    Uint32 i;
    Uint32 starCount;
    System *system;
    system = system_new();
    if (!system)return NULL;
    system->id = id;
    
    srand(id + seed);

    starCount = (int)(gfc_random()*4.0) + 1;
    
    system->position.x = gfc_random();
    system->position.y = gfc_random();
    
    slog("Generating System %i at (%f,%f) with %i stars", system->id,system->position.x,system->position.y,starCount);
    
    for (i = 0; i < starCount;i++)
    {
        system->idPool++;
        planet_generate(&system->idPool, (int)(gfc_random()*PC_GasGiant),id + seed);
    }
    return system;
}

System *system_load_from_json(SJson *json);
SJson  *system_save_to_json(System *system);

List *system_list_load_from_json(SJson *json);
SJson *system_list_save_to_json(List *systemList);

void system_draw_galaxy_view(System *system)
{
    Vector2D scale;
    Vector2D resolution;
    if (!system)return;
    resolution = gf2d_graphics_get_resolution();
    resolution.x = (system->position.x * (float)resolution.x);
    resolution.y = (system->position.y * (float)resolution.y);
    scale = vector2d(0.5,0.5);
    gf2d_sprite_draw(
        starSprite,
        resolution,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        0);
        
}

void system_draw_system_view(System *system);


/*end of the line*/
