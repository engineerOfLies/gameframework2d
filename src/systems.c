#include <simple_logger.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "systems.h"
#include "planet.h"
#include "galaxy.h"

typedef struct
{
    Sprite *starSprite;
    Sprite *background;
}SystemManager;

static SystemManager system_manager = {0};


void system_close()
{
    slog("freeing system data");
    gf2d_sprite_free(system_manager.background);
    memset(&system_manager,0,sizeof(SystemManager));
}

void system_init()
{
    slog("initializing system data");
    system_manager.starSprite = gf2d_sprite_load_image("images/star.png");
    system_manager.background = gf2d_sprite_load_image("images/backgrounds/starsystem.png");
    atexit(system_close);
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

System *system_generate(Galaxy *galaxy, Uint32 id, Uint32 seed)
{
    Vector2D planetPosition = {0};
    Uint32 i;
    Uint32 starCount;
    Uint32 attempts = 0;
    System *system;
    system = system_new();
    if (!system)return NULL;
    system->id = id;
    
    srand(id + seed);

    starCount = (int)(gfc_random()*4.0) + 1;
    do
    {
        system->position.x = (gfc_random() * 0.8) + (gfc_random() * 0.1) + (gfc_random() * 0.05);
        system->position.y = (gfc_random() * 0.8) + (gfc_random() * 0.1) + (gfc_random() * 0.05);
        attempts++;
    }while ((attempts < 1000)&&(galaxy_get_next_system_in_range(galaxy, NULL,system,system->position,0.1)!= NULL));
    
    slog("Generating System %i at (%f,%f) with %i stars", system->id,system->position.x,system->position.y,starCount);
    
    system->size = (starCount * 0.25) + 0.5;
    system->color = gfc_color_hsl((gfc_random() * (10 * system->size) + 30),1,0.5 + (gfc_random() *system->size),1);
    for (i = 0; i < starCount;i++)
    {
        system->idPool++;
        system->planets = gfc_list_append(system->planets,planet_generate(&system->idPool, (int)(gfc_random()*PC_GasGiant),id + seed,planetPosition));
        planetPosition.y += 256;
    }
    return system;
}

System *system_load_from_json(SJson *json);
SJson  *system_save_to_json(System *system);

List *system_list_load_from_json(SJson *json);
SJson *system_list_save_to_json(List *systemList);

void system_draw_galaxy_view(System *system)
{
    Vector2D scale,scalecenter;
    Vector2D drawposition;
    Vector4D color;
    if (!system)return;
    if (system_manager.starSprite == NULL)return;
    drawposition = galaxy_position_to_screen_position(system->position);
    scale = vector2d(system->size,system->size);
    color = gfc_color_to_vector4(system->color);
    scalecenter.x = system_manager.starSprite->frame_w * 0.5;
    scalecenter.y = system_manager.starSprite->frame_h * 0.5;
    drawposition.x -= (scalecenter.x * scale.x);
    drawposition.y -= (scalecenter.y * scale.y);
    gf2d_sprite_draw(
        system_manager.starSprite,
        drawposition,
        &scale,
        NULL,
        NULL,
        NULL,
        &color,
        0);
        
}

void system_draw_system_view(System *system, Vector2D offset)
{
    int i,count;
    Planet* planet;
    gf2d_sprite_draw_image(system_manager.background,vector2d(0,0));
    if (!system)return;
    count = gfc_list_get_count(system->planets);
//    slog("attempting to draw %i stars for system",count);
    for (i =0 ; i < count; i++)
    {
        planet = gfc_list_get_nth(system->planets,i);
        if (!planet)continue;
        planet_draw_system_view(planet,offset);
    }

}


/*end of the line*/
