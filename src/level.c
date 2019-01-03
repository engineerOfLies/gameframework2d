#include "level.h"
#include "simple_json.h"
#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_config.h"
#include <stdio.h>

typedef struct
{
    Space      *space;
    Sprite     *backgroundImage;
    Sprite     *tileLayer;
    Mix_Music  *backgroundMusic;
}Level;

static Level gamelevel = {0};

void level_clear()
{
    gf2d_space_free(gamelevel.space);
    gf2d_sprite_free(gamelevel.backgroundImage);
    gf2d_sprite_free(gamelevel.tileLayer);
    if (gamelevel.backgroundMusic)
    {
        Mix_FreeMusic(gamelevel.backgroundMusic);
    }
    memset(&gamelevel,0,sizeof(Level));
    slog("level cleared");
}

void level_info_free(LevelInfo *linfo)
{
    if (!linfo)return;
    if (linfo->tileMap != NULL)
    {
        free(linfo->tileMap);
    }
    //TODO:free spawn list
    free(linfo);
}

LevelInfo *level_info_new()
{
    LevelInfo *linfo = NULL;
    linfo = (LevelInfo*)malloc(sizeof(LevelInfo));
    if (!linfo)
    {
        slog("failed to allocate level info");
        return NULL;
    }
    memset(linfo,0,sizeof(LevelInfo));
    return linfo;
}

void level_info_tilemap_load(LevelInfo *linfo, SJson *tilemap,Uint32 w,Uint32 h)
{
    int i,j;
    SJson *row,*value;
    if ((!linfo)||(!tilemap))
    {
        slog("missing level info or map object");
        return;
    }
    if ((!w) || (!h))
    {
        slog("cannot have a zero width or height for tilemap");
        return;
    }
    linfo->tileMap = (int *)malloc(sizeof(int)*w*h);
    if (!linfo->tileMap)
    {
        slog("failed to allocate tilemap data");
        return;
    }
    for (j = 0; j < h;j++)
    {
        row = sj_array_get_nth(tilemap,j);
        for (i = 0;i < w;i++)
        {
            value = sj_array_get_nth(row,i);
            sj_get_integer_value(value,&linfo->tileMap[j*w+i]);
        }
    }
}

LevelInfo *level_info_load(char *filename)
{
    LevelInfo *linfo = NULL;
    SJson *json,*world;
    if (!filename)return NULL;
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to load level file %s",filename);
        return NULL;
    }
    linfo = level_info_new();
    if (!linfo)
    {
        return NULL;
    }
    world = sj_object_get_value(json,"world");
    if (!world)
    {
        slog("missing world object in level file %s",filename);
        sj_free(json);
        level_info_free(linfo);
        return NULL;
    }
    gf2d_line_cpy(linfo->backgroundImage,sj_get_string_value(sj_object_get_value(world,"backgroundImage")));
    gf2d_line_cpy(linfo->backgroundMusic,sj_get_string_value(sj_object_get_value(world,"backgroundMusic")));
    gf2d_line_cpy(linfo->tileSet,sj_get_string_value(sj_object_get_value(world,"tileSet")));
    
    sj_value_as_vector2d(sj_object_get_value(world,"tileMapSize"),&linfo->tileMapSize);
    slog("loaded tile size of %f,%f",linfo->tileMapSize.x,linfo->tileMapSize.y);
    
    level_info_tilemap_load(linfo, sj_object_get_value(world,"tileMap"),(Uint32)linfo->tileMapSize.x,(Uint32)linfo->tileMapSize.y);

    sj_value_as_vector2d(sj_object_get_value(world,"tileSize"),&linfo->tileSize);
    
    sj_free(json);
    slog("loaded level info for %s",filename);
    return linfo;
}

Sprite *level_make_tile_layer(LevelInfo *linfo,Sprite * tileset, Uint32 format)
{
    Sprite *sprite;
    Uint32 clear;
    int i,j;
    if (!linfo)return NULL;
    sprite = gf2d_sprite_new();
    if (!sprite)return NULL;
    sprite->surface = SDL_CreateRGBSurfaceWithFormat(
        0,
        linfo->tileMapSize.x*linfo->tileSize.x,
        linfo->tileMapSize.y*linfo->tileSize.y,
        32,
        format);

    clear = SDL_MapRGBA(sprite->surface->format,0,0,0,0);
    SDL_FillRect(sprite->surface,NULL,clear);
    
    for (j = 0; j < linfo->tileMapSize.y;j++)
    {
        for (i = 0; i < linfo->tileMapSize.x;i++)
        {
            if (linfo->tileMap[j*(Uint32)linfo->tileMapSize.x + i])
            {
                gf2d_sprite_draw_to_surface(
                tileset,
                vector2d(i*linfo->tileSize.x,j*linfo->tileSize.y),
                NULL,
                NULL,
                linfo->tileMap[j*(Uint32)linfo->tileMapSize.x + i] - 1,
                sprite->surface);
            }
        }
    }

    sprite->surface = gf2d_graphics_screen_convert(&sprite->surface);

    if (sprite->surface)
    {
        sprite->texture = SDL_CreateTextureFromSurface(gf2d_graphics_get_renderer(),sprite->surface);
        if (!sprite->texture)
        {
            slog("failed to convert tileLayer data to texture");
            gf2d_sprite_free(sprite);
            return NULL;
        }
        SDL_SetTextureBlendMode(sprite->texture,SDL_BLENDMODE_BLEND);        
        SDL_UpdateTexture(
            sprite->texture,
            NULL,
            sprite->surface->pixels,
            sprite->surface->pitch);
        
        sprite->frame_w = sprite->surface->w;
        sprite->frame_h = sprite->surface->h;
        sprite->frames_per_line = 1;
        
        gf2d_line_cpy(sprite->filepath,"tileLayer");
    }
    else
    {
        slog("failed to convert tile layer to supported format");
        gf2d_sprite_free(sprite);
    }
    
    return sprite;
}

void level_build_tile_space(LevelInfo *linfo)
{
    int i,j;
    int count = 0;
    for (j = 0;j < linfo->tileMapSize.y;j++)
    {
        for (i = 0; i < linfo->tileMapSize.x;i++)
        {
            if (!linfo->tileMap[j * (Uint32)linfo->tileMapSize.x + i])continue;
            slog("adding static %i",count++);
            gf2d_space_add_static_shape(gamelevel.space,gf2d_shape_rect(i * linfo->tileSize.x, j * linfo->tileSize.y, linfo->tileSize.x, linfo->tileSize.y));
        }
    }
}

void level_init(LevelInfo *linfo)
{
    Sprite *tileset;
    if (!linfo)
    {
        return;
    }
    level_clear();
    gamelevel.backgroundImage = gf2d_sprite_load_image(linfo->backgroundImage);
    tileset = gf2d_sprite_load_all(
        linfo->tileSet,
        linfo->tileSize.x,
        linfo->tileSize.y,
        1,
        true);

    gamelevel.backgroundMusic = Mix_LoadMUS(linfo->backgroundMusic);
    if (gamelevel.backgroundMusic)Mix_PlayMusic(gamelevel.backgroundMusic, -1);
    
    tileset = gf2d_sprite_load_all(
        linfo->tileSet,
        linfo->tileSize.x,
        linfo->tileSize.y,
        1,
        true);
    gamelevel.tileLayer = level_make_tile_layer(linfo,tileset, tileset->surface->format->format);
    gf2d_sprite_free(tileset);

    gamelevel.space = gf2d_space_new_full(
        3,
        gf2d_rect(0,0,gamelevel.tileLayer->surface->w,gamelevel.tileLayer->surface->h),
        0.1,
        vector2d(0,0.098),
        1,
        0.1);
    level_build_tile_space(linfo);
}

void level_draw()
{
    gf2d_sprite_draw_image(gamelevel.backgroundImage,vector2d(0,0));
    gf2d_sprite_draw_image(gamelevel.tileLayer,vector2d(0,0));
    gf2d_space_draw(gamelevel.space);
}

void level_update()
{
    
}

/*eol@eof*/
