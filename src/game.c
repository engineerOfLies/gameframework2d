#include <SDL.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_list.h"
#include "gf2d_audio.h"
#include "gf2d_windows.h"
#include "simple_logger.h"
#include "gf2d_config.h"
#include "gf2d_mouse.h"

int main_menu_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    if (!win)return 0;
    if (!updateList)return 0;
    count = gf2d_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gf2d_list_get_nth(updateList,i);
        if (!e)continue;
        slog("updated element index %i",e->index);
        switch(e->index)
        {
            case 51:
                slog("ok");
                break;
            case 52:
                slog("cancel");
                break;
        }
    }
    return 0;
}

void main_menu()
{
    Window *win;
    SJson *json = NULL;
    json = sj_load("config/testwindow.cfg");
    win = gf2f_window_load_from_json(json);
    if (win)
    {
        win->update = main_menu_update;
    }
    sj_free(json);
}

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Sprite *sprite;
    
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
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_audio_init(256,16,4,1,1,1);
    gf2d_sprite_init(1024);
    gf2d_action_list_init(128);
    gf2d_text_init("config/font.cfg");
    gf2d_windows_init(128);
    SDL_ShowCursor(SDL_DISABLE);
    
    /*demo setup*/
    sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    gf2d_mouse_load("actors/mouse.actor");
    main_menu();
    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        gf2d_mouse_update();
        gf2d_windows_update_all();
        
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            gf2d_sprite_draw_image(sprite,vector2d(0,0));
            
            //UI elements last
            gf2d_windows_draw_all();
            gf2d_mouse_draw();
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
   //     slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
