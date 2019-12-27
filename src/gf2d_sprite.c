#include <SDL_image.h>
#include <stdlib.h>

#include "simple_logger.h"
#include "gfc_text.h"

#include "gf2d_graphics.h"

#include "gf2d_sprite.h"

typedef struct
{
    Uint32 max_sprites;
    Sprite * sprite_list;
}SpriteManager;

static SpriteManager sprite_manager;

void gf2d_sprite_close()
{
    gf2d_sprite_clear_all();
    if (sprite_manager.sprite_list != NULL)
    {
        free(sprite_manager.sprite_list);
    }
    sprite_manager.sprite_list = NULL;
    sprite_manager.max_sprites = 0;
    slog("sprite system closed");
}

void gf2d_sprite_init(Uint32 max)
{
    if (!max)
    {
        slog("cannot intialize a sprite manager for Zero sprites!");
        return;
    }
    sprite_manager.max_sprites = max;
    sprite_manager.sprite_list = (Sprite *)malloc(sizeof(Sprite)*max);
    memset (sprite_manager.sprite_list,0,sizeof(Sprite)*max);
    if (!(IMG_Init( IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        slog("failed to init image: %s",SDL_GetError());
    }
    slog("sprite system initialized");
    atexit(IMG_Quit);
    atexit(gf2d_sprite_close);
}

void gf2d_sprite_delete(Sprite *sprite)
{
    if (!sprite)return;
    if (sprite->texture != NULL)
    {
        SDL_DestroyTexture(sprite->texture);
    }    
    memset(sprite,0,sizeof(Sprite));//clean up all other data
}

void gf2d_sprite_free(Sprite *sprite)
{
    if (!sprite) return;
    sprite->ref_count--;
}

void gf2d_sprite_clear_all()
{
    int i;
    for (i = 0;i < sprite_manager.max_sprites;i++)
    {
        gf2d_sprite_delete(&sprite_manager.sprite_list[i]);// clean up the data
    }
}

Sprite *gf2d_sprite_new()
{
    int i;
    /*search for an unused sprite address*/
    for (i = 0;i < sprite_manager.max_sprites;i++)
    {
        if ((sprite_manager.sprite_list[i].ref_count == 0)&&(sprite_manager.sprite_list[i].texture == NULL))
        {
            sprite_manager.sprite_list[i].ref_count = 1;//set ref count
            return &sprite_manager.sprite_list[i];//return address of this array element        }
        }
    }
    /*find an unused sprite address and clean up the old data*/
    for (i = 0;i < sprite_manager.max_sprites;i++)
    {
        if (sprite_manager.sprite_list[i].ref_count == 0)
        {
            gf2d_sprite_delete(&sprite_manager.sprite_list[i]);// clean up the old data
            sprite_manager.sprite_list[i].ref_count = 1;//set ref count
            return &sprite_manager.sprite_list[i];//return address of this array element
        }
    }
    slog("error: out of sprite addresses");
    return NULL;
}

Sprite *gf2d_sprite_get_by_filename(char * filename)
{
    int i;
    for (i = 0;i < sprite_manager.max_sprites;i++)
    {
        if (gfc_line_cmp(sprite_manager.sprite_list[i].filepath,filename)==0)
        {
            return &sprite_manager.sprite_list[i];
        }
    }
    return NULL;// not found
}

Sprite *gf2d_sprite_load_image(char *filename)
{
    return gf2d_sprite_load_all(filename,-1,-1,1);
}

Sprite *gf2d_sprite_load_all(
    char *filename,
    Sint32 frameWidth,
    Sint32 frameHeight,
    Sint32 framesPerLine
)
{
    SDL_Surface *surface = NULL;
    Sprite *sprite = NULL;
    
    sprite = gf2d_sprite_get_by_filename(filename);
    if (sprite != NULL)
    {
        // found a copy already in memory
        sprite->ref_count++;
        return sprite;
    }
    
    sprite = gf2d_sprite_new();
    if (!sprite)
    {
        return NULL;
    }
    
    surface = IMG_Load(filename);
    if (!surface)
    {
        slog("failed to load sprite image %s",filename);
        gf2d_sprite_free(sprite);
        return NULL;
    }

    surface = gf2d_graphics_screen_convert(&surface);
    if (!surface)
    {
        slog("failed to load sprite image %s",filename);
        gf2d_sprite_free(sprite);
        return NULL;
    }
    
    sprite->texture = SDL_CreateTextureFromSurface(gf2d_graphics_get_renderer(),surface);
    if (!sprite->texture)
    {
        slog("failed to load sprite image %s",filename);
        gf2d_sprite_free(sprite);
        SDL_FreeSurface(surface);
        return NULL;
    }
    SDL_SetTextureBlendMode(sprite->texture,SDL_BLENDMODE_BLEND);        
    SDL_UpdateTexture(sprite->texture,
                    NULL,
                    surface->pixels,
                    surface->pitch);
    if (frameHeight == -1)
    {
        sprite->frame_h = surface->h;
    }
    else sprite->frame_h = frameHeight;
    if (frameWidth == -1)
    {
        sprite->frame_w = surface->w;
    }
    else sprite->frame_w = frameWidth;
    sprite->frames_per_line = framesPerLine;
    gfc_line_cpy(sprite->filepath,filename);

    SDL_FreeSurface(surface);
    return sprite;
}

void gf2d_sprite_draw_image(Sprite *image,Vector2D position)
{
    gf2d_sprite_draw(
        image,
        position,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        0);
}

void gf2d_sprite_draw(
    Sprite * sprite,
    Vector2D position,
    Vector2D * scale,
    Vector2D * scaleCenter,
    Vector3D * rotation,
    Vector2D * flip,
    Vector4D * colorShift,
    Uint32 frame)
{
    SDL_Rect cell,target;
    SDL_RendererFlip flipFlags = SDL_FLIP_NONE;
    SDL_Point r;
    int fpl;
    Vector2D scaleFactor = {1,1};
    Vector2D scaleOffset = {0,0};
    if (!sprite)
    {
        return;
    }
    
    if (scale)
    {
        vector2d_copy(scaleFactor,(*scale));
    }
    if (scaleCenter)
    {
        vector2d_copy(scaleOffset,(*scaleCenter));
    }
    if (rotation)
    {
        vector2d_copy(r,(*rotation));
        r.x *= scaleFactor.x;
        r.y *= scaleFactor.y;
    }
    if (flip)
    {
        if (flip->x)flipFlags |= SDL_FLIP_HORIZONTAL;
        if (flip->y)flipFlags |= SDL_FLIP_VERTICAL;
    }
    if (colorShift)
    {
        SDL_SetTextureColorMod(
            sprite->texture,
            colorShift->x,
            colorShift->y,
            colorShift->z);
        SDL_SetTextureAlphaMod(
            sprite->texture,
            colorShift->w);
    }
    
    fpl = (sprite->frames_per_line)?sprite->frames_per_line:1;
    gfc_rect_set(
        cell,
        frame%fpl * sprite->frame_w,
        frame/fpl * sprite->frame_h,
        sprite->frame_w,
        sprite->frame_h);
    gfc_rect_set(
        target,
        position.x - (scaleFactor.x * scaleOffset.x),
        position.y - (scaleFactor.y * scaleOffset.y),
        sprite->frame_w * scaleFactor.x,
        sprite->frame_h * scaleFactor.y);
    SDL_RenderCopyEx(gf2d_graphics_get_renderer(),
                     sprite->texture,
                     &cell,
                     &target,
                     rotation?rotation->z:0,
                     rotation?&r:NULL,
                     flipFlags);
    if (colorShift)
    {
        SDL_SetTextureColorMod(
            sprite->texture,
            255,
            255,
            255);
        SDL_SetTextureAlphaMod(
            sprite->texture,
            255);
    }
}

/*eol@eof*/
