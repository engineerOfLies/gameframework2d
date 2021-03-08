#include <stdlib.h>

#include "simple_json.h"
#include "simple_logger.h"

#include "gfc_input.h"

#include "gf2d_config.h"
#include "gf2d_graphics.h"

#include "camera.h"
#include "level.h"


Level *level_new()
{
    Level *level;
    level = (Level *)malloc(sizeof(Level));
    if (!level)
    {
        slog("failed to allocate memory for the game level");
        return NULL;
    }
    memset(level,0,sizeof(Level));
    return level;
}

void level_generate_background(Level *level)
{
    int i,j;
    int countx,county;
    
    level->background = gf2d_sprite_new();
    if (!level->background)
    {
        slog("failed to allocate space for a background sprite!");
        return;
    }
    if ((!level->levelSize.x)||(!level->levelSize.y))
    {
        slog("cannot create a zero size background (%i,%i)",level->levelSize.x,level->levelSize.y);
    }
    level->background->surface = gf2d_graphics_create_surface(level->levelSize.x,level->levelSize.y);
    level->background->frame_h = level->background->surface->h;
    level->background->frame_w = level->background->surface->w;
    level->background->frames_per_line = 1;

    if (!level->baseTile)
    {
        slog("not generating background from baseTile, none provided!");
        gf2d_sprite_create_texture_from_surface(level->background);
        return;
    }
    countx = (level->background->surface->w / level->baseTile->frame_w) + 1;
    county = (level->background->surface->h / level->baseTile->frame_h) + 1;
    slog("generating the background with %i tiles",countx * county);
    for (j = 0; j < county;j++)
    {
        for (i = 0; i < countx; i++)
        {
            gf2d_sprite_draw_to_surface(
                level->baseTile,
                vector2d(i * level->baseTile->frame_w,j * level->baseTile->frame_h),
                NULL,
                NULL,
                level->tileFrame,
                level->background->surface
            );

        }
    }
    
    j = (int)(gfc_random() * 100);
    for ( i = 0; i < j;i++)
    {
            gf2d_sprite_draw_to_surface(
                level->baseTile,
                vector2d(gfc_random() * level->background->surface->w,gfc_random() * level->background->surface->h),
                NULL,
                NULL,
                4,
                level->background->surface
            );
    }
    gf2d_sprite_create_texture_from_surface(level->background);
}

Level *level_load(const char *filename)
{
    const char *string;
    Level *level;
    SJson *json,*levelJS;
    int frameWidth, frameHeight, framesPerLine;

    if (!filename)
    {
        slog("filename is NULL, cannot load the level");
        return NULL;
    }
    json = sj_load(filename);
    if (!json)return NULL;

    level = level_new();
    if (!level)
    {
        sj_free(json);
        return NULL;
    }
    
    levelJS = sj_object_get_value(json,"level");
    if (!levelJS)
    {
        slog("level json missing level object");
        level_free(level);
        sj_free(json);
        return NULL;
    }

    string = sj_get_string_value(sj_object_get_value(levelJS,"baseTile"));
    if (string)
    {
        slog("loading level base tile spriet %s",string);
        
        sj_get_integer_value(sj_object_get_value(levelJS,"frameWidth"),&frameWidth);
        sj_get_integer_value(sj_object_get_value(levelJS,"frameHeight"),&frameHeight);
        sj_get_integer_value(sj_object_get_value(levelJS,"framesPerLine"),&framesPerLine);
        sj_get_integer_value(sj_object_get_value(levelJS,"tileFrame"),&level->tileFrame);
        
        level->baseTile = gf2d_sprite_load_all(
                (char *)string,
                frameWidth,
                frameHeight,
                framesPerLine,
                true
            );
    }
    sj_value_as_vector2d(sj_object_get_value(levelJS,"levelSize"),&level->levelSize);    
    level_generate_background(level);
    sj_free(json);
    return level;
}


void level_update(Level *level)
{
    SDL_Rect camera;
    if (!level)return;
    
    if (gfc_input_command_down("cameraleft"))
    {
        camera_move(vector2d(-10,0));
    }
    if (gfc_input_command_down("cameraright"))
    {
        camera_move(vector2d(10,0));
    }
    if (gfc_input_command_down("cameraup"))
    {
        camera_move(vector2d(0,-10));
    }
    if (gfc_input_command_down("cameradown"))
    {
        camera_move(vector2d(0,10));
    }

    camera = camera_get_rect();
    
    //snap camera to the level bounds
    if ((camera.x + camera.w) > (int)level->levelSize.x)
    {
        camera.x = level->levelSize.x - camera.w;
    }
    if ((camera.y + camera.h) > (int)level->levelSize.y)
    {
        camera.y = level->levelSize.y - camera.h;
    }
    if (camera.x < 0)camera.x = 0;
    if (camera.y < 0)camera.y = 0;
    camera_set_position(vector2d(camera.x,camera.y));
}

void level_free(Level *level)
{
    if (!level)return;// nothing to do
    
    gf2d_sprite_free(level->baseTile);
    gf2d_sprite_free(level->background);
    
    free(level);
}

void level_draw(Level *level)
{
    Vector2D offset;
    if (!level)
    {
        slog("cannot draw level, NULL pointer provided");
        return;
    }
    // draw the background first
    offset = camera_get_offset();
    //then draw the tiles
    gf2d_sprite_draw(
    level->background,
        offset,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        0);
}



/*file footer*/
