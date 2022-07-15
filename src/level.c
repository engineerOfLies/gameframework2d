#include <stdio.h>

#include "simple_json.h"
#include "simple_logger.h"

#include "gfc_config.h"

#include "gf2d_graphics.h"
#include "gf2d_particles.h"
#include "gf2d_draw.h"
#include "gf2d_mouse.h"
#include "gf2d_camera.h"
#include "gf2d_lighting.h"

#include "entity_common.h"
#include "player.h"
#include "spawn.h"
#include "level.h"

extern int __DebugMode;
extern int editorMode;
#define DRAWMODE 1

typedef struct
{
    Space           *space;
    List            *backgroundImages;
    Sprite          *tileLayer;
    LightingMask    *lightMask;
    Sprite          *darkLayer;
    Sprite          *waterLayer;
    Sprite          *tileShadow;
    Actor           *light;
    Actor           *tileset;
    Mix_Music       *backgroundMusic;
    ParticleEmitter *pe;
    Uint8            dark;
    LevelInfo       *info;
    List            *water;//rects containing water of some type
}Level;

static Level gamelevel = {0};

int *level_alloc_tilemap(int w,int h);

void level_clear()
{
    Sprite *image;
    int i,c;
    Rect *waterRect;
    gf2d_space_free(gamelevel.space);
    c = gfc_list_get_count(gamelevel.backgroundImages);
    for (i = 0; i < c; i++)
    {
        image = gfc_list_get_nth(gamelevel.backgroundImages,i);
        if (!image)continue;
        gf2d_sprite_free(image);
    }
    gfc_list_delete(gamelevel.backgroundImages);
    if (gamelevel.water)
    {
        c = gfc_list_get_count(gamelevel.water);
        for (i = 0; i < c; i++)
        {
            waterRect = gfc_list_get_nth(gamelevel.water,i);
            if (!waterRect)continue;
            free(waterRect);
        }
        gfc_list_delete(gamelevel.water);
    }
    gf2d_actor_free(gamelevel.tileset);
    gf2d_actor_free(gamelevel.light);
    gf2d_sprite_free(gamelevel.tileLayer);
    gf2d_lighting_layer_free(gamelevel.lightMask);
    gf2d_sprite_free(gamelevel.darkLayer);
    gf2d_sprite_free(gamelevel.tileShadow);
    gf2d_sprite_free(gamelevel.waterLayer);
    gf2d_particle_emitter_free(gamelevel.pe);
    if (gamelevel.backgroundMusic)
    {
        Mix_FreeMusic(gamelevel.backgroundMusic);
    }
    memset(&gamelevel,0,sizeof(Level));
}

Vector2D level_get_tile_size()
{
    Vector2D size = {0};
    if ((!gamelevel.tileset)||(!gamelevel.tileset->sprite))
        return size;
    size.x = gamelevel.tileset->sprite->frame_w;
    size.y = gamelevel.tileset->sprite->frame_h;
    return size;
}
Vector2D level_get_size()
{
    Vector2D size = {0};
    if (!gamelevel.tileLayer)
        return size;
    size.x = gamelevel.tileLayer->frame_w;
    size.y = gamelevel.tileLayer->frame_h;
    return size;
}

Vector2D level_get_camera_diff()
{
    Vector2D diff,extent,levelsize;
    extent = gf2d_camera_get_size();
    levelsize = level_get_size();
    vector2d_sub(diff,levelsize,extent);// difference between level and camera    
    return diff;
}


void level_info_free(LevelInfo *linfo)
{
    int i,c;
    char *name;
    if (!linfo)return;
    if (linfo->tileMap != NULL)
    {
        free(linfo->tileMap);
    }
    sj_free(linfo->spawnList);
    sj_free(linfo->args);
    c = gfc_list_get_count(linfo->backgroundImages);
    for (i = 0;i < c; i++)
    {
        name = gfc_list_get_nth(linfo->backgroundImages,i);
        if (!name)continue;
        free(name);
    }
    gfc_list_delete(linfo->backgroundImages);
    free(linfo);
}

const char* level_info_get_tileset(LevelInfo *linfo)
{
    if (!linfo)return NULL;
    return linfo->tileSet;
}

void level_info_set_tileset(LevelInfo *linfo,const char *tileSet)
{
    if (!linfo)return;
    if (!tileSet)return;
    gf2d_actor_free(gamelevel.tileset);
    gfc_line_cpy(linfo->tileSet,tileSet);
    gamelevel.tileset = gf2d_actor_load(linfo->tileSet);
    level_make_tile_layer(linfo);
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
    linfo->backgroundImages = gfc_list_new();
    linfo->spawnList = sj_array_new();
    linfo->args = sj_object_new();
    linfo->idPool = 1;// player is always 0
    return linfo;
}

LevelInfo *level_info_create(
    const char *backgroundImage,
    const char *backgroundMusic,
    const char *tileSet,
    Vector2D    tileMapSize
)
{
    LevelInfo *linfo = NULL;
    linfo = level_info_new();
    if (!linfo)return NULL;
    level_info_add_background(linfo,backgroundImage);
    gfc_line_cpy(linfo->backgroundMusic,backgroundMusic);
    gfc_line_cpy(linfo->tileSet,tileSet);
    vector2d_copy(linfo->tileMapSize,tileMapSize);
    linfo->tileMap = level_alloc_tilemap(tileMapSize.x,tileMapSize.y);
    return linfo;
}

int *level_alloc_tilemap(int w,int h)
{
    int *tileMap;
    if ((!w) || (!h))
    {
        slog("cannot have a zero width or height for tilemap");
        return NULL;
    }
    tileMap = (int *)malloc(sizeof(int)*w*h);
    if (!tileMap)
    {
        slog("failed to allocate tilemap data");
        return NULL;
    }
    memset(tileMap,0,sizeof(int)*w*h);
    return tileMap;
}

SJson *level_info_tilemap_to_json(LevelInfo *linfo)
{
    int r,c,i;
    SJson *rows,*columns;
    if ((!linfo)||(!linfo->tileMap))return NULL;
    rows = sj_array_new();
    if (!rows)return NULL;
    for (r = 0,i = 0;r < linfo->tileMapSize.y;r++)
    {
        columns = sj_array_new();
        for (c = 0; c < linfo->tileMapSize.x;c++,i++)
        {
            sj_array_append(columns,sj_new_int(linfo->tileMap[i]));
        }
        sj_array_append(rows,columns);
    }
    return rows;
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
    linfo->tileMap = level_alloc_tilemap(w,h);
    if (!linfo->tileMap)
    {
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

SJson *level_to_json(LevelInfo *linfo)
{
    int i,c;
    SJson *json;
    SJson *world;
    SJson *backgroundImages;
    const char *name;
    if (!linfo)return NULL;
    json = sj_object_new();
    if (!json)return NULL;
    world = sj_object_new();
    if (!world)
    {
        sj_free(json);
        return NULL;
    }
    sj_object_insert(json,"world",world);
    backgroundImages = sj_array_new();
    c = gfc_list_get_count(linfo->backgroundImages);
    for (i = 0; i < c; i++)
    {
        name = gfc_list_get_nth(linfo->backgroundImages,i);
        if (!name)continue;
        sj_array_append(backgroundImages,sj_new_str(name));
    }
    sj_object_insert(world,"backgroundImages",backgroundImages);
    sj_object_insert(world,"backgroundMusic",sj_new_str(linfo->backgroundMusic));
    sj_object_insert(world,"tileSet",sj_new_str(linfo->tileSet));
    sj_object_insert(world,"idPool",sj_new_int(linfo->idPool));
    sj_object_insert(world,"tileMapSize",sj_vector2d_new(linfo->tileMapSize));
    sj_object_insert(world,"tileMap",level_info_tilemap_to_json(linfo));
    sj_object_insert(world,"spawnList",sj_copy(linfo->spawnList));
    sj_object_insert(world,"args",sj_copy(linfo->args));
    return json;
}

void level_save_to_file(LevelInfo *linfo,const char *filename)
{
    SJson *json;
    if (!linfo)return;
    if (!filename)return;
    json = level_to_json(linfo);
    if (!json)return;
    sj_save(json,(char *)filename);
    sj_free(json);
}

LevelInfo *level_info_load(char *filename)
{
    int i,c;
    TextLine filepath;
    Vector3D level = {0};
    LevelInfo *linfo = NULL;
    char *imageName;
    SJson *json,*world,*dark,*backgroundImages,*image;
    if (!filename)return NULL;
    if (strncmp(filename,"levels/",7)!= 0)
    {
        snprintf(filepath,GFCLINELEN,"levels/%s",filename);
    }
    else
    {
        gfc_line_cpy(filepath,filename);
    }

    json = sj_load(filepath);
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
    backgroundImages = sj_object_get_value(world,"backgroundImages");
    c = sj_array_get_count(backgroundImages);
    for (i = 0; i < c; i++)
    {
        image = sj_array_get_nth(backgroundImages,i);
        if (!image)continue;
        imageName = gfc_allocate_array(sizeof(TextLine),1);
        gfc_line_cpy(imageName,sj_get_string_value(image));
        linfo->backgroundImages = gfc_list_append(linfo->backgroundImages,imageName);
    }
    gfc_line_cpy(linfo->backgroundMusic,sj_get_string_value(sj_object_get_value(world,"backgroundMusic")));
    gfc_line_cpy(linfo->tileSet,sj_get_string_value(sj_object_get_value(world,"tileSet")));
    gfc_line_cpy(linfo->filename,filename);
    
    sj_get_integer_value(sj_object_get_value(world,"idPool"),(int *)&linfo->idPool);
    
    sj_value_as_vector2d(sj_object_get_value(world,"tileMapSize"),&linfo->tileMapSize);
    
    level_info_tilemap_load(linfo, sj_object_get_value(world,"tileMap"),(Uint32)linfo->tileMapSize.x,(Uint32)linfo->tileMapSize.y);

    if (linfo->spawnList)sj_free(linfo->spawnList);
    linfo->spawnList = sj_copy(sj_object_get_value(world,"spawnList"));
    if (linfo->args)sj_free(linfo->args);
    linfo->args = sj_copy(sj_object_get_value(world,"args"));
    
    dark = sj_object_get_value(linfo->args,"dark");
    if (dark)
    {
        linfo->dark = true;
        sj_value_as_vector3d(dark,&level);
        linfo->darkLevel = gfc_color_from_vector4(vector4d(level.x,level.y,level.z,255));

//        slog("dark level color: %f,%f,%f",level.x,level.y,level.z);
    }
    sj_free(json);
//    slog("loaded level info for %s",filename);
    return linfo;
}

void level_make_space()
{
    gamelevel.space = gf2d_space_new_full(
        3,
        gfc_rect(0,0,gamelevel.tileLayer->surface->w,gamelevel.tileLayer->surface->h),
        0.01,
        vector2d(0,0),
        1,
        1,//slop
        1,//use hash or not
        vector2d(128,128));
}

//  THEDARKNESS

void level_reset_darkness()
{
    if (!gamelevel.lightMask)return;//can't reset an empty sprite
    gf2d_lighting_reset_layer(gamelevel.lightMask);
}

void level_set_light_at(Vector2D position,Color color, float size)
{
    gf2d_lighting_set_area_light(gamelevel.lightMask,position,color,size);
}

void level_darkness_draw(Vector2D offset)
{
    gf2d_lighting_layer_draw(gamelevel.lightMask,offset);
}

void level_make_light_layer(LevelInfo *linfo)
{
    if (!linfo)
    {
        slog("no level info provided");
        return;
    }
    if ((!linfo->levelBounds.x)||(!linfo->levelBounds.y))
    {
        slog("cannot make dark / light layer, zero sized level bounds");
        return;
    }
    gamelevel.lightMask = gf2d_lighting_layer_new(vector2d(linfo->levelBounds.x,linfo->levelBounds.y));
    if (!gamelevel.lightMask)return;
    gamelevel.lightMask->darkLevel = linfo->darkLevel;
}


void level_make_tile_layer(LevelInfo *linfo)
{
    Sprite *sprite;
    Uint32 clear;
    Uint32 format;
    int i,j;
    if (!linfo)
    {
        slog("no level info provided");
        return;
    }
    if (!gamelevel.tileset)
    {
        slog("no tileset loaded for level");
        return;
    }
    format = gamelevel.tileset->sprite->surface->format->format;
    sprite = gf2d_sprite_new();
    if (!sprite)return;
    if (gamelevel.tileLayer != NULL)
    {
        // if one already exists, clear it
        gf2d_sprite_delete(gamelevel.tileLayer);
        gamelevel.tileLayer = NULL;
    }

    linfo->levelBounds.x = linfo->tileMapSize.x*gamelevel.tileset->sprite->frame_w;
    linfo->levelBounds.y = linfo->tileMapSize.y*gamelevel.tileset->sprite->frame_h;
    sprite->surface = SDL_CreateRGBSurfaceWithFormat(
        0,
        linfo->levelBounds.x,
        linfo->levelBounds.y,
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
                gamelevel.tileset->sprite,
                vector2d(i*gamelevel.tileset->sprite->frame_w,j*gamelevel.tileset->sprite->frame_h),
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
        sprite->texture = SDL_CreateTexture(gf2d_graphics_get_renderer(),
                                format,
                                SDL_TEXTUREACCESS_TARGET, 
                                sprite->surface->w,
                                sprite->surface->h);
        if (!sprite->texture)
        {
            slog("failed to convert tileLayer data to texture");
            gf2d_sprite_free(sprite);
            return;
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
        
        gfc_line_cpy(sprite->filepath,"tileLayer");
        gamelevel.tileLayer = sprite;
    }
    else
    {
        slog("failed to convert tile layer to supported format");
        gf2d_sprite_free(sprite);
    }
}

Sprite *level_make_layer(LevelInfo *linfo)
{
    Sprite *sprite;
    Uint32 clear;
    Uint32 format;
    Vector2D size;
    if (!linfo)
    {
        slog("no level info provided");
        return NULL;
    }
    if (!gamelevel.tileset)
    {
        slog("no tiles loaded to make a layer for");
        return NULL;
    }
    sprite = gf2d_sprite_new();
    if (!sprite)
    {
        slog("no new sprites available");
        return NULL;
    }
    format = gf2d_graphics_get_image_format();
    size.x = linfo->tileMapSize.x*gamelevel.tileset->sprite->frame_w;
    size.y = linfo->tileMapSize.y*gamelevel.tileset->sprite->frame_h;
    sprite->surface = SDL_CreateRGBSurfaceWithFormat(
        0,
        size.x,
        size.y,
        32,
        format);
    clear = SDL_MapRGBA(sprite->surface->format,0,0,0,0);
    SDL_FillRect(sprite->surface,NULL,clear);
    sprite->surface = gf2d_graphics_screen_convert(&sprite->surface);
    if (sprite->surface)
    {
        sprite->texture = SDL_CreateTexture(gf2d_graphics_get_renderer(),
                                format,
                                SDL_TEXTUREACCESS_TARGET, 
                                sprite->surface->w,
                                sprite->surface->h);

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
        return sprite;
    }
    slog("failed to convert layer sprite to screen format");
    gf2d_sprite_free(sprite);
    return NULL;
}

int level_tile_hidden(LevelInfo *linfo,Uint32 x,Uint32 y)
{
    int count = 0;
    if (!linfo)return -1;
//    if (!linfo->tileMap[y * (Uint32)linfo->tileMapSize.x + x])return 0;// empty tile, not hidden
    if ((x == 0)&&(y == 0))// top left corner, only need to check the three neighbors.
    {
        count += linfo->tileMap[(y * (Uint32)linfo->tileMapSize.x) + x + 1]?1:0;
        count += linfo->tileMap[((y + 1) * (Uint32)linfo->tileMapSize.x) + x]?1:0;
        count += linfo->tileMap[((y + 1) * (Uint32)linfo->tileMapSize.x) + x + 1]?1:0;
        if (count == 3)return 1;
        return 0;
    }
    if ((x == (linfo->tileMapSize.x - 1))&&(y == 0))// top right corner, only need to check the three neighbors.
    {
        count += linfo->tileMap[(y * (Uint32)linfo->tileMapSize.x) + x - 1]?1:0;
        count += linfo->tileMap[((y + 1) * (Uint32)linfo->tileMapSize.x) + x]?1:0;
        count += linfo->tileMap[((y + 1) * (Uint32)linfo->tileMapSize.x) + x - 1]?1:0;
        if (count == 3)return 1;
        return 0;
    }
    if ((x == 0)&&(y == (linfo->tileMapSize.y - 1)))// bottom left corner, only need to check the three neighbors.
    {
        count += linfo->tileMap[y * (Uint32)linfo->tileMapSize.x + x + 1]?1:0;
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x]?1:0;
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x + 1]?1:0;
        if (count == 3)return 1;
        return 0;
    }
    if ((x == (linfo->tileMapSize.x - 1))&&(y == (linfo->tileMapSize.y - 1)))// bottom right corner
    {
        count += linfo->tileMap[y * (Uint32)linfo->tileMapSize.x + x - 1]?1:0;
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x]?1:0;
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x - 1]?1:0;
        if (count == 3)return 1;
        return 0;
    }
    if (x == 0)//left side
    {
        count = 0;
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x]?1:0;//U
        count += linfo->tileMap[y * (Uint32)linfo->tileMapSize.x + x + 1]?1:0;//R
        count += linfo->tileMap[(y + 1) * (Uint32)linfo->tileMapSize.x + x]?1:0;//D
        count += linfo->tileMap[(y + 1) * (Uint32)linfo->tileMapSize.x + x + 1]?1:0;//UR
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x + 1]?1:0;//DR
        if (count == 5)return 1;
        return 0;
    }
    else if (y == 0)//top
    {
        count += linfo->tileMap[y * (Uint32)linfo->tileMapSize.x + x - 1]?1:0;//L
        count += linfo->tileMap[y * (Uint32)linfo->tileMapSize.x + x + 1]?1:0;//R
        count += linfo->tileMap[(y + 1) * (Uint32)linfo->tileMapSize.x + x]?1:0;//D
        count += linfo->tileMap[(y + 1) * (Uint32)linfo->tileMapSize.x + x + 1]?1:0;///DR
        count += linfo->tileMap[(y + 1) * (Uint32)linfo->tileMapSize.x + x - 1]?1:0;//DL
        if (count == 5)return 1;
        return 0;
    }
    else if (x == (linfo->tileMapSize.x - 1))//right
    {
        count += linfo->tileMap[y * (Uint32)linfo->tileMapSize.x + x - 1]?1:0;//L
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x]?1:0;//U
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x - 1]?1:0;//LU
        count += linfo->tileMap[(y + 1) * (Uint32)linfo->tileMapSize.x + x]?1:0;//D
        count += linfo->tileMap[(y + 1) * (Uint32)linfo->tileMapSize.x + x - 1]?1:0;//DL
        if (count == 5)return 1;
        return 0;
    }
    else if (y == (linfo->tileMapSize.y - 1))//bottom
    {
        count += linfo->tileMap[y * (Uint32)linfo->tileMapSize.x + x - 1]?1:0;
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x]?1:0;
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x - 1]?1:0;
        count += linfo->tileMap[y * (Uint32)linfo->tileMapSize.x + x + 1]?1:0;
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x + 1]?1:0;
        if (count == 5)return 1;
        return 0;
    }
    else
    {
        count += linfo->tileMap[y * (Uint32)linfo->tileMapSize.x + x - 1]?1:0;
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x]?1:0;
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x - 1]?1:0;
        count += linfo->tileMap[y * (Uint32)linfo->tileMapSize.x + x + 1]?1:0;
        count += linfo->tileMap[(y + 1) * (Uint32)linfo->tileMapSize.x + x]?1:0;
        count += linfo->tileMap[(y + 1) * (Uint32)linfo->tileMapSize.x + x + 1]?1:0;
        count += linfo->tileMap[(y - 1) * (Uint32)linfo->tileMapSize.x + x + 1]?1:0;
        count += linfo->tileMap[(y + 1) * (Uint32)linfo->tileMapSize.x + x - 1]?1:0;
        if (count == 8)return 1;
        return 0;
    }
    return 0;
}

void level_water_draw_rect(Rect rect,Actor *actor,float frame)
{
    int i,j;
    int drawFrame;
    if (!actor)return;
    for (j = 0;j < rect.h;j++)
    {
        for (i = 0; i < rect.w;i++)
        {
            if (j == 0)
            {
                drawFrame = frame + (i % 2);
            }
            else if (j < 2)
            {
                drawFrame = frame + (i % 2) + (j % 2) * 2;
            }
            else
            {
                drawFrame = frame + (i % 2) + (j % 2)* 2 + 4;
            }
            gf2d_actor_draw(
                actor,
                drawFrame,
                vector2d(((i + rect.x)* actor->size.x),((j + rect.y)* actor->size.y)),
                NULL,
                NULL,
                NULL,
                NULL,
                NULL);
        }
    }
}

int level_water_check(Vector2D position)
{
    Rect *waterRect;
    int i,c;
    c = gfc_list_get_count(gamelevel.water);
    vector2d_scale(position,position,1/32.0);
    for (i = 0; i < c; i++)
    {
        waterRect = gfc_list_get_nth(gamelevel.water,i);
        if (!waterRect)continue;
        if (gfc_point_in_rect(position,*waterRect))return 1;
    }
    return 0;
}

void level_make_water_layer(LevelInfo *linfo)
{
    Actor *waterActor;
    Sprite *sprite;
    SJson *water_rects,*rect;
    Vector4D r;
    Rect *waterRect;
    const char *water_type = NULL;
    float frame;
    int i,c;
    if (!linfo)
    {
        slog("no level info provided");
        return;
    }
    water_rects = sj_object_get_value(linfo->args,"water_rect");
    if (!water_rects)return;// nothing to do
    water_type = sj_get_string_value(sj_object_get_value(linfo->args,"water_type"));
    if (!water_type)return;
    waterActor = gf2d_actor_load("actors/water.actor");
    if (!waterActor)
    {
        slog("failed to load water actor");
        return;//nothing to do
    }
    if (!gf2d_actor_set_action(waterActor, water_type,&frame))
    {
        slog("failed to set water_type to %s",water_type);
        gf2d_actor_free(waterActor);
        return;
    }
    sprite = level_make_layer(linfo);
    if (!sprite)
    {
        slog("failed to make waterLayer");
        gf2d_actor_free(waterActor);
        return;
    }
    if (gamelevel.waterLayer != NULL)
    {
        // if one already exists, clear it
        gf2d_sprite_delete(gamelevel.waterLayer);
    }
    gamelevel.waterLayer = sprite;
    SDL_SetRenderTarget(gf2d_graphics_get_renderer(),
        gamelevel.waterLayer->texture);
    gamelevel.water = gfc_list_new();
    if (!sj_is_array(water_rects))
    {
        sj_value_as_vector4d(water_rects,&r);
        waterRect = gfc_allocate_array(sizeof(Rect),1);
        *waterRect = gfc_rect_from_vector4(r);
        level_water_draw_rect(*waterRect,waterActor,frame);
        gamelevel.water = gfc_list_append(gamelevel.water,waterRect);
    }
    else
    {
        c = sj_array_get_count(water_rects);
        for (i = 0; i < c; i++)
        {
            rect = sj_array_get_nth(water_rects,i);
            if (!rect)continue;
            sj_value_as_vector4d(rect,&r);
            waterRect = gfc_allocate_array(sizeof(Rect),1);
            *waterRect = gfc_rect_from_vector4(r);
            level_water_draw_rect(*waterRect,waterActor,frame);
            gamelevel.water = gfc_list_append(gamelevel.water,waterRect);
        }
    }
    

    SDL_SetRenderTarget(gf2d_graphics_get_renderer(),NULL);
    gf2d_actor_free(waterActor);
}


void level_make_shadow_layer(LevelInfo *linfo)
{
    Sprite *sprite;
    int i,j;
    if (!linfo)
    {
        slog("no level info provided");
        return;
    }
    if (!gamelevel.tileShadow)
    {
        slog("no tileShadow loaded for level");
        return;
    }
    sprite = level_make_layer(linfo);
    if (!sprite)
    {
        slog("failed to make darkLayer");
        return;
    }
    if (gamelevel.darkLayer != NULL)
    {
        // if one already exists, clear it
        gf2d_sprite_delete(gamelevel.darkLayer);
    }
    gamelevel.darkLayer = sprite;
    SDL_SetRenderTarget(gf2d_graphics_get_renderer(),
        gamelevel.darkLayer->texture);
    for (j = 0;j < linfo->tileMapSize.y;j++)
    {
        for (i = 0; i < linfo->tileMapSize.x;i++)
        {
            if (!linfo->tileMap[j * (Uint32)linfo->tileMapSize.x + i])continue;
            if (level_tile_hidden(linfo,i,j))
            {
                gf2d_sprite_draw_image(
                    gamelevel.tileShadow,
                    vector2d(gamelevel.tileset->sprite->frame_w * i - 8,gamelevel.tileset->sprite->frame_h * j - 8));
            }
        }
    }
    SDL_SetRenderTarget(gf2d_graphics_get_renderer(),NULL);
}


void level_build_tile_space(LevelInfo *linfo)
{
    int i,j;
    for (j = 0;j < linfo->tileMapSize.y;j++)
    {
        for (i = 0; i < linfo->tileMapSize.x;i++)
        {
            if (!linfo->tileMap[j * (Uint32)linfo->tileMapSize.x + i])continue;
            gf2d_space_add_static_shape(gamelevel.space,gfc_shape_rect(i * gamelevel.tileset->sprite->frame_w, j * gamelevel.tileset->sprite->frame_h, gamelevel.tileset->sprite->frame_w, gamelevel.tileset->sprite->frame_h));
        }
    }
}

void level_spawn_change_position(SJson *spawn,Vector2D position)
{
    if (!spawn)return;
    sj_object_delete_key(spawn,"position");
    sj_object_insert(spawn,"position",sj_vector2d_new(position));
}

int level_spawn_get_id(SJson *spawn)
{
    int id = -1;
    if (!spawn)return -1;
    sj_get_integer_value(sj_object_get_value(spawn,"id"),&id);
    return id;
}

const char *level_spawn_name(SJson *spawn)
{
    if (!spawn)return NULL;
    return sj_get_string_value(sj_object_get_value(spawn,"name"));
}

Vector2D level_spawn_position(SJson *spawn)
{
    Vector2D position = {0};
    if (!spawn)return position;
    sj_value_as_vector2d(sj_object_get_value(spawn,"position"),&position);
    return position;
}

SJson *level_spawn_get_by_position(LevelInfo *linfo,Vector2D position)
{
    SJson *item;
    Spawn *spawn;
    SpawnData *spawnData;
    Shape  shape;
    Vector2D spawnPosition;
    const char *name;
    int i,c;
    if (!linfo)return NULL;
    c = sj_array_get_count(linfo->spawnList);
    for (i = 0; i < c; i++)
    {
        item = sj_array_get_nth(linfo->spawnList,i);
        if (!item)continue;
        name = sj_get_string_value(sj_object_get_value(item,"name"));
        spawn = spawn_get_by_name(name);
        if (!spawn)continue;
        spawnData = spawn_data_get_by_index(spawn_get_index_by_name(name));
        if (!spawnData)continue;

        gfc_shape_copy(&shape,spawnData->shape);
        spawnPosition = level_spawn_position(item);

        gfc_shape_move(&shape,spawnPosition);
        if (gfc_point_in_shape(position,shape))return item;
    }
    return NULL;
}

SJson *level_spawn_get_by_id(LevelInfo *linfo,Uint32 id)
{
    SJson *item;
    int index;
    int i,c;
    if (!linfo)return NULL;
    c = sj_array_get_count(linfo->spawnList);
    for (i = 0; i < c; i++)
    {
        item = sj_array_get_nth(linfo->spawnList,i);
        if (!item)continue;
        if (sj_get_integer_value(sj_object_get_value(item,"id"),&index))
        {
            if (index == id)return item;
        }
    }
    return NULL;
}

void level_spawn_entities(SJson *spawnList)
{
    int i = 0, count = 0;
    SJson *item;
    Vector2D position;
    int id = 0;
    count  = sj_array_get_count(spawnList);
    for (i = 0; i < count; i++)
    {
        item = sj_array_get_nth(spawnList,i);
        if (!item)continue;
        position = level_spawn_position(item);
        if (!sj_get_integer_value(sj_object_get_value(item,"id"),&id))
        {
            id = 0;
        }
        spawn_entity(sj_get_string_value(sj_object_get_value(item,"name")),position,id,item);
    }
}

void level_draw_outline_tile(LevelInfo *linfo,Vector2D tile,Color color)
{
    Vector2D cam;
    if (!linfo)
    {
        slog("no tile provided");
        return;
    }
    if ((tile.x < 0)||(tile.x >= linfo->tileMapSize.x)||
        (tile.y < 0)||(tile.y >= linfo->tileMapSize.y))
    {
        return;
    }
    cam = gf2d_camera_get_offset();
    gf2d_draw_rect(
        gfc_rect(
            (tile.x * gamelevel.tileset->sprite->frame_w) + cam.x,
            (tile.y * gamelevel.tileset->sprite->frame_h) + cam.y,
            gamelevel.tileset->sprite->frame_w,
            gamelevel.tileset->sprite->frame_h),
            color);
}

void level_update_tile(LevelInfo *linfo,Vector2D position,Uint32 tile)
{
    if (!linfo)
    {
        slog("no tile provided");
        return;
    }
    if (!gfc_point_in_rect(position,gfc_rect(0,0,linfo->tileMapSize.x - 1,linfo->tileMapSize.y - 1)))
    {
        return;
    }
    linfo->tileMap[(Uint32)position.y * (Uint32)linfo->tileMapSize.x + (Uint32)position.x] = tile;
}

int level_info_get_tile_index_at(LevelInfo *linfo,Vector2D position)
{
    if ((!linfo)||(!linfo->tileMap))
    {
        slog("no level info or missing tilemap");
        return -1;
    }
    return linfo->tileMap[(Uint32)position.y * (Uint32)linfo->tileMapSize.x + (Uint32)position.x];
}

void level_add_border(LevelInfo *linfo)
{
    int i;
    if (!linfo)return;
    for (i = 0; i < linfo->tileMapSize.x;i++)
    {
        level_update_tile(linfo,vector2d(i,0),1);
        level_update_tile(linfo,vector2d(i,linfo->tileMapSize.y - 1),1);
    }
    for (i = 0; i < linfo->tileMapSize.y;i++)
    {
        level_update_tile(linfo,vector2d(0,i),1);
        level_update_tile(linfo,vector2d(linfo->tileMapSize.x - 1,i),1);
    }
    level_make_tile_layer(linfo);
}

Vector2D level_mouse_to_tile()
{
    Vector2D cam,mouse,out = {-1,-1};
    if (gamelevel.tileset->sprite == NULL)return out;
    cam =  gf2d_camera_get_position();
    mouse = gf2d_mouse_get_position();
    vector2d_add(mouse,mouse,cam);
    out.x = (int)(mouse.x/gamelevel.tileset->sprite->frame_w);
    out.y = (int)(mouse.y/gamelevel.tileset->sprite->frame_h);
    return out;
}

Vector2D level_snape_to_tile(Vector2D position)
{
    Vector2D tile = {-1,-1};
    if ((!gamelevel.tileset)||(!gamelevel.tileset->sprite))return tile;

    tile.x = ((Uint32)(position.x / gamelevel.tileset->sprite->frame_w)) * gamelevel.tileset->sprite->frame_w;
    tile.y = ((Uint32)(position.y / gamelevel.tileset->sprite->frame_h)) * gamelevel.tileset->sprite->frame_h;
    return tile;
}

Vector2D level_position_to_tile(LevelInfo *linfo, Vector2D position)
{
    Vector2D tile = {-1,-1};
    if (!linfo)
    {
        slog("no level info provided");
        return tile;
    }
    if ((!gamelevel.tileset->sprite->frame_w)||(!gamelevel.tileset->sprite->frame_h))
    {
        slog("level info missing tile size data!");
        return tile;
    }
    tile.x = (Uint32)(position.x / gamelevel.tileset->sprite->frame_w);
    tile.y = (Uint32)(position.y / gamelevel.tileset->sprite->frame_h);
    return tile;
}

void level_transition(char *filename, const char *playerTarget, Uint32 targetId)
{
    Vector2D position;
    SJson *spawnInfo;
    TextLine targetname;
    Uint32 id;
    LevelInfo *linfo = NULL;
    
    if (!filename)return;
    gfc_line_cpy(targetname,playerTarget);
    id = targetId;
    
    linfo = level_info_load(filename);
    if (!linfo)return;
    
    entity_clear_all_but_player();
    level_init(linfo,1,0);
    
    spawnInfo = level_spawn_get_by_id(linfo,id);
    if (!spawnInfo)
    {
        slog("expected target %s, %i not found",playerTarget,id);
        return;
    }
    sj_value_as_vector2d(sj_object_get_value(spawnInfo,"position"),&position);
    
    player_set_position(vector2d(position.x,position.y));
    level_add_entity(player_get());
}

void level_delete_background(Uint32 layer)
{
    Sprite *image;
    if (!gamelevel.backgroundImages)return;
    image = gfc_list_get_nth(gamelevel.backgroundImages,layer);
    if (!image)return;
    gf2d_sprite_free(image);
    gfc_list_delete_nth(gamelevel.backgroundImages,layer);
}

void level_add_background(Sprite *background)
{
    if ((!gamelevel.backgroundImages)||(!background))return;
    gamelevel.backgroundImages = gfc_list_append(gamelevel.backgroundImages,background);
}

void level_info_delete_background(LevelInfo *linfo,Uint32 layer)
{
    char *name;
    if (!linfo)return;
    name = gfc_list_get_nth(linfo->backgroundImages,layer);
    if (!name)return;
    free(name);
    gfc_list_delete_nth(linfo->backgroundImages,layer);
}

void level_info_add_background(LevelInfo *linfo,const char *background)
{
    char *name;
    if ((!linfo)||(!background))return;
    name = gfc_allocate_array(sizeof(TextLine),1);
    gfc_line_cpy(name,background);
    linfo->backgroundImages = gfc_list_append(linfo->backgroundImages,name);
}

void level_info_lower_layer(LevelInfo *linfo,Uint32 layer)
{
    int c;
    if (!linfo)return;
    c = gfc_list_get_count(linfo->backgroundImages);
    if ((layer >= c)||(!layer))return;
    gfc_list_swap_indices(linfo->backgroundImages,layer, layer - 1);
}

void level_info_raise_layer(LevelInfo *linfo,Uint32 layer)
{
    int c;
    if (!linfo)return;
    c = gfc_list_get_count(linfo->backgroundImages);
    if ((layer +1) >= c)return;
    gfc_list_swap_indices(linfo->backgroundImages,layer, layer + 1);
}

void level_lower_layer(Uint32 layer)
{
    int c;
    c = gfc_list_get_count(gamelevel.backgroundImages);
    if ((layer >= c)||(layer == 0))return;
    gfc_list_swap_indices(gamelevel.backgroundImages,layer, layer - 1);
}

void level_raise_layer(Uint32 layer)
{
    int c;
    c = gfc_list_get_count(gamelevel.backgroundImages);
    if ((layer +1) >= c)return;
    gfc_list_swap_indices(gamelevel.backgroundImages,layer, layer + 1);
}

void level_set_music(const char *music)
{
    if (gamelevel.backgroundMusic)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(gamelevel.backgroundMusic);
    }
    gamelevel.backgroundMusic = Mix_LoadMUS(music);
    if (gamelevel.backgroundMusic)Mix_PlayMusic(gamelevel.backgroundMusic, -1);
}

void level_set_tileset(const char *tileset)
{
    if (!tileset)return;
    if (gamelevel.tileset != NULL)
    {
        gf2d_actor_free(gamelevel.tileset);
    }
    gamelevel.tileset = gf2d_actor_load(tileset);
    slog("loading tileset %s",tileset);
}

void level_init(LevelInfo *linfo,Uint8 space,Uint8 editMode)
{
    int i,c;
    Sprite *image;
    const char *imageName;
    if (!linfo)
    {
        return;
    }
    level_clear();
    gamelevel.backgroundImages = gfc_list_new();
    c = gfc_list_get_count(linfo->backgroundImages);
    for (i = 0;i < c; i++)
    {
        imageName = gfc_list_get_nth(linfo->backgroundImages,i);
        if (!imageName)continue;
        image = gf2d_sprite_load_image(imageName);
        if (!image)continue;
        level_add_background(image);
    }
    gamelevel.tileset = gf2d_actor_load(linfo->tileSet);
    gamelevel.tileShadow = gf2d_sprite_load_image("images/tiles/tile_shadow.png");

    gamelevel.backgroundMusic = Mix_LoadMUS(linfo->backgroundMusic);
    if (gamelevel.backgroundMusic)
    {
     //   Mix_PlayMusic(gamelevel.backgroundMusic, -1);
    }
    
    if (gamelevel.pe == NULL)
    {
        gamelevel.pe = gf2d_particle_emitter_new_full(
            10000,//int         maxParticles,
            1000,
            10,
            PT_Pixel,//ParticleTypes particleType,
            vector2d(0,0),//position
            vector2d(0,0),//Vector2D    positionVariance,
            vector2d(0,0),//Vector2D    velocity,
            vector2d(0,0),//Vector2D    velocityVariance,
            vector2d(0,0),//Vector2D    acceleration,
            vector2d(0,0),//Vector2D    accelerationVariance,
            gfc_color8(0,0,0,255),//Color       color,
            gfc_color8(0,0,0,0),//Color       colorVector,
            gfc_color8(0,0,0,0),//Color       colorVariance,
            NULL,//Shape      *shape,
            0,//Uint32      startFrame,
            0,//Uint32      endFrame,
            0,//Uint32      frameVariance,
            NULL,//const char *spriteFile,
            0,//Uint32      frameWidth,
            0,//Uint32      frameHeight,
            0,//Uint32      framesPerLine,
            0,//float       framerate,
            SDL_BLENDMODE_BLEND);//SDL_BlendMode mode);
    }
    level_make_tile_layer(linfo);
    
    gf2d_camera_set_bounds(0,0,gamelevel.tileLayer->surface->w,gamelevel.tileLayer->surface->h);

    gamelevel.info = linfo;
    gamelevel.dark = linfo->dark;
    if (editMode)return;
    level_make_light_layer(linfo);
    level_make_shadow_layer(linfo);
    level_make_water_layer(linfo);

    if (space)
    {
        level_make_space();
        level_build_tile_space(linfo);
    }
    level_spawn_entities(linfo->spawnList);
}

void level_draw_background()
{
    int i,c;
    Sprite *image;
    Vector2D cam,diff,size, parallax = {0};
    
    if (!gamelevel.backgroundImages)return;

    cam = gf2d_camera_get_offset();
    size = gf2d_camera_get_size();
    diff = level_get_camera_diff();
    c = gfc_list_get_count(gamelevel.backgroundImages);
    for (i = 0; i < c; i++)
    {
        image = gfc_list_get_nth(gamelevel.backgroundImages,i);
        if (!image)continue;
        vector2d_clear(parallax);
        if (diff.x)
        {
            parallax.x = (float)(image->frame_w - size.x)/ diff.x;
        }
        if (diff.y)
        {
            parallax.y = (float)(image->frame_h - size.y)/ diff.y;
        }
        gf2d_sprite_draw_image(image,vector2d(cam.x * parallax.x,cam.y * parallax.y));        
    }
}

Sprite *level_get_tile_layer()
{
    return gamelevel.tileLayer;
}

void level_draw_tiles(Vector2D offset)
{
    gf2d_sprite_draw_image(gamelevel.tileLayer,offset);
}

void level_resize(LevelInfo *linfo,Vector2D newSize)
{
    int i,j;
    int oldIndex,newIndex;
    int *newMap = NULL;
    if ((!linfo)||(!linfo->tileMap))return;
    newMap = level_alloc_tilemap(newSize.x,newSize.y);
    if (!newMap)return;
    for (j = 0;j < MIN(newSize.y,linfo->tileMapSize.y);j++)
    {
        for (i = 0; i < MIN(newSize.x,linfo->tileMapSize.x);i++)
        {
            oldIndex = j * linfo->tileMapSize.x + i;
            newIndex = j * newSize.x + i;
            newMap[newIndex] = linfo->tileMap[oldIndex];
        }
    }
    free(linfo->tileMap);
    linfo->tileMap = newMap;
    vector2d_copy(linfo->tileMapSize,newSize);
    level_init(linfo,0,1);
}

void level_draw()
{
    Uint32 ticks;
    float offset = 0;
    Vector2D cam,drawPosition;
    cam = gf2d_camera_get_offset();
    level_draw_background();
    if ((!editorMode) && (gamelevel.waterLayer))
    {
        ticks = SDL_GetTicks();
        offset = cos((((ticks % 5000)/(float)5000) * GFC_2PI) - GFC_PI) * 8;
        // the above uses time changes to make an oscillating position based on cosine
        drawPosition.x = cam.x;
        drawPosition.y = cam.y + offset;
        gf2d_sprite_draw_image(gamelevel.waterLayer,drawPosition);
    }
    level_draw_tiles(cam);
    gf2d_entity_draw_all();
    gf2d_entity_draw(player_get());
    gf2d_particle_emitter_draw(gamelevel.pe,cam);
    if ((!editorMode) && (gamelevel.waterLayer))gf2d_sprite_draw_image(gamelevel.waterLayer,cam);

    if ((!editorMode) && (gamelevel.dark))level_darkness_draw(cam);
    if ((__DebugMode) && (gamelevel.space))gf2d_space_draw(gamelevel.space,cam);
}

void level_draw_spawn(SJson *item)
{
    const char *name;
    Vector2D cam;
    Vector2D position;
    if (!item)return;
    cam = gf2d_camera_get_offset();
    sj_value_as_vector2d(sj_object_get_value(item,"position"),&position);
    name = sj_get_string_value(sj_object_get_value(item,"name"));
    vector2d_add(position,position,cam);
    spawn_draw_spawn_by_name(name, position);
}

void level_draw_spawns(LevelInfo *linfo)
{
    int i = 0, count = 0;
    SJson *item;
    if (!linfo)return;
    count  = sj_array_get_count(linfo->spawnList);
    for (i = 0; i < count; i++)
    {
        item = sj_array_get_nth(linfo->spawnList,i);
        if (!item)continue;
        level_draw_spawn(item);
    }
}

void level_update()
{
    gf2d_space_update(gamelevel.space);
    gf2d_particle_emitter_update(gamelevel.pe);
}

void level_remove_entity(Entity *ent)
{
    if (!ent)return;
    if (!gamelevel.space)
    {//nothing to do
        return;
    }
    gf2d_entity_remove_from_space(ent,gamelevel.space);
}

SJson *level_new_spawn_entity(const char *name,Vector2D position,Uint32 id)
{
    SJson *spawn;
    if (!name)return NULL;
    spawn = sj_object_new();
    if (!spawn)return NULL;
    sj_object_insert(spawn,"name",sj_new_str(name));
    sj_object_insert(spawn,"position",sj_vector2d_new(position));
    sj_object_insert(spawn,"id",sj_new_int(id));
    sj_object_insert(spawn,"keys",sj_object_new());
    return spawn;
}

void level_add_spawn_entity(LevelInfo *linfo, const char *name,Vector2D position)
{
    SJson *spawn;
    if (!linfo)return;
    spawn = level_new_spawn_entity(name,position,linfo->idPool++);
    if (spawn == NULL)return;
    sj_array_append(linfo->spawnList,spawn);
}

void level_add_entity(Entity *ent)
{
    if (!ent)return;
    if (!gamelevel.space)
    {
        slog("cannot add entity %s to level, no space defined!",ent->name);
        return;
    }
    gf2d_entity_add_to_space(ent,gamelevel.space);
}

const char *level_get_name()
{
    if (!gamelevel.info)return NULL;
    return gamelevel.info->filename;
}

Space *level_get_space()
{
    return gamelevel.space;
}

ParticleEmitter *level_get_particle_emitter()
{
    return gamelevel.pe;
}
/*eol@eof*/
