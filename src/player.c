#include "simple_logger.h"
#include "simple_json.h"
#include "gf2d_config.h"
#include "camera.h"
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

void player_idle(Entity *self)
{
    if (!self)return;
}

int player_near_point(Entity *self,Vector2D position)
{
    if (!self)return 0;
    if (vector2d_magnitude_between(self->position,position) <= self->walkingSpeed)return 1;
    return 0;
}

int player_near_scene_point(Entity *player,Vector2D position)
{
    Rect r;
    if (!player)return 0;
    r = gf2d_shape_get_bounds(player->shape);
    position.y -= r.h;
    position.x -= (r.w/2);
    return player_near_point(player,position);
}

void player_walk(Entity *self)
{
    Vector2D direction;
    if (!self)return;
    camera_set_focus(self->position);
    if (player_near_point(self,self->targetPosition))
    {
        vector2d_copy(self->position,self->targetPosition);
        self->state = ES_Idle;
        gf2d_actor_set_action(&self->actor,"idle");
        vector2d_set(self->velocity,0,0);
        self->update = player_idle;
        player_run_callback(self);
        return;
    }
    vector2d_sub(direction,self->targetPosition,self->position);
    vector2d_set_magnitude(&direction,self->walkingSpeed);
    vector2d_copy(self->velocity,direction);
    if (self->velocity.x < 0)self->flip.x = 1;
    if (self->velocity.x > 0)self->flip.x = 0;
}

Entity *player_spawn(Vector2D position)
{
    Entity *player;
    PlayerData *pd = NULL;
    player = gf2d_entity_load("config/testplayerent.json");
    if (!player)return NULL;
    pd = (PlayerData*)gfc_allocate_array(sizeof(PlayerData),1);
    if (!pd)
    {
        slog("failed to allocate player data for player entity");
        gf2d_entity_free(player);
        return NULL;
    }
    player->update = player_idle;
    player->data = pd;
    vector2d_copy(player->position,position);
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

void player_walk_to(Entity *player,Vector2D position)
{
    Rect r;
    if (!player)return;
    player->state = ES_Walking;
    player->update = player_walk;
    vector2d_copy(player->targetPosition,position);
    r = gf2d_shape_get_bounds(player->shape);
    player->targetPosition.y -= r.h;
    player->targetPosition.x -= (r.w/2);
    gf2d_actor_set_action(&player->actor,"walk");
}

void player_run_callback(Entity *player)
{
    Callback *call;
    if (!player)return;
    if (!player->nextAction)return;
    call = player->nextAction;
    player->nextAction = NULL;
    gfc_callback_call(call);//just in case the callback sets the player's next callback
    gfc_callback_free(call);
}

void player_set_callback(Entity *player,void(*call)(void *),void *data)
{
    if (!player)return;
    if (player->nextAction)
    {
        slog("callback already set, overwriting");
        gfc_callback_free(player->nextAction);
    }
    player->nextAction = gfc_callback_new(call,data);
}

/*eol@eof*/
