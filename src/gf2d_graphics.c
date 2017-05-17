#include <SDL.h>

#include "gf2d_graphics.h"
#include "simple_logger.h"

/*local types*/
typedef struct
{
    SDL_Window   *   main_window;
    SDL_Renderer *   renderer;
    SDL_Texture  *   texture;
    SDL_Surface  *   surface;
    SDL_Surface  *   temp_buffer;

    Uint32 frame_delay;
    Uint32 now;
    Uint32 then;
    Bool print_fps;
    float fps; 

    Uint32 background_color;
    Vector4D background_color_v;

    Sint32 bitdepth;
    Uint32 rmask;
    Uint32 gmask;
    Uint32 bmask;
    Uint32 amask;
}Graphics;

/*local gobals*/
static Graphics gf2d_graphics;

/*forward declarations*/
void gf2d_graphics_close();

void gf2d_graphics_initialize(
    char *windowName,
    int viewWidth,
    int viewHeight,
    int renderWidth,
    int renderHeight,
    Vector4D bgcolor,
    Bool fullscreen
)
{
    Uint32 flags = 0;
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        slog("Unable to initilaize SDL system: %s",SDL_GetError());
        return;
    }
    atexit(SDL_Quit);
    if (fullscreen)
    {
        if (renderWidth == 0)
        {
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
        else
        {
            flags |= SDL_WINDOW_FULLSCREEN;
        }
    }
    gf2d_graphics.main_window = SDL_CreateWindow(windowName,
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             renderWidth, renderHeight,
                             flags);

    if (!gf2d_graphics.main_window)
    {
        slog("failed to create main window: %s",SDL_GetError());
        gf2d_graphics_close();
        return;
    }
    
    gf2d_graphics.renderer = SDL_CreateRenderer(gf2d_graphics.main_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (!gf2d_graphics.renderer)
    {
        slog("failed to create renderer: %s",SDL_GetError());
        gf2d_graphics_close();
        return;
    }
    
    SDL_SetRenderDrawColor(gf2d_graphics.renderer, 0, 0, 0, 255);
    SDL_RenderClear(gf2d_graphics.renderer);
    SDL_RenderPresent(gf2d_graphics.renderer);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(gf2d_graphics.renderer, renderWidth, renderHeight);

    gf2d_graphics.texture = SDL_CreateTexture(
        gf2d_graphics.renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        renderWidth, renderHeight);
    if (!gf2d_graphics.texture)
    {
        slog("failed to create screen texture: %s",SDL_GetError());
        gf2d_graphics_close();
        return;
    }
    
    SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888,
                                    &gf2d_graphics.bitdepth,
                                    &gf2d_graphics.rmask,
                                    &gf2d_graphics.gmask,
                                    &gf2d_graphics.bmask,
                                    &gf2d_graphics.amask);

    
    gf2d_graphics.surface = SDL_CreateRGBSurface(0, renderWidth, renderHeight, gf2d_graphics.bitdepth,
                                        gf2d_graphics.rmask,
                                    gf2d_graphics.gmask,
                                    gf2d_graphics.bmask,
                                    gf2d_graphics.amask);
    
    if (!gf2d_graphics.surface)
    {
        slog("failed to create screen surface: %s",SDL_GetError());
        gf2d_graphics_close();
        return;
    }
    
    gf2d_graphics.background_color = SDL_MapRGB(gf2d_graphics.surface->format, bgcolor.x,bgcolor.y,bgcolor.z);
    vector4d_set(gf2d_graphics.background_color_v,bgcolor.x,bgcolor.y,bgcolor.z,bgcolor.w);
    
    srand(SDL_GetTicks());
    atexit(gf2d_graphics_close);
    slog("graphics initialized");
}

void gf2d_graphics_close()
{
    if (gf2d_graphics.texture)
    {
        SDL_DestroyTexture(gf2d_graphics.texture);
    }
    if (gf2d_graphics.renderer)
    {
        SDL_DestroyRenderer(gf2d_graphics.renderer);
    }
    if (gf2d_graphics.main_window)
    {
        SDL_DestroyWindow(gf2d_graphics.main_window);
    }
    if (gf2d_graphics.surface)
    {
        SDL_FreeSurface(gf2d_graphics.surface);
    }
    if (gf2d_graphics.temp_buffer)
    {
        SDL_FreeSurface(gf2d_graphics.temp_buffer);
    }
    gf2d_graphics.surface = NULL;
    gf2d_graphics.main_window = NULL;
    gf2d_graphics.renderer = NULL;
    gf2d_graphics.texture = NULL;
    gf2d_graphics.temp_buffer = NULL;
}

/*eol@eof*/
