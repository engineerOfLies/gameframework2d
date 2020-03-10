#include "simple_logger.h"
#include "simple_json.h"
#include "gf2d_config.h"
#include "player.h"

typedef struct
{
    TextLine    scene;
    Vector2D    position;
    SJson      *history;
}PlayerData;

void player_free(Entity *player)
{
    if (!player)
    {
        return;
    }
    if (player->data)
    {
        free(player->data);
        player->data = NULL;
    }
    gf2d_entity_free(player);
}

Entity *player_spawn(Vector2D position)
{
    Entity *player;
    PlayerData *pd = NULL;
    player = gf2d_entity_new();
    if (!player)return NULL;
    pd = (PlayerData*)gfc_allocate_array(sizeof(PlayerData),1);
    if (!pd)
    {
        slog("failed to allocate player data for player entity");
        gf2d_entity_free(player);
        return NULL;
    }
    gfc_line_cpy(player->name,"player");
    player->data = pd;
    vector2d_copy(player->position,position);
    gf2d_actor_load(&player->actor,"actors/player.json");
    gf2d_actor_set_action(&player->actor,"idle");
    return player;
}

void player_save(Entity *player)
{
    if (!player)return;
}

Entity *player_load(char *filename)
{
    Entity *player = NULL;
    PlayerData *pd = NULL;
    SJson *json;
    const char *str;
    Vector2D position = {0};
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to load player information from file %s",filename);
        return NULL;
    }
    sj_value_as_vector2d(sj_object_get_value(json,"position"),&position);
    player = player_spawn(position);
    if (!player)
    {
        sj_free(json);
        return NULL;
    }
    pd = (PlayerData*)player->data;
    str = sj_get_string_value(sj_object_get_value(json,"scene"));
    if (str)
    {
        gfc_line_cpy(pd->scene,str);
    }
    pd->history = sj_copy(sj_object_get_value(json,"history"));
    sj_free(json);
    return player;
}


/*eol@eof*/
