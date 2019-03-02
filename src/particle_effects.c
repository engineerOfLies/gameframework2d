#include "particle_effects.h"

#include "gf2d_particles.h"
#include "level.h"
#include "simple_logger.h"

void particle_spray(Vector2D position, Vector2D direction,Color color, Uint32 count)
{
    int i;
    Vector2D dir;
    for (i = 0; i < count; i++)
    {
        dir = vector2d_rotate(direction, gf2d_crandom()*0.5);
        vector2d_scale(dir,dir,gf2d_random());
        gf2d_particle_new_full(
            level_get_particle_emitter(),
            500,
            NULL,
            NULL,
            position,
            dir,
            vector2d(0,0.038),
            color,
            gf2d_color(0,0,0,0),
            PT_Pixel,
            0,
            0,
            0,
            0,
            SDL_BLENDMODE_BLEND);
    }
}

void particle_trail(Vector2D start, Vector2D end,Color color)
{
    Shape s;
    s = gf2d_shape_edge(start.x,start.y,end.x,end.y);
    gf2d_particle_new_full(
        level_get_particle_emitter(),
        500,
        NULL,
        &s,
        vector2d(0,0),
        vector2d(0,0),
        vector2d(0,0),
        color,
        gf2d_color(0,0,0,0),
        PT_Shape,
        0,
        0,
        0,
        0,
        SDL_BLENDMODE_BLEND);
}

/*eol@eof*/
