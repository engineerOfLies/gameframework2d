#include <simple_logger.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "systems.h"
#include "planet.h"
#include "galaxy.h"

static char *systemNames[] =
{
    "LHS",
    "Rigel",
    "Omacron",
    "Maaz",
    "Procyon",
    "Falcyon",
    "Halceon",
    "Hyperion",
    "Wasat",
    "Sirius",
    "Cygna",
    "Algol",
    "Rao",
    "Arcturus",
    "Skaedii",
    "Abraxus",
    "Draconis",
    "Indra",
    "Polaris",
    "Centauri",
    "Narnia",
    "Fillori",
    "Regulus",
    "Vulkan",
    "Qo'Nos",
    "Xi Puppies",
    "Pandemonia",
    ""
};

typedef struct
{
    Uint32  systemNameCount;
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
    int i;
    slog("initializing system data");
    system_manager.starSprite = gf2d_sprite_load_image("images/star.png");
    system_manager.background = gf2d_sprite_load_image("images/backgrounds/starsystem.png");
    for (i = 0;strcmp(systemNames[i],"") != 0;i++);
    system_manager.systemNameCount = i;
    slog("we have %i system name roots",system_manager.systemNameCount);
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
    int nameIndex;
    TextLine planetName;
    char Designation = '';
    Vector2D planetPosition = {0};
    Vector2D bottomRight = {0};
    Uint32 i;
    Uint32 starCount;
    Uint32 attempts = 0;
    System *system;
    system = system_new();
    if (!system)return NULL;
    system->id = id;
    
    srand(id + seed);

    nameIndex = (int)(gfc_random() * system_manager.systemNameCount);
    gfc_line_sprintf(system->name,"%s-%i",systemNames[nameIndex],system->id);
    starCount = (int)(gfc_random()*4.0) + 1;
    do
    {
        system->position.x = (gfc_random() * 0.8) + (gfc_random() * 0.1) + (gfc_random() * 0.05);
        system->position.y = (gfc_random() * 0.8) + (gfc_random() * 0.1) + (gfc_random() * 0.05);
        attempts++;
    }while ((attempts < 1000)&&(galaxy_get_next_system_in_range(galaxy, NULL,system,system->position,0.1)!= NULL));
    
    //slog("Generating System %i at (%f,%f) with %i stars", system->id,system->position.x,system->position.y,starCount);
    
    system->size = (starCount * 0.25) + 0.5;
    system->color = gfc_color_hsl((gfc_random() * (10 * system->size) + 30),1,0.5 + (gfc_random() *system->size),1);
    if (starCount > 1)
    {
        Designation = 'A';
    }
    for (i = 0; i < starCount;i++)
    {
        if (starCount > 1)
        {
            gfc_line_sprintf(planetName,"%s %c",system->name,Designation);
            Designation++;
        }
        else
        {
            gfc_line_sprintf(planetName,"%s",system->name);
        }
        system->idPool++;
        system->planets = gfc_list_append(system->planets,planet_generate(&system->idPool, (int)(gfc_random()*PC_GasGiant),id + seed,planetPosition,&bottomRight));
        planetPosition.y = bottomRight.y + 100;
    }
    return system;
}

System *system_load_from_json(SJson *json);
SJson  *system_save_to_json(System *system);

List *system_list_load_from_json(SJson *json);
SJson *system_list_save_to_json(List *systemList);

void system_draw_galaxy_view(System *system,Vector2D offset)
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
    drawposition.x += offset.x;
    drawposition.y += offset.y;
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


Planet *system_get_nearest_planet(System *system,Planet *ignore,Vector2D position,float radius)
{
    Planet *planet = NULL;//search item
    Planet *best = NULL;
    float bestrange = radius * 2;
    float range;
    if (!system)
    {
        slog("no System provided");
        return NULL;
    }
    do
    {
        planet = system_get_next_planet_in_range(system, planet,ignore,position,radius);
        if (!planet)break;   
        range = vector2d_magnitude_between(position,planet->systemPosition);
        if (range < bestrange)
        {
            best = planet;
            bestrange = range;
        }
    }while (planet != NULL);
    return best;
}

Planet *system_get_next_planet_in_range(System *system, Planet *from,Planet *ignore,Vector2D position,float radius)
{
    int i,count;
    Uint8 fromHit = 0;
    Planet *planet;//search item
    Planet *child = NULL;
    if (!system)
    {
        slog("no system provided");
        return NULL;
    }
    count = gfc_list_get_count(system->planets);
    if (from == NULL)
    {
        i = 0;
    }
    else
    {
        for (i =0 ; i < count; i++)
        {
            planet = gfc_list_get_nth(system->planets,i);
            if (!planet)continue;
            if (planet == from)
            {
                i++;
                fromHit = 1;
                break;
            }
            child = planet_get_next_child_in_range(planet, from,ignore,position,radius,&fromHit);
            if (child != NULL)return child;
            if (fromHit)break;
        }
    }
    for (;i < count; i++)
    {
        planet = gfc_list_get_nth(system->planets,i);
        if (!planet)continue;
        if ((planet != ignore) && (vector2d_magnitude_between(planet->systemPosition,position) <= radius))
        {
            return planet;
        }
        child = planet_get_next_child_in_range(planet, from,ignore,position,radius,NULL);
        if (child != NULL)return child;
    }
    return NULL;
}


void system_draw_system_lines(System *system, Vector2D offset)
{
    int i,count;
    Planet* planet;
    if (!system)return;
    count = gfc_list_get_count(system->planets);
//    slog("attempting to draw %i stars for system",count);
    for (i =0 ; i < count; i++)
    {
        planet = gfc_list_get_nth(system->planets,i);
        if (!planet)continue;
        planet_draw_system_view_lines(planet,offset);
    }
}

void system_draw_system_background(System *system, Vector2D offset)
{
    gf2d_sprite_draw_image(system_manager.background,offset);
}

void system_draw_system_view(System *system, Vector2D offset)
{
    int i,count;
    Planet* planet;
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
