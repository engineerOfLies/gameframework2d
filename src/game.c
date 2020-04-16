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
#include "main_menu.h"
#include "items.h"

static int _done = 0;
static int _begin = 0;
static Window *_quit = NULL;
int editorMode = 0;
extern int __DebugMode;


void init_all(int argc, char *argv[]);

void onCancel(void *data)
{
    _quit = NULL;
}

void beginGame()
{
    _begin = 1;
}

void onExit(void *data)
{
    _begin = 0;
    _done = 1;
    _quit = NULL;
}

void exitGame()
{
    _done = 1;
}

void exitCheck()
{
    if (_quit)return;
    _quit = window_yes_no("GTFO?",onExit,onCancel,NULL,NULL);
}

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int windowsUpdated = 0;
        
    init_all(argc,argv);
    
    
    // game specific setup
        // init mouse, editor window
    gf2d_mouse_load("actors/mouse.json");
    gf2d_mouse_set_function(MF_Pointer);
    
    if (editorMode)
    {
        editor_menu();
    }
    else
    {
        main_menu();
    }
    
    item_list_load("config/item_list.json");
    
    
    /*main game loop*/
    while(!_done)
    {
        gfc_input_update();
        gf2d_mouse_update();
        /*update things here*/
        windowsUpdated = gf2d_windows_update_all();
                
        gf2d_entity_update_all();
        
        gf2d_entity_think_all();

        if ((!editorMode)&&(!windowsUpdated)&&(_begin))
        {
            gf2d_mouse_scene_update();
            scene_update(scene_get_active());
        }
                
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
                // DRAW WORLD
            if ((!editorMode)&&(_begin))
            {   
                scene_draw(scene_get_active());
            }
            //UI elements last
            gf2d_windows_draw_all();
            gf2d_mouse_draw();
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
        if ((gfc_input_command_down("exit"))&&(_quit == NULL))
        {
            _quit = window_yes_no("GTFO?",onExit,onCancel,NULL,NULL);
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
        else if (strcmp(argv[i],"--debug") == 0)
        {
            __DebugMode = 1;
        }
    }
    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "Legacy of the Necromancer",
        1366,
        768,
        1366,
        768,
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
    camera_set_dimensions(0,0,1366,768);
    
    SDL_ShowCursor(SDL_DISABLE);
}
/*eol@eof*/
