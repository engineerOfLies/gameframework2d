#include <SDL.h>
#include "gf2d_graphics.h"
#include "simple_logger.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    
    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1024,
        768,
        1024,
        768,
        vector4d(0,0,0,255),
        0);

    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        
        slog_sync();/*makes sure log file is updated*/
        
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
    }
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
