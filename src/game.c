#include <SDL.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "simple_logger.h"
#include "gf2d_particles.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Sprite *sprite;
    
    int mx,my;
    float mf = 0;
    ParticleEmitter *pe;
    Sprite *mouse;
    Sprite *ship;
    Sprite *bug;
    Shape shape;
    Vector2D flipHorizontal = {1,0};
    Vector4D mouseColor = {255,100,255,200};
    
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
    SDL_ShowCursor(SDL_DISABLE);
    
    /*demo setup*/
    sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16);
    ship = gf2d_sprite_load_all("images/ed210.png",128,128,16);
    bug = gf2d_sprite_load_all("images/space_bug.png",128,128,16);
    
    shape = gf2d_shape_circle(0,0,8);
//    shape = gf2d_shape_rect(-8, -8, 16, 16);
//    shape = gf2d_shape_edge(-5,-5,5,5);
    pe = gf2d_particle_emitter_new_full(
        500000,
        100,
        5,
        PT_Shape,
        vector2d(575,340),
        vector2d(2,2),
        vector2d(0,-3),
        vector2d(2,1),
        vector2d(0,0.05),
        vector2d(0,0.01),
        gf2d_color(0.85,0.55,0,1),
        gf2d_color(-0.01,-0.02,0,0),
        gf2d_color(0.1,0.1,0,0.1),
        &shape,
        0,
        0,
        0,
        "images/cloud.png",
        32,
        32,
        1,
        0,
//        SDL_BLENDMODE_BLEND);
        SDL_BLENDMODE_ADD);
    
    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;

        gf2d_particle_emitter_update(pe);        
        gf2d_particle_new_default(pe,20);

        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            gf2d_sprite_draw_image(sprite,vector2d(0,0));
            
            // game entities next
            gf2d_particle_emitter_draw(pe);
            gf2d_sprite_draw(
                ship,
                vector2d(64,570),
                NULL,
                NULL,
                NULL,
                &flipHorizontal,
                NULL,
                90+(int)mf);
            
            gf2d_sprite_draw(
                bug,
                vector2d(1024,570),
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                88+(int)mf);
            
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
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    // any custome clean up goes here
    gf2d_particle_emitter_free(pe);
    
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
