#include <SDL_image.h>
#include <stdlib.h>

#include "simple_logger.h"

#include "gfc_shape.h"
#include "gfc_text.h"
#include "gfc_pak.h"

#include "gf2d_graphics.h"

#include "gf2d_sprite.h"

typedef struct
{
    Uint32 max_sprites;
    Sprite * sprite_list;
}SpriteManager;

static SpriteManager sprite_manager = {0};

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
    if (sprite->surface != NULL)
    {
        SDL_FreeSurface(sprite->surface);
    }
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
            memset(&sprite_manager.sprite_list[i],0,sizeof(Sprite));
            sprite_manager.sprite_list[i].ref_count = 1;//set ref count
            return &sprite_manager.sprite_list[i];//return address of this array element        }
        }
    }
    /*find an unused sprite address and clean up the old data*/
    for (i = 0;i < sprite_manager.max_sprites;i++)
    {
        if (sprite_manager.sprite_list[i].ref_count <= 0)
        {
            gf2d_sprite_delete(&sprite_manager.sprite_list[i]);// clean up the old data
            sprite_manager.sprite_list[i].ref_count = 1;//set ref count
            return &sprite_manager.sprite_list[i];//return address of this array element
        }
    }
    slog("error: out of sprite addresses");
    return NULL;
}

Sprite *gf2d_sprite_get_by_filename(const char * filename)
{
    int i;
    if (!filename)
    {
        slog("cannot find blank filename");
        return NULL;
    }
    for (i = 0;i < sprite_manager.max_sprites;i++)
    {
        if (gfc_line_cmp(sprite_manager.sprite_list[i].filepath,filename)==0)
        {
            return &sprite_manager.sprite_list[i];
        }
    }
    return NULL;// not found
}

Sprite *gf2d_sprite_load_image(const char *filename)
{
    return gf2d_sprite_load_all(filename,-1,-1,1,false);
}

Sprite *gf2d_sprite_load_all(
    const char   *filename,
    Sint32  frameWidth,
    Sint32  frameHeight,
    Sint32  framesPerLine,
    Bool    keepSurface
)
{
    SDL_Surface *surface = NULL;
    Sprite *sprite = NULL;
    if (!filename)
    {
        slog("cannot find blank filename");
        return NULL;
    }

    sprite = gf2d_sprite_get_by_filename(filename);
    if (sprite != NULL)
    {
        // found a copy already in memory
        sprite->ref_count++;
        return sprite;
    }
    surface = IMG_Load(filename);
    if (!surface)
    {
        slog("failed to load sprite image %s",filename);
        return NULL;
    }
    sprite = gf2d_sprite_new();
    if (!sprite)
    {
        SDL_FreeSurface(surface);
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

    if(!keepSurface)
    {
        SDL_FreeSurface(surface);
    }
    else
    {
        sprite->surface = surface;
    }
    return sprite;
}

void gf2d_sprite_draw_to_surface(
    Sprite *sprite,
    GFC_Vector2D position,
    GFC_Vector2D * scale,
    GFC_Vector2D * center,
    Uint32 frame,
    SDL_Surface *surface
)
{
    SDL_Rect cell,target;
    int fpl;
    GFC_Vector2D scaleFactor = {1,1};
    GFC_Vector2D scaleOffset = {0,0};
    if (!sprite)
    {
        slog("no sprite provided to draw");
        return;
    }
    if (!sprite->surface)
    {
        slog("sprite does not contain surface to draw with");
        return;
    }
    if (!surface)
    {
        slog("no surface provided to draw to");
        return;
    }
    if (scale)
    {
        gfc_vector2d_copy(scaleFactor,(*scale));
    }
    if (center)
    {
        gfc_vector2d_copy(scaleOffset,(*center));
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
    SDL_BlitScaled(
        sprite->surface,
        &cell,
        surface,
        &target);
}

void gf2d_sprite_draw_image(Sprite *image,GFC_Vector2D position)
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
    GFC_Vector2D position,
    GFC_Vector2D * scale,
    GFC_Vector2D * center,
    float    * rotation,
    GFC_Vector2D * flip,
    GFC_Color    * color,
    Uint32 frame)
{
    gf2d_sprite_render(
        sprite,
        position,
        scale,
        center,
        rotation,
        flip,
        color,
        NULL,
        frame);
}

void gf2d_sprite_render(
    Sprite * sprite,
    GFC_Vector2D position,
    GFC_Vector2D * scale,
    GFC_Vector2D * center,
    float    * rotation,
    GFC_Vector2D * flip,
    GFC_Color    * color,
    GFC_Vector4D * clip,
    Uint32 frame)
{
    float drawRotation = 0;
    GFC_Vector4D colorShift = {1,1,1,1};
    GFC_Vector4D drawClip = {0,0,1,1};
    SDL_Rect cell,target;
    SDL_RendererFlip flipFlags = SDL_FLIP_NONE;
    SDL_Point r = {0,0};
    int fpl;
    GFC_Vector2D scaleFactor = {1,1};
    GFC_Vector2D scaleOffset = {0,0};
    if (!sprite)
    {
        return;
    }
    if (clip)
    {
        gfc_vector4d_copy(drawClip,(*clip));
    }
    if (scale)
    {
        gfc_vector2d_copy(scaleFactor,(*scale));
        if (scale->x < 0)
        {
            if (scale->x)flipFlags |= SDL_FLIP_HORIZONTAL;
            scaleFactor.x *= -1;
        }
        if (scale->y < 0)
        {
            if (scale->y)flipFlags |= SDL_FLIP_VERTICAL;
            scaleFactor.y *= -1;
        }
    }
    if (center)
    {
        gfc_vector2d_copy(scaleOffset,(*center));
        gfc_vector2d_copy(r,(*center));
    }
    if (rotation)
    {
        drawRotation = *rotation;
        gfc_vector2d_copy(r,(scaleOffset));
        r.x *= fabs(scaleFactor.x);
        r.y *= fabs(scaleFactor.y);
    }
    if (flip)
    {
        if (flip->x)flipFlags |= SDL_FLIP_HORIZONTAL;
        if (flip->y)flipFlags |= SDL_FLIP_VERTICAL;
    }
    if (color)
    {
        colorShift = gfc_color_to_vector4(gfc_color_to_int8(*color));
        SDL_SetTextureColorMod(
            sprite->texture,
            colorShift.x,
            colorShift.y,
            colorShift.z);
        SDL_SetTextureAlphaMod(
            sprite->texture,
            colorShift.w);
    }
    
    fpl = (sprite->frames_per_line)?sprite->frames_per_line:1;
    gfc_rect_set(
        cell,
        (frame%fpl * sprite->frame_w) + (drawClip.x * sprite->frame_w),
        (frame/fpl * sprite->frame_h) + (drawClip.y * sprite->frame_h),
        (sprite->frame_w * drawClip.z) - (drawClip.x * sprite->frame_w),
        (sprite->frame_h * drawClip.w) - (drawClip.y * sprite->frame_h));
    gfc_rect_set(
        target,
        position.x - (scaleFactor.x * scaleOffset.x) + (drawClip.x * sprite->frame_w * scaleFactor.x),
        position.y - (scaleFactor.y * scaleOffset.y) + (drawClip.y * sprite->frame_h * scaleFactor.y),
        (sprite->frame_w * scaleFactor.x * drawClip.z) - (drawClip.x * sprite->frame_w * scaleFactor.x),
        (sprite->frame_h * scaleFactor.y * drawClip.w) - (drawClip.y * sprite->frame_h * scaleFactor.y));
    SDL_RenderCopyEx(gf2d_graphics_get_renderer(),
                     sprite->texture,
                     &cell,
                     &target,
                     drawRotation,
                     &r,
                     flipFlags);
    if (color)
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
