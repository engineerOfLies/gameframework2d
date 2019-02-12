#include <SDL.h>
#include <stdio.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_list.h"
#include "gf2d_input.h"
#include "gf2d_audio.h"
#include "gf2d_windows.h"
#include "gf2d_entity.h"
#include "gf2d_mouse.h"
#include "simple_logger.h"
#include "camera.h"
#include "windows_common.h"

static int _done = 0;
static Window *_quit = NULL;

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
    int i;
    int fullscreen = 0;
    Sprite *background = NULL;
    /*parse args*/
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i],"--fullscreen") == 0)
        {
            fullscreen = 1;
        }
    }
    
    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        fullscreen);
    gf2d_graphics_set_frame_delay(16);
    gf2d_audio_init(256,16,4,1,1,1);
    gf2d_sprite_init(1024);
    gf2d_action_list_init(128);
    gf2d_text_init("config/font.cfg");
    gf2d_input_init("config/input.cfg");
    gf2d_windows_init(128);
    gf2d_entity_system_init(1024);
    
    camera_set_dimensions(0,0,1200,700);
    background = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    
    SDL_ShowCursor(SDL_DISABLE);
    // game specific setup
        // init mouse, editor window
    gf2d_mouse_load("actors/mouse.actor");
    
    /*main game loop*/
    while(!_done)
    {
        gf2d_input_update();
        /*update things here*/
        gf2d_windows_update_all();
                
        gf2d_entity_think_all();
        gf2d_mouse_update();
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
                gf2d_sprite_draw_image(background,vector2d(0,0));
                // DRAW WORLD
                gf2d_entity_update_all();
                // Draw entities
            //UI elements last
            
            gf2d_text_draw_line("Press F4 to quit!",FT_H1,gf2d_color(255,255,255,255), vector2d(0,0));
            gf2d_windows_draw_all();
            gf2d_mouse_draw();
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
        if ((gf2d_input_command_down("exit"))&&(_quit == NULL))
        {
            _quit = window_yes_no("Exit?",onExit,onCancel,NULL,NULL);
        }
   //     slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
