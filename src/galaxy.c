#include <simple_logger.h>

#include "gf2d_graphics.h"
#include "gf2d_mouse.h"
#include "gf2d_draw.h"
#include "gf2d_sprite.h"

#include "systems.h"
#include "galaxy.h"

typedef struct
{
    Sprite *background;
}GalaxyManager;

static GalaxyManager galaxy_manager = {0};

void galaxy_close()
{
    slog("freeing galaxy data");
    gf2d_sprite_free(galaxy_manager.background);
    memset(&galaxy_manager,0,sizeof(GalaxyManager));
}

void galaxy_init()
{
    slog("initializing galaxy data");
    galaxy_manager.background = gf2d_sprite_load_image("images/backgrounds/nebulablue.jpg");
    atexit(galaxy_close);
}

Galaxy *galaxy_new()
{
    Galaxy *galaxy;
    galaxy = gfc_allocate_array(sizeof(Galaxy),1);
    if (!galaxy)
    {
        slog("failed to allocate memory for a new galaxy");
        return NULL;
    }
    galaxy->systemList = gfc_list_new();
    if (!galaxy->systemList)
    {
        slog("failed to allocate memory for galactic systems");
        galaxy_free(galaxy);
        return NULL;
    }
    return galaxy;
}

void galaxy_free(Galaxy *galaxy)
{
    int i,count;
    System* system;
    if (!galaxy)return;
    count = gfc_list_get_count(galaxy->systemList);
    for (i =0 ; i < count; i++)
    {
        system = gfc_list_get_nth(galaxy->systemList,i);
        if (!system)continue;
        system_free(system);
    }
    gfc_list_delete(galaxy->systemList);
    galaxy->systemList = NULL;
    free(galaxy);
}

Galaxy *galaxy_generate(Uint32 seed, Uint32 count)
{
    Uint32 i;
    Galaxy *galaxy;
    galaxy = galaxy_new();
    if (!galaxy)return NULL;
    galaxy->seed = seed;
    srand(galaxy->seed);
    slog("generating galaxy with seed (%i) with %i star systems",galaxy->seed,count);
    for (i = 0; i < count;i++)
    {
        galaxy->systemList = gfc_list_append(galaxy->systemList,system_generate(galaxy,++galaxy->idPool,galaxy->seed));
    }
    return galaxy;
}

Galaxy *galaxy_load_from_json(SJson *json);

SJson  *galaxy_save_to_json(Galaxy *galaxy);

void galaxy_draw(Galaxy *galaxy)
{
    System *system;
    int count,i;
    if (!galaxy)return;

    gf2d_sprite_draw_image(galaxy_manager.background,vector2d(-(galaxy_manager.background->frame_w *0.5),-(galaxy_manager.background->frame_h *0.5)));
    count = gfc_list_get_count(galaxy->systemList);
    for (i =0 ; i < count; i++)
    {
        system = gfc_list_get_nth(galaxy->systemList,i);
        if (!system)continue;
        system_draw_galaxy_view(system);
    }
}

Vector2D galaxy_position_to_screen_position(Vector2D position)
{
    Vector2D resolution;
    resolution = gf2d_graphics_get_resolution();
    resolution.x = (position.x * (float)resolution.x);
    resolution.y = (position.y * (float)resolution.y);
    return resolution;
}

Vector2D galaxy_position_from_screen_position(Vector2D position)
{
    Vector2D resolution;
    resolution = gf2d_graphics_get_resolution();
    resolution.x = (position.x / (float)resolution.x);
    resolution.y = (position.y / (float)resolution.y);
    return resolution;
}

System *galaxy_get_nearest_system(Galaxy *galaxy,System *ignore,Vector2D position,float radius)
{
    System *system = NULL;//search item
    System *best = NULL;
    float bestrange = 2;
    float range;
    if (!galaxy)
    {
        slog("no galaxy provided");
        return NULL;
    }
    do
    {
        system = galaxy_get_next_system_in_range(galaxy, system,ignore,position,radius);
        if (!system)break;
        range = vector2d_magnitude_between(position,system->position);
        if (range < bestrange)
        {
            best = system;
            bestrange = range;
        }
    }while (system != NULL);
    return best;
}

System *galaxy_get_next_system_in_range(Galaxy *galaxy, System *from,System *ignore,Vector2D position,float radius)
{
    int i,count;
    System *system;//search item
    if (!galaxy)
    {
        slog("no galaxy provided");
        return NULL;
    }
    count = gfc_list_get_count(galaxy->systemList);
    if (from == NULL)
    {
        i = 0;
    }
    else
    {
        for (i =0 ; i < count; i++)
        {
            system = gfc_list_get_nth(galaxy->systemList,i);
            if (!system)continue;
            if (system == from)
            {
                i++;
                break;
            }
        }
    }
    for (;i < count; i++)
    {
        system = gfc_list_get_nth(galaxy->systemList,i);
        if (!system)continue;
        if (system == ignore)continue;
        if (vector2d_magnitude_between(system->position,position) <= radius)
        {
            return system;
        }
    }
    return NULL;
}

/*eol@eof*/
