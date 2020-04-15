#include "simple_logger.h"
#include "simple_json.h"
#include "gf2d_config.h"
#include "camera.h"
#include "scene.h"
#include "inventory.h"
#include "player.h"

typedef struct
{
    TextLine    filename;   /**<player save file*/
    TextLine    scene;
    Vector2D    position;
    Inventory  *items;
    Inventory  *spells;
    Inventory  *skills;
    SJson      *history;
}PlayerData;


void player_set_scene(Entity *player,char *scene)
{
    PlayerData *pd;
    if ((!player)||(!scene))return;
    pd = player->data;
    if (!pd)return;
    gfc_line_cpy(pd->scene,scene);
}

void player_update(Entity *player);

void player_free(Entity *player)
{
    PlayerData *pd;
    if (!player)
    {
        return;
    }
    pd = player->data;
    if (pd)
    {
        inventory_free(pd->items);
        inventory_free(pd->skills);
        inventory_free(pd->spells);
        sj_free(pd->history);
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
    if (!player)return 0;
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
        self->think = player_idle;
        player_run_callback(self);
        return;
    }
    vector2d_sub(direction,self->targetPosition,self->position);
    vector2d_set_magnitude(&direction,self->walkingSpeed);
    vector2d_copy(self->velocity,direction);
    if (self->velocity.x < 0)self->flip.x = 1;
    if (self->velocity.x > 0)self->flip.x = 0;
}

Entity *player_new()
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
    gfc_line_cpy(pd->filename,"saves/");
    player->think = player_idle;
    player->data = pd;
    player->update = player_update;
    player->drawColor = gfc_color(1,0,0,1);
    pd->items = inventory_new();
    pd->spells = inventory_new();
    pd->skills = inventory_new();
    return player;
}

Entity *player_spawn(Vector2D position,char *scene)
{
    PlayerData *pd = NULL;
    Entity *player;
    player = player_new();
    if (!player)return NULL;
    pd = player->data;
    vector2d_copy(player->position,position);
    gfc_line_cpy(pd->scene,scene);
    return player;
}

void player_set_position(Entity *player,Vector2D position)
{
    if (!player)return;
    vector2d_copy(player->position,position);
}

SJson *player_to_json(Entity *player)
{
    SJson *json;
    PlayerData *pd = NULL;
    if (!player)return NULL;
    
    json = sj_object_new();
    if (!json)return NULL; 
    
    pd = (PlayerData*)player->data;
    vector2d_copy(pd->position,player->position);

    sj_object_insert(json,"position",sj_vector2d_new(pd->position));
    sj_object_insert(json,"scene",sj_new_str(pd->scene));
    sj_object_insert(json,"history",sj_copy(pd->history));
    sj_object_insert(json,"items",inventory_to_json(pd->items));
    sj_object_insert(json,"skills",inventory_to_json(pd->skills));
    sj_object_insert(json,"spells",inventory_to_json(pd->spells));
    
    return json;
}

void player_save(Entity *player,char *filename)
{
    SJson *json;
    if (!player)return;
    
    json = player_to_json(player);
    if (!json)return;
    sj_save(json,filename);
}

Entity *player_load(char *filename)
{
    SJson *json;
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to load player information from file %s",filename);
        return NULL;
    }
    return player_parse_from_json(json);
}

Entity *player_parse_from_json(SJson *json)
{
    Entity *player = NULL;
    PlayerData *pd = NULL;
    SJson *inven;
    const char *str;
    Vector2D position = {0};
    sj_value_as_vector2d(sj_object_get_value(json,"position"),&position);
    str = sj_get_string_value(sj_object_get_value(json,"scene"));
    if (str)
    {
        player = player_spawn(position,(char *)str);
        if (!player)
        {
            sj_free(json);
            return NULL;
        }
        pd = (PlayerData*)player->data;
        gfc_line_cpy(pd->scene,str);
    }
    
    inven = sj_object_get_value(json,"items");
    pd->items = inventory_parse_from_json(inven);
    
    inven = sj_object_get_value(json,"skills");
    pd->items = inventory_parse_from_json(inven);
    
    inven = sj_object_get_value(json,"spells");
    pd->items = inventory_parse_from_json(inven);
    
    pd->history = sj_copy(sj_object_get_value(json,"history"));
    sj_free(json);
    return player;
}

void player_walk_to(Entity *player,Vector2D position)
{
    if (!player)return;
    player->state = ES_Walking;
    player->think = player_walk;
    vector2d_copy(player->targetPosition,position);
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

void player_update(Entity *player)
{
    Scene *scene;
    Layer *layer;
    if (!player)return;
    scene = scene_get_active();
    if (!scene)return;
    layer = scene_get_layer_by_position(scene, player->position);
    if (layer)
    {
        player->drawLayer = layer->index;
        vector2d_copy(player->scale,layer->playerScale);
    }
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

Inventory *player_get_item_inventory(Entity *player)
{
    PlayerData *pd;
    if (!player)return NULL;
    pd = player->data;
    return pd->items;
}

char *player_get_scene(Entity *player)
{
    PlayerData *pd;
    if (!player)return NULL;
    pd = player->data;
    return pd->scene;
}

char *player_get_filename(Entity *player)
{
    PlayerData *pd;
    if (!player)return NULL;
    pd = player->data;
    return pd->filename;
}
/*eol@eof*/
