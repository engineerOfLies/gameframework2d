#include <SDL2/SDL.h>
#include "simple_logger.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#ifdef __SWITCH__
#include <switch.h>
#include <unistd.h>
#endif

int main(int argc, char * argv[])
{
#ifdef __SWITCH__
    romfsInit();
    chdir("romfs:/");
#endif

    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Sprite *sprite;
    
    int mx = 0, my = 0;
    float mf = 0;
    Sprite *mouse;
    Color mouseColor = gfc_color8(255,100,255,200);
    
    /*program initializtion*/

#ifdef __SWITCH__
    init_logger("sdmc:/gf2d.log",0);
#else
    init_logger("gf2d.log",0);
#endif

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
    
#ifdef __SWITCH__
    SDL_JoystickEventState(SDL_ENABLE);
    SDL_Joystick* joystick = SDL_JoystickOpen(0);
#endif

    /*demo setup*/
    sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16,0);
    /*main game loop*/
#ifdef __SWITCH__
    while(!done && appletMainLoop())
#else
    while(!done)
#endif
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
#ifndef __SWITCH__
        SDL_GetMouseState(&mx,&my);
#else
        if(SDL_JoystickGetAxis(joystick, 0) > 0)
        {
            mx+=4;
        }
        else if(SDL_JoystickGetAxis(joystick, 0) < 0)
        {
            mx-=4;
        }
        
        if(SDL_JoystickGetAxis(joystick, 1) > 0)
        {
            my+=4;
        }
        else if(SDL_JoystickGetAxis(joystick, 1) < 0)
        {
            my-=4;
        }
#endif
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            gf2d_sprite_draw_image(sprite,vector2d(0,0));
            
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
    slog("---==== END ====---");
#ifdef __SWITCH__
    romfsExit();
#endif
    return 0;
}
/*eol@eof*/
