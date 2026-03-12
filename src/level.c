#include <stdio.h>

#include "simple_logger.h"

#include "gf2d_graphics.h"

#include "camera.h"
#include "level.h"

void level_setup_camera_bounds(Level *level);

Level *level_new()
{
    Level *level;
    level = gfc_allocate_array(sizeof(Level),1);
    if (!level)return NULL;
    return level;
}

Level *level_load(const char *filepath)
{
    int i,j;
    int index;
    const char *str;
    SJson *rows,*tiles,*tile;
    Level * level;
    SJson *json;
    SJson *config;
    if (!filepath)return NULL;
    json = sj_load(filepath);
    if (!json)return NULL;
    
    config = sj_object_get_value(json,"level");
    if (!config)
    {
        slog("failed to load level file %f, missing level information",filepath);
        sj_free(json);
        return NULL;
    }
    level = level_new();
    if (!level)
    {
        slog ("failed to allocate level data for levl %s",filepath);
        sj_free(json);
        return NULL;
    }
    
    str = sj_object_get_string(config,"background");
    if (str)
    {
        level->background = gf2d_sprite_load_image(str);
    }
    level->tiledef = tiledef_parse(sj_object_get_value(config,"tileDef"));
    if (!level->tiledef)
    {
        level_free(level);
        sj_free(json);
        slog("failed to parse tiledef data from level %s",filepath);
        return NULL;
    }
    rows = sj_object_get_value(config,"tileMap");
    level->height = sj_array_count(rows);
    if (!level->height)
    {
        level_free(level);
        sj_free(json);
        slog("we suck: %s",filepath);
        return NULL;
    }
    tiles = sj_array_nth(rows,0);
    level->width = sj_array_count(tiles);
    if (!level->width)
    {
        level_free(level);
        sj_free(json);
        slog("we suck: %s",filepath);
        return NULL;
    }
    level->tileMap = gfc_allocate_array(sizeof(Uint8),level->width*level->height);
    if (!level->tileMap)
    {
        level_free(level);
        sj_free(json);
        slog("we suck: %s",filepath);
        return NULL;
    }
    for (j = 0; j < level->height;j++)
    {
        tiles = sj_array_get_nth(rows,j);
        if (!tiles)continue;
        for (i = 0;i < level->width;i++)
        {
            tile = sj_array_nth(tiles,i);
            if (!tile)continue;
            index = level_get_tile_index(level,i, j);
            if (index == -1)continue;
            sj_get_uint8_value(tile,&level->tileMap[index]);
        }
    }
    slog("we loaded %i by %i tiles",level->width,level->height);
    level_setup_camera_bounds(level);
    sj_free(json);
    return level;
}

int level_get_tile_index(Level *level,Uint32 x, Uint32 y)
{
    if ((!level)||(!level->tileMap))return -1;
    if (x >= level->width)return -1;
    if (y >= level->height)return -1;
    return (y * level->width) + x;
}

void level_add_border(Level *level,Uint8 tile)
{
    int index;
    int i,j;
    if ((!level)||(!level->tileMap))return;
    for (j = 0; j < level->height; j++)
    {
        index = level_get_tile_index(level,0, j);
        if (index >= 0)level->tileMap[index] = tile;
        index = level_get_tile_index(level,level->width - 1, j);
        if (index >= 0)level->tileMap[index] = tile;
    }
    for (i = 1; i < level->width-1; i++)
    {
        index = level_get_tile_index(level,i, 0);
        if (index >= 0)level->tileMap[index] = tile;
        index = level_get_tile_index(level,i, level->height-1);
        if (index >= 0)level->tileMap[index] = tile;
    }
}

void level_free(Level *level)
{
    if (!level)return;
    gf2d_sprite_free(level->background);
    gf2d_sprite_free(level->tileLayer);
    tiledef_free(level->tiledef);
    if (level->tileMap)free(level->tileMap);
    free(level);
}

void level_setup_camera_bounds(Level *level)
{
    if ((!level)||(!level->tiledef))return;
    camera_set_bounds(gfc_rect(0,0,level->tiledef->width * level->width,level->tiledef->height * level->height));
}

void level_bake_tiles(Level *level)
{
    Uint8 tile;
    SDL_Surface * tileSurface;
    int index;
    int i,j;
    if ((!level)||(!level->tiledef))return;
    if ((!level->width)||(!level->height))return;
    level->size.x = level->tiledef->width * level->width;
    level->size.y = level->tiledef->height * level->height;
    tileSurface = gf2d_graphics_create_surface(level->size.x,level->size.y);
    if (!tileSurface)
    {
        slog("failed to allocate a surface for the size of the image requested");
        return;
    }
    for (j = 0; j < level->height; j++)
    {
        for (i = 0 ;i < level->width; i++)
        {
            index = level_get_tile_index(level,i,j);
            if (index < 0)continue;
            tile = level->tileMap[index];
            if (!tile)continue;
            gf2d_sprite_draw_to_surface(
                    level->tiledef->sheet,
                    gfc_vector2d((i * level->tiledef->width),(j * level->tiledef->height)),
                    NULL,
                    NULL,
                    tile - 1,
                    tileSurface
                );
        }
    }
    level->tileLayer = gf2d_sprite_from_surface(
            tileSurface,
            -1,
            -1,
            0,
            1
        );
}

Level *level_load_bin(const char *filename)
{
    GFC_TextLine buffer;
    FILE *file;
    Level *level;
    if (!filename)return NULL;
    file = fopen(filename, "rb");
    if (!file)
    {
        slog("failed to open level file %s",filename);
        return NULL;
    }
    level = level_new();
    if (!level)
    {
        slog("failed to allocate a new level");
        goto failparse;
    }
    
    fread(buffer, sizeof(GFC_TextLine), 1,file);
    level->background = gf2d_sprite_load_image(buffer);
    fread(&level->width,sizeof(Uint32),1,file);
    fread(&level->height,sizeof(Uint32),1,file);
    if ((!level->width)||(!level->height))
    {
        slog("level file %s is bad: width: %i, height: %i",filename,level->width,level->height);
        goto failparse;
    }
    level->tileMap = gfc_allocate_array(sizeof(Uint8),level->width* level->height);
    if (!level->tileMap)
    {
        slog("failed to allocate tilemap for %s, width: %i, height: %i",filename,level->width,level->height);
        goto failparse;
    }
    fread(level->tileMap,sizeof(Uint8),level->width* level->height,file);
    slog("we loaded %i by %i tiles",level->width,level->height);
    level->tiledef = tiledef_load_from_file(file);
    level_setup_camera_bounds(level);

    fclose(file);
    return level;
failparse:
    level_free(level);
    fclose(file);
    return NULL;
}

void level_save_bin(Level *level, const char *filename)
{
    FILE *file;
    GFC_TextLine blank = {0};
    if ((!level)||(!filename))return;
    file = fopen(filename, "wb");
    if (!file)
    {
        slog("failed to savel level to file %s!",filename);
        return;
    }
    if (level->background)
    {
        fwrite(level->background->filepath,sizeof(GFC_TextLine),1,file);
    }
    else
    {
        fwrite(blank,sizeof(GFC_TextLine),1,file);
    }
    fwrite(&level->width,sizeof(Uint32),1,file);
    fwrite(&level->height,sizeof(Uint32),1,file);
    fwrite(level->tileMap,sizeof(Uint8),level->width* level->height,file);
    tiledef_save_to_file(level->tiledef, file);
    fclose(file);
}

void level_slog(Level *level)
{
    if (!level)return;
    if (level->background)slog("level background: %s",level->background->filepath);
    slog("level tile width: %i height: %i",level->width,level->height);
}

void level_draw(Level *level)
{
    GFC_Vector2D offset;
    if (!level)return;
    offset = camera_get_offset();
    if (level->background)
    {
        gf2d_sprite_draw_image(level->background,offset);
    }
    if (level->background)
    {
        gf2d_sprite_draw_image(level->tileLayer,offset);
    }
}

/**/
