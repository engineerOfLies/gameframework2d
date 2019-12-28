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
#include "windows_common.h"
#include "camera.h"
#include "scene.h"
#include "editor.h"

static int _done = 0;
static Window *_quit = NULL;
static int editorMode = 0;

void exitGame()
{
    _done = 1;
}

void init_all(int argc, char *argv[]);

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
    Scene *scene;
    int mx,my;
    
    init_all(argc,argv);
    
    // game specific setup
        // init mouse, editor window
    gf2d_mouse_load("actors/mouse.actor");
    
    if (editorMode)
    {
        editor_menu();
        gf2d_mouse_set_action("pointer");
    }
    else
    {
        gf2d_mouse_set_action("look");
    }

    scene = scene_load("config/testlevel.json");
    scene_camera_focus(scene,vector2d(1280,768));
    /*main game loop*/
    while(!_done)
    {
        gfc_input_update();
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        gf2d_windows_update_all();
                
        gf2d_entity_think_all();
        gf2d_mouse_update();
                
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            scene_draw(scene);
                // DRAW WORLD
                // Draw entities
            //UI elements last
            
            gf2d_windows_draw_all();
            gf2d_mouse_draw();
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
        if ((gfc_input_command_down("exit"))&&(_quit == NULL))
        {
            _quit = window_yes_no("Exit?",onExit,onCancel,NULL,NULL);
        }
   //     slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
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
    gfc_audio_init(256,16,4,1,1,1);
    gf2d_sprite_init(1024);
    gf2d_action_list_init(128);
    gf2d_font_init("config/font.cfg");
    gfc_input_init("config/input.cfg");
    gf2d_windows_init(128);
    gf2d_entity_system_init(1024);
    
    scene_manager_init(10);
    camera_set_dimensions(0,0,1200,700);
    
    SDL_ShowCursor(SDL_DISABLE);
}
/*eol@eof*/
