#include "level.h"
#include "camera.h"
#include "spawn.h"
#include "player.h"
#include "entity_common.h"
#include "simple_json.h"
#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_particles.h"
#include "gf2d_config.h"
#include <stdio.h>

typedef struct
{
    Space           *space;
    Sprite          *backgroundImage;
    Sprite          *tileLayer;
    Sprite          *tileSet;
    Mix_Music       *backgroundMusic;
    ParticleEmitter *pe;
}Level;

static Level gamelevel = {0};

int *level_alloc_tilemap(int w,int h);

void level_clear()
{
    gf2d_space_free(gamelevel.space);
    gf2d_sprite_free(gamelevel.backgroundImage);
    gf2d_sprite_free(gamelevel.tileSet);
    gf2d_sprite_free(gamelevel.tileLayer);
    gf2d_particle_emitter_free(gamelevel.pe);
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
    sj_free(linfo->spawnList);
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

LevelInfo *level_info_create(
    const char *backgroundImage,
    const char *backgroundMusic,
    const char *tileSet,
    Vector2D    tileSize,
    Vector2D    tileMapSize
)
{
    LevelInfo *linfo = NULL;
    linfo = level_info_new();
    if (!linfo)return NULL;
    gf2d_line_cpy(linfo->backgroundImage,backgroundImage);
    gf2d_line_cpy(linfo->backgroundMusic,backgroundMusic);
    gf2d_line_cpy(linfo->tileSet,tileSet);
    vector2d_copy(linfo->tileSize,tileSize);
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
    
    linfo->spawnList = sj_copy(sj_object_get_value(world,"spawnList"));
    
    sj_free(json);
    slog("loaded level info for %s",filename);
    return linfo;
}

void level_make_space()
{
    gamelevel.space = gf2d_space_new_full(
        3,
        gf2d_rect(0,0,gamelevel.tileLayer->surface->w,gamelevel.tileLayer->surface->h),
        0.1,
        vector2d(0,0),
        1,
        0.001);
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
    if (!gamelevel.tileSet)
    {
        slog("no tileset loaded for level");
        return;
    }
    format = gamelevel.tileSet->surface->format->format;
    sprite = gf2d_sprite_new();
    if (!sprite)return;
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
                gamelevel.tileSet,
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
        
        gf2d_line_cpy(sprite->filepath,"tileLayer");
        gamelevel.tileLayer = sprite;
    }
    else
    {
        slog("failed to convert tile layer to supported format");
        gf2d_sprite_free(sprite);
    }
    
}

void level_build_tile_space(LevelInfo *linfo)
{
    int i,j;
    for (j = 0;j < linfo->tileMapSize.y;j++)
    {
        for (i = 0; i < linfo->tileMapSize.x;i++)
        {
            if (!linfo->tileMap[j * (Uint32)linfo->tileMapSize.x + i])continue;
            gf2d_space_add_static_shape(gamelevel.space,gf2d_shape_rect(i * linfo->tileSize.x, j * linfo->tileSize.y, linfo->tileSize.x, linfo->tileSize.y));
        }
    }
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
        sj_value_as_vector2d(sj_object_get_value(item,"position"),&position);
        if (!sj_get_integer_value(sj_object_get_value(item,"name"),&id))
        {
            id = 0;
        }
        spawn_entity(sj_get_string_value(sj_object_get_value(item,"name")),position,id,sj_object_get_value(item,"args"));
    }
}

void level_update_tile(LevelInfo *linfo,Vector2D position,Uint32 tile)
{
    if (!linfo)
    {
        slog("no tile provided");
        return;
    }
    if (!gf2d_point_in_rect(position,gf2d_rect(0,0,linfo->tileMapSize.x,linfo->tileMapSize.y)))
    {
        slog("tile position %f,%f is out of bounds of tile map",position.x,position.y);
        return;
    }
    linfo->tileMap[(Uint32)position.y * (Uint32)linfo->tileMapSize.x + (Uint32)position.x] = tile;
}

Vector2D level_position_to_tile(LevelInfo *linfo, Vector2D position)
{
    Vector2D tile = {-1,-1};
    if (!linfo)
    {
        slog("no level info provided");
        return tile;
    }
    if ((!linfo->tileSize.x)||(!linfo->tileSize.y))
    {
        slog("level info missing tile size data!");
        return tile;
    }
    tile.x = (Uint32)(position.x / linfo->tileSize.x);
    tile.y = (Uint32)(position.y / linfo->tileSize.y);
    return tile;
}

void level_transition(char *filename, const char *playerTarget, Uint32 targetId)
{
    Entity *target;
    TextLine filepath;
    TextLine targetname;
    Uint32 id;
    LevelInfo *linfo = NULL;
    
    snprintf(filepath,GF2DLINELEN,"levels/%s",filename);
    gf2d_line_cpy(targetname,playerTarget);
    id = targetId;
    
    linfo = level_info_load(filepath);
    if (!linfo)return;
    
    entity_clear_all_but_player();
    level_init(linfo,1);
    
    target = gf2d_entity_get_by_name_id(targetname,id);
    if (!target)
    {
        slog("expected target %s, %i not found",target,id);
        return;
    }
    player_set_position(vector2d(target->position.x,target->position.y-16));
}

void level_init(LevelInfo *linfo,Uint8 space)
{
    if (!linfo)
    {
        return;
    }
    level_clear();
    gamelevel.backgroundImage = gf2d_sprite_load_image(linfo->backgroundImage);
    gamelevel.tileSet = gf2d_sprite_load_all(
        linfo->tileSet,
        linfo->tileSize.x,
        linfo->tileSize.y,
        16,
        true);

    gamelevel.backgroundMusic = Mix_LoadMUS(linfo->backgroundMusic);
    if (gamelevel.backgroundMusic)Mix_PlayMusic(gamelevel.backgroundMusic, -1);
    
    level_make_tile_layer(linfo);
    
    camera_set_bounds(0,0,gamelevel.tileLayer->surface->w,gamelevel.tileLayer->surface->h);
    
    if (space)
    {
        level_make_space();
        level_build_tile_space(linfo);
    }
    gamelevel.pe = gf2d_particle_emitter_new_full(
        2048,
        10,
        5,
        PT_Pixel,
        vector2d(0,0),
        vector2d(0,0),
        vector2d(0,0),
        vector2d(0,0),
        vector2d(0,0),
        vector2d(0,0),
        gf2d_color(0,0,0,1),
        gf2d_color(0,0,0,0),
        gf2d_color(0,0,0,0),
        NULL,
        0,
        0,
        0,
        "",
        0,
        0,
        0,
        0,
        SDL_BLENDMODE_BLEND);
    level_spawn_entities(linfo->spawnList);
}

void level_draw()
{
    Vector2D cam;
    cam = camera_get_offset();
    gf2d_sprite_draw_image(gamelevel.backgroundImage,cam);
    gf2d_sprite_draw_image(gamelevel.tileLayer,cam);
    gf2d_entity_draw_all();
    gf2d_entity_draw(player_get());
    gf2d_particle_emitter_draw(gamelevel.pe,cam);    
//    if (gamelevel.space)gf2d_space_draw(gamelevel.space,cam);
}

void level_update()
{
    gf2d_entity_pre_sync_all();
    gf2d_space_update(gamelevel.space);
    gf2d_entity_post_sync_all();
    gf2d_particle_emitter_update(gamelevel.pe);
}

int body_body_touch(Body *self, List *collisionList)
{
    Entity *selfEnt;
    Collision *c;
    int i,count;
    if (!self)return 0;
    selfEnt = (Entity*)self->data;
    if (!selfEnt->touch)return 0;
    count = gf2d_list_get_count(collisionList);
    for (i = 0; i < count; i++)
    {
        c = (Collision *)gf2d_list_get_nth(collisionList,i);
        if (!c)continue;
        if (!c->body)continue;
        if (!c->body->data)continue;
        selfEnt->touch(selfEnt,(Entity*)c->body->data);
    }
    return 0;
}

void level_remove_entity(Entity *ent)
{
    if (!ent)return;
    if (!gamelevel.space)
    {//nothing to do
        return;
    }
    gf2d_space_remove_body(gamelevel.space,&ent->body);
}

void level_add_entity(Entity *ent)
{
    if (!ent)return;
    if (!gamelevel.space)
    {
        slog("cannot add entity %s to level, no space defined!",ent->name);
        return;
    }
    if (ent->body.touch == NULL)
    {
        ent->body.touch = body_body_touch;
    }
    gf2d_space_add_body(gamelevel.space,&ent->body);
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
