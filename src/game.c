#include <SDL.h>
#include "level.h"
#include "camera.h"
#include "items.h"
#include "gui.h"
#include "gf2d_audio.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "simple_logger.h"
#include "gf2d_particles.h"
#include "gf2d_actor.h"
#include "gf2d_entity.h"
#include "gf2d_draw.h"
#include "gf2d_collision.h"
#include "particle_effects.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    LevelInfo *level;
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
    gf2d_sprite_init(1024);
    gf2d_text_init("config/font.cfg");
    gui_setup_hud();
    gf2d_audio_init(256,16,4,1,1,1);
    item_manager_init();

    camera_set_dimensions(0,0,1200,720);// matches screen resolution
    
    SDL_ShowCursor(SDL_DISABLE);
    gf2d_action_list_init(200);
    gf2d_entity_system_init(2048);
    
    level = level_info_load("levels/testlevel.lvl");

    /*demo setup*/
    level_start(level);
    level_info_free(level);
    item_load_list("config/itemlist.cfg");
    pe_load_assets();
    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/

        level_update();
        gf2d_entity_think_all();
        gf2d_entity_update_all();
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            level_draw();
            // game entities next            
            gf2d_entity_draw_all();
            
            //UI elements last
            gui_draw_hud();
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
//        slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    // any custome clean up goes here
    level_close();
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
