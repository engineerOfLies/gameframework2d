
#include "level.h"
#include "gf2d_sprite.h"
#include "gf2d_audio.h"
#include "gf2d_particles.h"

typedef struct
{
    Sprite             *backgroundImage;
    Vector2D            backgroundOffset;
    Mix_Music          *backgroundMusic;
    Color               backgroundColor;
    ParticleEmitter    *pe;
    
}Level;

static Level level;

LevelInfo level_info_load(char *filename)
{
    LevelInfo info;
    memset(&info,0,sizeof(LevelInfo));
    return info;
}


void level_close()
{
    gf2d_sprite_free(level.backgroundImage);
    gf2d_particle_emitter_free(level.pe);
    if (level.backgroundMusic)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(level.backgroundMusic);
    }
}

void level_start(LevelInfo info)
{
//    Mix_Music *Mix_LoadMUS(const char *file)
}

/**
 * @brief update the current level
 */
void level_update()
{
    
}

/**
 * @brief draw the current level
 */
void level_draw()
{
    gf2d_sprite_draw_image(level.backgroundImage,level.backgroundOffset);
}

/*eol@eof*/
