#include "level.h"
#include "simple_json.h"
#include "simple_logger.h"
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
            sj_get_integer_value(value,&linfo->tileMap[j*h+i]);
        }
    }
}

LevelInfo *level_info_load(char *filename)
{
    LevelInfo *linfo = NULL;
    SJson *json,*world;
//    char *name;
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
    
    sj_value_as_vector2d(sj_object_get_value(world,"tileSize"),&linfo->tileSize);
    slog("loaded tile size of %f,%f",linfo->tileSize.x,linfo->tileSize.y);
    
    level_info_tilemap_load(linfo, sj_object_get_value(world,"tileMap"),(Uint32)linfo->tileSize.x,(Uint32)linfo->tileSize.y);
    
    sj_free(json);
    slog("loaded level info for %s",filename);
    return linfo;
}

void level_init(LevelInfo *linfo)
{
    if (!linfo)
    {
        return;
    }
    level_clear();
    gamelevel.backgroundImage = gf2d_sprite_load_image(linfo->backgroundImage);
    gamelevel.backgroundMusic = Mix_LoadMUS(linfo->backgroundMusic);
    if (gamelevel.backgroundMusic)Mix_PlayMusic(gamelevel.backgroundMusic, -1);
    
}

void level_draw()
{
    gf2d_sprite_draw_image(gamelevel.backgroundImage,vector2d(0,0));
}

void level_update()
{
    
}

/*eol@eof*/
