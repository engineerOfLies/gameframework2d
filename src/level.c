#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_config.h"

#include "level.h"

Level *level_load(const char *filename)
{
    int tile;
    int i,c;
    int j,d;
    int tileFPL;
    const char *str;
    SJson *json,*lj,*list,*row,*item;
    Vector2D tileSize = {0};
    Level *level;
    if (!filename)return NULL;
    json = sj_load(filename);
    if (!json)return NULL;
    level = level_new();
    if (!level)
    {
        slog("failed to allocate new level");
        sj_free(json);
        return NULL;
    }
    lj = sj_object_get_value(json,"level");
    if (!lj)
    {
        slog("file %s missing level object",filename);
        sj_free(json);
        level_free(level);
        return NULL;
    }
    str = sj_object_get_value_as_string(lj,"name");
    if (str)gfc_line_cpy(level->name,str);
    sj_value_as_vector2d(sj_object_get_value(lj,"tileSize"),&tileSize);
    sj_object_get_value_as_int(lj,"tileFPL",&tileFPL);
    str = sj_object_get_value_as_string(lj,"tileSet");
    if (str)
    {
        level->tileSet = gf2d_sprite_load_all(str,(Sint32)tileSize.x,(Sint32)tileSize.y,tileFPL,1);
    }
    list = sj_object_get_value(lj,"tileMap");
    c = sj_array_get_count(list);
    row = sj_array_get_nth(list,0);
    d = sj_array_get_count(row);
    if ((c * d) == 0)
    {
        slog("corrupt row or column count for %s level",filename);
        level_free(level);
        sj_free(json);
        return NULL;
    }
    level->tileMap = gfc_allocate_array(sizeof(int),c * d);
    if (!level->tileMap)
    {
        slog("failed to allocate tileMap for level %s",filename);
        level_free(level);
        sj_free(json);
        return NULL;
    }
    for (i = 0; i < c; i++)
    {
        row = sj_array_get_nth(list,i);
        if (!row)continue;
        d = sj_array_get_count(row);
        for (j =0; j < d; j++)
        {
            item = sj_array_get_nth(row,j);
            if (!item)continue;
            sj_get_integer_value(item,&tile);
        }
    }
    sj_free(json);
    return level;
}

Level *level_new()
{
    Level *level;
    level = gfc_allocate_array(sizeof(Level),1);
    
    return level;
}

void level_free(Level *level)
{
    if (!level)return;
    if (level->tileSet)gf2d_sprite_free(level->tileSet);
    if (level->tileLayer)gf2d_sprite_free(level->tileLayer);
    if (level->tileMap)free(level->tileMap);
    free(level);
}

/*eol@eof*/
