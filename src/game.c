#include <SDL.h>
#include <stdio.h>

#include <simple_logger.h>
#include "gfc_input.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_windows.h"
#include "gf2d_entity.h"
#include "gf2d_font.h"
#include "gf2d_mouse.h"
#include "gf2d_draw.h"
#include "gf2d_camera.h"
#include "gf2d_space.h"
#include "gf2d_lighting.h"
#include "gf2d_collision.h"
#include "gf2d_windows_common.h"


void init_all(int argc, char *argv[]);

static int _done = 0;
static Window *_quit = NULL;
int fpsMode = 0;
int editorMode = 0;
extern int __DebugMode;

void onCancel(void *data)
{
    _quit = NULL;
}
void onExit(void *data)
{
    _done = 1;
    _quit = NULL;
}

int main(int argc, char * argv[])
{
    /*variable declarations*/
    Sprite *background = NULL;
    init_all(argc, argv);
    background = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    
    SDL_ShowCursor(SDL_DISABLE);
    // game specific setup

    /*main game loop*/
    while(!_done)
    {
        gfc_input_update();
        /*update things here*/
        gf2d_mouse_update();
        gf2d_windows_update_all();                
        gf2d_entity_think_all();
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
                gf2d_sprite_draw_image(background,vector2d(0,0));
                // DRAW WORLD
                gf2d_entity_update_all();
                gf2d_entity_draw_all();
            //UI elements last
            gf2d_windows_draw_all();
            gf2d_mouse_draw();
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
        if ((gfc_input_command_down("exit"))&&(_quit == NULL))
        {
            _quit = window_yes_no("Exit?",onExit,onCancel,NULL);
        }
        if (fpsMode)slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}

void init_all(int argc, char *argv[])
{
    int i;
    int fullscreen = 0;
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i],"--fullscreen") == 0)
        {
            fullscreen = 1;
        }
        else if (strcmp(argv[i],"--editor") == 0)
        {
            editorMode = 1;
        }
        else if (strcmp(argv[i],"--fps") == 0)
        {
            fpsMode = 1;
        }
        else if (strcmp(argv[i],"--debug") == 0)
        {
            __DebugMode = 1;
        }
    }
    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "Game Framework 2D",
        1280,
        960,
        1280,
        960,
        vector4d(0,0,0,255),
        fullscreen);
    gf2d_graphics_set_frame_delay(16);
    gfc_audio_init(256,16,4,1,1,1);
    gf2d_sprite_init(1024);
    gf2d_actor_init(128);
    gf2d_armature_init(1024);
    gf2d_font_init("config/font.cfg");
    gfc_input_init("config/input.cfg");
    gf2d_windows_init(128,"config/windows.cfg");
    gf2d_lighting_system_init("config/lighting.cfg");
    gf2d_entity_system_init(1024);
    gf2d_figure_init(1024);
    gf2d_camera_set_dimensions(0,0,1280,960);
    gf2d_mouse_load("actors/mouse.actor");
        
    SDL_ShowCursor(SDL_DISABLE);
}

/*eol@eof*/
