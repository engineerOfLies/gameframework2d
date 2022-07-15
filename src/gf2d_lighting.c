#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_hashmap.h"

#include "gf2d_graphics.h"
#include "gf2d_lighting.h"

typedef struct
{
    HashMap *lights;
}LightingManager;

static LightingManager lighting_manger = {0};

void gf2d_lighting_system_close()
{
    int i,c;
    Sprite *light;
    List *lights;
    if (!lighting_manger.lights)return;
    lights = gfc_hashmap_get_all_values(lighting_manger.lights);
    if (!lights)return;
    c = gfc_list_get_count(lights);
    for (i = 0;i < c;i++)
    {
        light = gfc_list_get_nth(lights,i);
        if (!light)continue;
        gf2d_sprite_free(light);
    }
    gfc_list_delete(lights);
    gfc_hashmap_free(lighting_manger.lights);
}

HashMap *g2d_lights_parse_file(const char *filename)
{
    int i,c;
    SJson *file;
    SJson *lights;
    SJson *light;
    HashMap *pack;
    const char *name;
    const char *sprite;
    if (!filename)return NULL;
    file = sj_load(filename);
    if (!file)return NULL;
    
    lights = sj_object_get_value(file,"lights");
    if (!lights)
    {
        slog("failed to parse sound pack file, no 'lights' object");
        sj_free(file);
        return NULL;
    }
    pack = gfc_hashmap_new();
    c = sj_array_get_count(lights);
    for (i = 0; i < c; i++)
    {
        light = sj_array_get_nth(lights,i);
        if (!light)continue;
        name = sj_object_get_value_as_string(light,"name");
        sprite = sj_object_get_value_as_string(light,"sprite");
        if ((!name)||(!sprite))continue;
        gfc_hashmap_insert(pack,name,gf2d_sprite_load_image(sprite));
    }
    sj_free(file);
    return pack;
}


void gf2d_lighting_system_init(const char *filename)
{
    if (!filename)
    {
        slog("no filename provided for lights");
        return;
    }
    lighting_manger.lights = g2d_lights_parse_file(filename);
    atexit(gf2d_lighting_system_close);
}

void gf2d_lighting_reset_layer(LightingMask *mask)
{
    if ((!mask)||(!mask->layer))return;
    SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), mask->darkLevel.r, mask->darkLevel.g, mask->darkLevel.b, 255);
    SDL_SetRenderTarget(gf2d_graphics_get_renderer(),mask->layer->texture);
    SDL_RenderClear(gf2d_graphics_get_renderer());
    SDL_SetRenderTarget(gf2d_graphics_get_renderer(),NULL);
}

void gf2d_lighting_set_area_light(LightingMask *mask,Vector2D position,Color color, float size)
{
    Sprite *light;
    Vector2D scale = {size,size};
    Vector2D center;
    if ((!mask)||(!mask->layer))
    {
        slog("cannot update lights, missing mask or layer");
        return;
    }
    light = gfc_hashmap_get(lighting_manger.lights,"area");
    if (!light)
    {
        slog("no area light loaded");
        return;
    }
    color = gfc_color_to_int8(color);
    SDL_SetTextureColorMod(light->texture,color.r,color.g,color.b);
    if (SDL_SetRenderDrawBlendMode(gf2d_graphics_get_renderer(),SDL_BLENDMODE_ADD))
    {
        slog("failed to set light render mode: %s",SDL_GetError());
    }
    SDL_SetRenderTarget(gf2d_graphics_get_renderer(),mask->layer->texture);
    center = vector2d(light->frame_w/2,light->frame_h/2);
    SDL_SetTextureBlendMode(light->texture,SDL_BLENDMODE_ADD);
    gf2d_sprite_draw(
        light,
        vector2d(position.x,position.y),
        &scale,
        &center,
        NULL,
        NULL,
        NULL,
        0);
    SDL_SetRenderTarget(gf2d_graphics_get_renderer(),NULL);
    SDL_SetTextureColorMod(light->texture,255,255,255);
}

void gf2d_lighting_layer_draw(LightingMask *mask,Vector2D offset)
{
    SDL_Rect target;
    if (!mask)return;
    SDL_SetTextureBlendMode(mask->layer->texture,SDL_BLENDMODE_MOD);
    SDL_SetRenderDrawBlendMode(gf2d_graphics_get_renderer(),SDL_BLENDMODE_MOD);
    SDL_SetRenderTarget(gf2d_graphics_get_renderer(),NULL);
    target = gfc_sdl_rect(offset.x,offset.y,mask->layer->frame_w,mask->layer->frame_h);
    SDL_RenderCopy(gf2d_graphics_get_renderer(),
                mask->layer->texture,
                NULL,
                &target);
}

void gf2d_lighting_layer_free(LightingMask *mask)
{
    if (!mask)return;
    gf2d_sprite_free(mask->layer);
    free(mask);
}

LightingMask *gf2d_lighting_layer_new(Vector2D size)
{
    Uint32 clear;
    Uint32 format;
    LightingMask *mask = NULL;
    if ((!size.x)||(!size.y))
    {
        slog("can't make a lighting layer of zero size");
        return NULL;
    }
    mask = gfc_allocate_array(sizeof(LightingMask),1);
    if (!mask)return NULL;
    mask->layer = gf2d_sprite_new();
    if (!mask->layer)
    {
        free(mask);
        return NULL;
    }
    format = gf2d_graphics_get_image_format();
    mask->layer->surface = SDL_CreateRGBSurfaceWithFormat(0,size.x,size.y,32,format);
    clear = SDL_MapRGBA(mask->layer->surface->format,0,0,0,0);
    SDL_FillRect(mask->layer->surface,NULL,clear);
    
    mask->layer->surface = gf2d_graphics_screen_convert(&mask->layer->surface);

    if (!mask->layer->surface)
    {
        slog("failed to convert lightLayer to supported format");
        gf2d_sprite_free(mask->layer);
        free(mask);
        return NULL;
    }
    mask->layer->texture = SDL_CreateTexture(gf2d_graphics_get_renderer(),
                            format,
                            SDL_TEXTUREACCESS_TARGET, 
                            mask->layer->surface->w,
                            mask->layer->surface->h);
    if (!mask->layer->texture)
    {
        slog("failed to convert lighting surface to texture");
        gf2d_sprite_free(mask->layer);
        free(mask);
        return NULL;
    }
    SDL_UpdateTexture(
        mask->layer->texture,
        NULL,
        mask->layer->surface->pixels,
        mask->layer->surface->pitch);
    
    mask->layer->frame_w = mask->layer->surface->w;
    mask->layer->frame_h = mask->layer->surface->h;
    mask->layer->frames_per_line = 1;
    
    gfc_line_cpy(mask->layer->filepath,"lightLayer");
    SDL_SetTextureBlendMode(mask->layer->texture,SDL_BLENDMODE_ADD);
    return mask;
}
