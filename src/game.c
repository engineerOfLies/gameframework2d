#include <SDL.h>
#include "simple_logger.h"

#include "gfc_list.h"
#include "gfc_audio.h"
#include "gfc_pak.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

#include "entity.h"
#include "space_bug.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Sprite *sprite;
    Entity *ent;
    List *sounds;
    
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Color mouseColor = gfc_color8(255,100,255,200);
    
    /*program initializtion*/
    init_logger("gf2d.log",0);
    gfc_pak_manager_init();
    gfc_pak_manager_add("paks/audio.zip");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        0);
    gfc_audio_init(
        1024,
        16,
        5,
        MIX_MAX_VOLUME,
        1,
        1); 
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    entity_manager_init(1024);
    SDL_ShowCursor(SDL_DISABLE);
    
    /*demo setup*/
    sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16,0);
    ent = space_bug_new(vector2d(100,100));
    sounds = gfc_list_new();
    gfc_list_append(sounds,gfc_sound_load("audio/blue.wav",1,5));
    gfc_list_append(sounds,gfc_sound_load("audio/medic.wav",1,5));    
    gfc_list_append(sounds,gfc_sound_load("audio/is_about_to_die.wav",1,5));
    gfc_sound_queue_sequence(sounds,5);
    gfc_list_delete(sounds);
    sounds = gfc_list_new();
    gfc_list_append(sounds,gfc_sound_load("audio/blue.wav",1,5));
    gfc_list_append(sounds,gfc_sound_load("audio/medic.wav",1,5));    
    gfc_list_append(sounds,gfc_sound_load("audio/has_died.wav",1,5));
    gfc_sound_queue_sequence(sounds,5);
    
    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        entity_think_all();
        entity_update_all();
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            gf2d_sprite_draw_image(sprite,vector2d(0,0));
            
            entity_draw_all();
            //UI elements last
            gf2d_sprite_draw(
                mouse,
                vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseColor,
                (int)mf);

        gf2d_graphics_next_frame();// render current draw frame and skip to the next frame
        
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    entity_free(ent);
    slog("---==== END ====---");
    return 0;
}

/*eol@eof*/
