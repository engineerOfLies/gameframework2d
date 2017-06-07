#include <stdio.h>

#include "level.h"
#include "camera.h"
#include "spawn.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_audio.h"
#include "gf2d_particles.h"
#include "gf2d_collision.h"
#include "gf2d_entity.h"
#include "simple_logger.h"

typedef struct
{
    Sprite             *backgroundImage;
    Vector2D            backgroundOffset;
    Mix_Music          *backgroundMusic;
    Vector4D            backgroundColor;
    ParticleEmitter    *pe;
    Rect                bounds;
    Vector4D            starfield;
    Vector4D            starfieldVariance;
    Uint32              starRate;
    float               starSpeed;
    Space              *space;
}Level;

static Level level = {0};

void level_spawn_entities(LevelInfo *info);

int level_info_get_spawn_count(FILE *file)
{
    char buf[512];
    int count = 0;
    if (!file)return 0;
    rewind(file);
    while(fscanf(file, "%s", buf) != EOF)
    {
        if(strcmp(buf,"entity:") == 0)
        {
            count++;
        }
        fgets(buf, sizeof(buf), file);
    }
    return count;
}

void level_info_parse_file(FILE *file,LevelInfo *info)
{
    char buf[512];
    Spawn *spawn;
    if ((!file)||(!info))return;
    rewind(file);
    spawn = info->spawnList;
    spawn--;
    while(fscanf(file, "%s", buf) != EOF)
    {
        if(strcmp(buf,"name:") == 0)
        {
            fscanf(file,"%s",(char*)&info->name);
            continue;
        }
        if(strcmp(buf,"starRate:") == 0)
        {
            fscanf(file,"%u",&info->starRate);
            continue;
        }
        if(strcmp(buf,"starSpeed:") == 0)
        {
            fscanf(file,"%f",&info->starSpeed);
            continue;
        }
        if(strcmp(buf,"backgroundColor:") == 0)
        {
            fscanf(file,"%lf,%lf,%lf,%lf",&info->backgroundColor.x,&info->backgroundColor.y,&info->backgroundColor.z,&info->backgroundColor.w);
            continue;
        }
        if(strcmp(buf,"starfield:") == 0)
        {
            fscanf(file,"%lf,%lf,%lf,%lf",&info->starfield.x,&info->starfield.y,&info->starfield.z,&info->starfield.w);
            continue;
        }
        if(strcmp(buf,"starfieldVariance:") == 0)
        {
            fscanf(file,"%lf,%lf,%lf,%lf",&info->starfieldVariance.x,&info->starfieldVariance.y,&info->starfieldVariance.z,&info->starfieldVariance.w);
            continue;
        }
        if(strcmp(buf,"backgroundMusic:") == 0)
        {
            fscanf(file,"%s",(char*)&info->backgroundMusic);
            continue;
        }
        if(strcmp(buf,"backgroundOffset:") == 0)
        {
            fscanf(file,"%lf,%lf",&info->backgroundOffset.x,&info->backgroundOffset.y);
            continue;
        }
        if(strcmp(buf,"bounds:") == 0)
        {
            fscanf(file,"%f,%f,%f,%f",&info->bounds.x,&info->bounds.y,&info->bounds.w,&info->bounds.h);
            continue;
        }
        if(strcmp(buf,"backgroundImage:") == 0)
        {
            fscanf(file,"%s",(char*)&info->backgroundImage);
            continue;
        }
        if(strcmp(buf,"entity:") == 0)
        {
            spawn++;
            if (spawn >= &info->spawnList[info->spawnCount])
            {
                slog("error in file, position before entity");
                continue;// in case data is bad
            }
            fscanf(file,"%s",(char*)&spawn->name);
            continue;
        }
        if(strcmp(buf,"position:") == 0)
        {
            if (spawn < &info->spawnList[0])
            {
                slog("error in file, position before entity");
                continue;// in case data is bad
            }
            fscanf(file,"%lf,%lf",&spawn->position.x,&spawn->position.y);
            continue;
        }
        if(strcmp(buf,"endPosition:") == 0)
        {
            if (spawn < &info->spawnList[0])
            {
                slog("error in file, position before entity");
                continue;// in case data is bad
            }
            fscanf(file,"%u",&spawn->endPosition);
            continue;
        }
        if(strcmp(buf,"autospawn:") == 0)
        {
            if (spawn < &info->spawnList[0])
            {
                slog("error in file, position before entity");
                continue;// in case data is bad
            }
            fscanf(file,"%u",&spawn->autospawn);
            continue;
        }
        if(strcmp(buf,"startPosition:") == 0)
        {
            if (spawn < &info->spawnList[0])
            {
                slog("error in file, position before entity");
                continue;// in case data is bad
            }
            fscanf(file,"%u",&spawn->startPosition);
            continue;
        }
        if(strcmp(buf,"positionVariance:") == 0)
        {
            if (spawn < &info->spawnList[0])
            {
                slog("error in file, position before entity");
                continue;// in case data is bad
            }
            fscanf(file,"%lf,%lf",&spawn->positionVariance.x,&spawn->positionVariance.y);
            continue;
        }
        fgets(buf, sizeof(buf), file);
    }
    return;
}

void level_info_free(LevelInfo *info)
{
    if (!info)return;
    if (info->spawnList != NULL)free(info->spawnList);
    free(info);
}

LevelInfo *level_info_load(char *filename)
{
    LevelInfo *info;
    FILE *file;
    int count;
    slog("loading level file: %s",filename);
    file = fopen(filename,"r");
    if (!file)
    {
        slog("failed to open file: %s",filename);
        return NULL;
    }
    info = (LevelInfo*)malloc(sizeof(LevelInfo));
    if (!info)
    {
        fclose(file);
        slog("failed to allocate memory for level: %s",filename);
        return NULL;
    }
    memset(info,0,sizeof(LevelInfo));
    count = level_info_get_spawn_count(file);
    if (count > 0)
    {
        info->spawnList = (Spawn*)malloc(sizeof(Spawn)*count);
        if (!info->spawnList)
        {
            slog("failed to allocate memory for spawn list");
            free(info);
            return NULL;
        }
    }
    slog("level contains %i spawn candidates",count);
    info->spawnCount = count;
    level_info_parse_file(file,info);
    fclose(file);
    slog("level %s loaded",info->name);
    return info;
}


void level_close()
{
    gf2d_sprite_free(level.backgroundImage);
    gf2d_particle_emitter_free(level.pe);
    if (level.backgroundMusic)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(level.backgroundMusic);
    }
    gf2d_space_free(level.space);
}

void level_start(LevelInfo *info)
{
    Rect cam;
    if (!info)return;
    level_close();
    level.backgroundMusic = Mix_LoadMUS(info->backgroundMusic);
    if (level.backgroundMusic != NULL)
    {
        Mix_PlayMusic(level.backgroundMusic,-1);
    }
    else
    {
        slog("failed to load music file %s, re: %s",info->backgroundMusic,Mix_GetError());
    }
    vector4d_copy(level.backgroundColor,info->backgroundColor);
    level.backgroundImage = gf2d_sprite_load_image(info->backgroundImage);
    
    vector4d_copy(level.starfield,info->starfield);
    vector4d_copy(level.starfieldVariance,info->starfieldVariance);
    level.starRate = info->starRate;
    level.starSpeed = info->starSpeed;
    
    gf2d_rect_copy(level.bounds,info->bounds);
    camera_set_bounds(level.bounds.x,level.bounds.y,level.bounds.w,level.bounds.h);

    cam = camera_get_dimensions();

    level.space = gf2d_space_new_full(
        3,
        level.bounds,
        0.1,
        vector2d(0,0),
        0,
        1);
    
    level.pe = gf2d_particle_emitter_new_full(
        10000,
        1200,
        0,
        PT_Pixel,
        vector2d(cam.w + 20,cam.h/2),
        vector2d(0,cam.h/2),
        vector2d(-level.starSpeed,0),
        vector2d(-0.1,0),
        vector2d(-0.1,0),
        vector2d(0.1,0),
        gf2d_color_from_vector4(level.starfield),
        gf2d_color(0,0,0,0),
        gf2d_color_from_vector4(level.starfieldVariance),
        NULL,
        0,
        0,
        0,
        "",
        0,
        0,
        0,
        0,
        SDL_BLENDMODE_ADD);
    level_spawn_entities(info);
}

void level_spawn_entities(LevelInfo *info)
{
    int i,j;
    Entity *ent;
    Vector2D startPosition;
    if (!info)return;
    /*spawn entities*/
    for (i = 0; i < info->spawnCount; i++)
    {
        if (info->spawnList[i].autospawn > 0)
        {
            for (j = 0; j < info->spawnList[i].autospawn;j++)
            {
                startPosition.x = info->spawnList[i].startPosition + (gf2d_random() * (info->spawnList[i].endPosition - info->spawnList[i].startPosition));
                startPosition.y = info->spawnList[i].position.y + gf2d_crandom()*info->spawnList[i].positionVariance.y;
                ent = spawn_entity(info->spawnList[i].name,startPosition);
                level_add_entity(ent);
            }
            continue;
        }
        startPosition.x = info->spawnList[i].position.x + gf2d_crandom()*info->spawnList[i].positionVariance.x;
        startPosition.y = info->spawnList[i].position.y + gf2d_crandom()*info->spawnList[i].positionVariance.y;
        ent = spawn_entity(info->spawnList[i].name,startPosition);
        level_add_entity(ent);
    }
}

int body_world_touch(Body *body, Collision *collision)
{
    Entity *self;
    if (!body)return 0;
    self = (Entity *)body->data;
    if (!self)return 0;
    return 0;
}

int body_body_touch(Body *self, Body *other, Collision *collision)
{
    Entity *selfEnt,*otherEnt;
    if ((!self)||(!other))return 0;
    selfEnt = (Entity *)self->data;
    if (!selfEnt)return 0;
    otherEnt = (Entity *)other->data;
    if (!otherEnt)return 0;
    if (selfEnt->touch)
    {
        slog("%s is touching %s",selfEnt->name,otherEnt->name);
        return selfEnt->touch(selfEnt,otherEnt);
    }
    return 0;
}

void level_remove_entity(Entity *ent)
{
    if (!ent)return;
    if (!level.space)
    {
        slog("cannot add entity %s to level, no space defined!",ent->name);
        return;
    }
    gf2d_space_remove_body(level.space,&ent->body);
}

void level_add_entity(Entity *ent)
{
    if (!ent)return;
    if (!level.space)
    {
        slog("cannot add entity %s to level, no space defined!",ent->name);
        return;
    }
    if (ent->body.bodyTouch == NULL)
    {
        ent->body.bodyTouch = body_body_touch;
    }
    gf2d_space_add_body(level.space,&ent->body);
}

Rect level_get_bounds()
{
    return level.bounds;
}

void level_update()
{
    gf2d_particle_new_default(
        level.pe,
        level.starRate);

    gf2d_particle_emitter_update(level.pe);

    gf2d_entity_pre_sync_all();
    gf2d_space_update(level.space);
    gf2d_entity_post_sync_all();
    
}

void level_draw()
{
    Vector2D drawPosition,aspect;
    Rect camera = camera_get_dimensions();

    if (level.backgroundImage != NULL)
    {
        aspect.x = (level.backgroundImage->frame_w - camera.w) / (level.bounds.w - camera.w);
        aspect.y = 1;

        drawPosition.x = level.backgroundOffset.x - (camera.x * aspect.x);
        drawPosition.y = level.backgroundOffset.y - (camera.y * aspect.y);
        
        SDL_SetRenderDrawBlendMode(gf2d_graphics_get_renderer(),SDL_BLENDMODE_ADD);
        
        gf2d_sprite_draw(
            level.backgroundImage,
            drawPosition,
            NULL,
            NULL,
            NULL,
            NULL,
            &level.backgroundColor,
            0);
        SDL_SetRenderDrawBlendMode(gf2d_graphics_get_renderer(),SDL_BLENDMODE_BLEND);
    }
    gf2d_particle_emitter_draw(level.pe);
//    gf2d_space_draw(level.space);
}

/*eol@eof*/
