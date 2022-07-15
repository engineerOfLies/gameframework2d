#include "simple_logger.h"

#include "gfc_config.h"

#include "gf2d_draw.h"
#include "gf2d_entity_common.h"

#include "monsters.h"
#include "level.h"
#include "player.h"
#include "exit.h"
#include "bat.h"

#include "spawn.h"

static Spawn spawnlist[] = 
{
    {
        "player_start",
        "actors/glyph.json",
        "idle",
        player_spawn,
        "entities/player.def"
    },
    {
        "player_enter",
        "actors/player_enter.json",
        "idle",
        NULL,
        "entities/player.def"
    },
    {
        "exit",
        "actors/exit.json",
        "default",
        exit_spawn,
        "entities/exit_def.json"
    },
    {
        "monster_bat",
        "actors/bat.actor",
        "idle",
        bat_spawn,
        "entities/bat.def"
    },
    {0}
};

static List *_spawnData = NULL;

void spawn_data_free()
{
    SpawnData *data;
    int i,c;
    if (!_spawnData)return;
    c = gfc_list_get_count(_spawnData);
    for (i = 0;i < c; i++)
    {
        data = gfc_list_get_nth(_spawnData,i);
        if (!data)continue;
        gf2d_actor_free(data->actor);
        if (data->entityDef)sj_free(data->entityDef);
        free(data);
    }
    gfc_list_delete(_spawnData);
    _spawnData = NULL;
}

void spawn_load_data()
{
    SpawnData *data;
    int i;
    if (_spawnData != NULL)return;
    _spawnData = gfc_list_new();
    for (i = 0; spawnlist[i].name != NULL;i++)
    {
        data = gfc_allocate_array(sizeof(SpawnData),1);
        if (spawnlist[i].actor != NULL)
        {
            data->actor = gf2d_actor_load(spawnlist[i].actor);
            data->action = gf2d_actor_set_action(data->actor, spawnlist[i].action,&data->frame);
            data->entityDef = sj_load(spawnlist[i].entityDef);
            data->shape = entity_config_parse_shape(sj_object_get_value(data->entityDef,"physics"));
        }
        data->index = i;
        data->spawn = &spawnlist[i];
        _spawnData = gfc_list_append(_spawnData,data);
    }
}

SpawnData *spawn_data_get_by_index(Uint32 index)
{
    int i,c;
    SpawnData *data = NULL;
    if (_spawnData == NULL)return NULL;
    c = gfc_list_get_count(_spawnData);
    for (i = 0;i < c; i++)
    {
        data = gfc_list_get_nth(_spawnData,i);
        if (!data)continue;
        if (data->index == index)return data;
    }
    return data;
}

Spawn *spawn_get_by_name(const char *name)
{
    int i;
    if (!name)return NULL;
    for (i = 0; spawnlist[i].name != NULL;i++)
    {
        if (strcmp(spawnlist[i].name,name)==0)return &spawnlist[i];
    }
    return NULL;
}

int spawn_get_index_by_name(const char *name)
{
    int i;
    if (!name)return -1;
    for (i = 0; spawnlist[i].name != NULL;i++)
    {
        if (strcmp(spawnlist[i].name,name)==0)return i;
    }
    return -1;
}

const char *spawn_get_name(SJson *spawn)
{
    if (!spawn)return NULL;
    return sj_get_string_value(sj_object_get_value(spawn,"name"));
}

const char *spawn_get_name_by_index(Uint32 index)
{
    SpawnData *data;
    if (!_spawnData)return NULL;
    data = gfc_list_get_nth(_spawnData,index);
    if (!data)return NULL;
    return data->spawn->name;
}

int spawn_get_count()
{
    if (!_spawnData)return 0;
    return gfc_list_get_count(_spawnData);
}

Shape spawn_get_shape(SpawnData *spawn)
{
    Shape shape = {0};
    if (!spawn)return shape;
    return spawn->shape;
}

Shape spawn_get_shape_by_index(Uint32 index)
{
    return spawn_get_shape(spawn_data_get_by_index(index));
}

Shape spawn_get_shape_by_name(const char *name)
{
    return spawn_get_shape_by_index(spawn_get_index_by_name(name));
}

void spawn_draw_spawn_outline(SpawnData *spawn, Vector2D position,Color color)
{
    if (!spawn)return;
    gf2d_draw_shape(spawn->shape,color,position);
}

void spawn_draw_spawn(SpawnData *spawn, Vector2D position)
{
    if (!spawn)return;
    gf2d_actor_draw(
        spawn->actor,
        spawn->frame,
        position,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    spawn_draw_spawn_outline(spawn, position,gfc_color8(255,0,255,255));
}


void spawn_draw_spawn_by_name(const char* spawn, Vector2D position)
{
    SpawnData *data;
    int index;
    index = spawn_get_index_by_name(spawn);
    if (index == -1)
    {
        slog("cannot draw %s, no info found",spawn);
        return;// not found
    }
    data = spawn_data_get_by_index(index);
    if (!data)
    {
        slog("cannot draw %s, no data found",spawn);
        return;
    }
    spawn_draw_spawn(data, position);
}

void spawn_draw_spawn_outline_by_name(const char* spawn, Vector2D position, Color color)
{
    SpawnData *data;
    int index;
    index = spawn_get_index_by_name(spawn);
    if (index == -1)
    {
        slog("cannot draw %s, no info found",spawn);
        return;// not found
    }
    data = spawn_data_get_by_index(index);
    if (!data)
    {
        slog("cannot draw %s, no data found",spawn);
        return;
    }
    spawn_draw_spawn_outline(data, position,color);
}

void spawn_entity(const char *name,Vector2D position,Uint32 id,SJson *args)
{
    Spawn *spawn;
    if (!name)
    {
        slog("no spawn name provided");
        return;
    }
    for (spawn = spawnlist; spawn->name != 0; spawn++)
    {
        if (strcmp(spawn->name,name)==0)
        {
            if (spawn->spawn)
            {
                spawn->spawn(position,id,args,spawn->entityDef);
                return;
            }
        }
    }
}


/*eol@eof*/
