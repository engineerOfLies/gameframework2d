#include <SDL2/SDL.h>
#include "simple_logger.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

#ifdef __SWITCH__
#include <switch.h>
#include <unistd.h>
#endif

#ifdef __GAMECUBE__
#include <ogcsys.h>
#include <gccore.h>
#include "bg_flat_png.h"
#include "pointer_png.h"
#endif

int main(int argc, char * argv[])
{
#ifdef __SWITCH__
    romfsInit();
    chdir("romfs:/");
#endif

#ifdef __GAMECUBE__
    PAD_Init();
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
#elif __GAMECUBE__
    SYS_STDIO_Report(true);
    PAD_Init();
#else
    init_logger("gf2d.log",0);
#endif

    slog("---==== BEGIN ====---");
#ifdef __GAMECUBE__
    gf2d_graphics_initialize("gf2d", 640, 480, 640, 480, vector4d(0,0,0,255), 0);
#else
    gf2d_graphics_initialize("gf2d", 1200, 720, 1200, 720, vector4d(0,0,0,255), 0);
#endif
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    SDL_ShowCursor(SDL_DISABLE);
    
#if defined(__SWITCH__)
    SDL_JoystickEventState(SDL_ENABLE);
    SDL_Joystick* joystick = SDL_JoystickOpen(0);
#endif

#ifdef __GAMECUBE__
    sprite = gf2d_sprite_load_image_mem("images/backgrounds/bg_flat.png", bg_flat_png, bg_flat_png_size);
    mouse = gf2d_sprite_load_all_mem("images/pointer.png", pointer_png, pointer_png_size,32,32,16,0);
#else
    /*demo setup*/
    sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16,0);
#endif


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

#if defined(__SWITCH__)
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
#elif defined(__GAMECUBE__)
        if(PAD_StickX(0) > 0)
        {
            mx+=4;
        }
        else if(PAD_StickX(0) < 0)
        {
           mx-=4;
        }
        
        if(PAD_StickY(0) < 0)
        {
            my+=4;
        }
        else if(PAD_StickY(0) > 0)
        {
            my-=4;
        }
#else
        SDL_GetMouseState(&mx,&my);
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
