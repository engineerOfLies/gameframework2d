#include <SDL.h>

#include "simple_logger.h"

#include "gfc_input.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

#include "camera.h"
#include "entity.h"
#include "player.h"
#include "monster.h"
#include "level.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    Level *level;
    int done = 0;
    int i;
    const Uint8 * keys;
    
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    GFC_Color mouseGFC_Color = gfc_color8(255,100,255,200);
    
    /*program initializtion*/
    init_logger("gf2d.log",0);
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        gfc_vector4d(0,0,0,255),
        0);
    camera_set_dimension(gfc_vector2d(1200,720));
    gfc_input_init("config/input.cfg");
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    entity_manager_init(10024);
    SDL_ShowCursor(SDL_DISABLE);
    
    /*demo setup*/
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16,0);
    slog("press [escape] to quit");
    
    level = level_load("levels/testlevel.level");
    level_bake_tiles(level);
    player_entity_new(gfc_vector2d(100,100));
    for (i = 0; i < 10;i++)
    {
        monster_new(gfc_vector2d(gfc_random() * level->size.x,gfc_random()*level->size.y));
    }
    /*main game loop*/
    while(!done)
    {
        gfc_input_update();
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        
        entity_manager_think_all();
        entity_manager_update_all();
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            level_draw(level);
            entity_manager_draw_all();
            entity_draw(player_entity_get());
            //UI elements last
            gf2d_sprite_draw(
                mouse,
                gfc_vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseGFC_Color,
                (int)mf);

        gf2d_graphics_next_frame();// render current draw frame and skip to the next frame
        
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        //slog("Rendering at %g FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
