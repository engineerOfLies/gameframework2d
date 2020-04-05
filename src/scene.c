#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_graphics.h"

#include "scene.h"
#include "camera.h"
#include "player.h"
#include "exhibits.h"
#include "hud.h"

extern int editorMode;
extern int debugMode;

typedef struct
{
    Scene * sceneList;
    Uint32  sceneMax;
}SceneManager;

static SceneManager scene_manager = {0};

static Scene *_active_scene = NULL;

void scene_manager_close()
{
    int i;
    for (i = 0; i < scene_manager.sceneMax; i++)
    {
        scene_free(&scene_manager.sceneList[i]);
    }
    if (scene_manager.sceneList)
    {
        free(scene_manager.sceneList);
    }
    memset(&scene_manager,0,sizeof(SceneManager));
}

void scene_manager_init(Uint32 sceneMax)
{
    if (sceneMax == 0)
    {
        slog("cannot allocate 0 scenes");
        return;
    }
    scene_manager.sceneList = (Scene*)gfc_allocate_array(sizeof(Scene),sceneMax);
    if (scene_manager.sceneList)
    {
        scene_manager.sceneMax = sceneMax;    
    }
    atexit(scene_manager_close);
}

Scene *scene_new()
{
    int i;
    for (i = 0; i < scene_manager.sceneMax; i++)
    {
        if (scene_manager.sceneList[i]._inuse)continue;
        scene_manager.sceneList[i]._inuse = 1;
        scene_manager.sceneList[i].walkmasks = gfc_list_new();
        scene_manager.sceneList[i].exhibits = gfc_list_new();
        scene_manager.sceneList[i].entities = gfc_list_new();
        scene_manager.sceneList[i].layers = gfc_list_new();
        return &scene_manager.sceneList[i];
    }
    slog("failed to find a free scene in memory");
    return NULL;
}

void scene_add_exhibit(Scene *scene,Exhibit *exhibit)
{
    if ((!scene)||(!exhibit))return;
    scene->exhibits = gfc_list_append(scene->exhibits,exhibit);
    exhibit_set_scene(exhibit,scene);
}

void scene_add_walkmask(Scene *scene,Walkmask *mask)
{
    if ((!scene)||(!mask))return;
    scene->walkmasks = gfc_list_append(scene->walkmasks,mask);
}

void scene_add_layer(Scene *scene,Layer *layer)
{
    if ((!scene)||(!layer))return;
    scene->layers = gfc_list_append(scene->layers,layer);
    layer->index = gfc_list_get_item_index(scene->layers,layer);
    slog("layer set to index %i",layer->index);
}

void scene_add_entity(Scene *scene, Entity *entity)
{
    if ((!scene)||(!entity))return;
    scene->entities = gfc_list_append(scene->entities,entity);
}

void scene_save(Scene *scene, char *filename)
{
    SJson *json = NULL, *array = NULL, *item = NULL;
    Exhibit *exhibit = NULL;
    Walkmask *mask = NULL;
    Layer *layer = NULL;
    int i,count;
    if ((!scene)||(!filename))
    {
        slog("missing scene or filename for scene_save");
        return;
    }
    
    json = sj_object_new();
    if (!json)
    {
        slog("failed to make base json object for file safe");
        return;
    }
    array = sj_array_new();
    sj_object_insert(json,"exhibits",array);
    count = gfc_list_get_count(scene->exhibits);
    for (i = 0; i < count;i++)
    {
        exhibit = (Exhibit*)gfc_list_get_nth(scene->exhibits,i);
        if (!exhibit)continue;
        item = exhibit_to_json(exhibit);
        if (!item)continue;
        sj_array_append(array,item);
    }
    array = sj_array_new();
    sj_object_insert(json,"walkmasks",array);
    count = gfc_list_get_count(scene->walkmasks);
    for (i = 0; i < count;i++)
    {
        mask = (Walkmask*)gfc_list_get_nth(scene->walkmasks,i);
        if (!mask)continue;
        item = walkmask_to_json(mask);
        if (!item)continue;
        sj_array_append(array,item);
    }
    array = sj_array_new();
    sj_object_insert(json,"layers",array);
    count = gfc_list_get_count(scene->layers);
    for (i = 0; i < count;i++)
    {
        layer = (Layer*)gfc_list_get_nth(scene->layers,i);
        if (!layer)continue;
        item = layer_save_to_json(layer);
        if (!item)continue;
        sj_array_append(array,item);
    }

    sj_save(json,filename);
    
    sj_free(json);
}

Scene *scene_load(char *filename)
{
    Scene *scene;
    SJson *json;
    SJson *exhibits,*exhibitjs;
    SJson *walkmasks,*maskjs;
    SJson *layers,*layerjs;
    Exhibit *exhibit;
    Walkmask *walkmask;
    Layer *layer;
    int i,count;    
    
    scene = scene_new();
    if (!scene)return NULL;
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to load scene file %s",filename);
        scene_free(scene);
        return NULL;
    }
    gfc_line_cpy(scene->filename,filename);
    exhibits = sj_object_get_value(json,"exhibits");
    if (exhibits)
    {
        count = sj_array_get_count(exhibits);
        slog("%i exhibits found",count);
        for (i = 0;i < count; i++)
        {
            exhibitjs = sj_array_get_nth(exhibits,i);
            if (!exhibitjs)continue;
            exhibit = exhibit_load(exhibitjs);
            if (exhibit)
            {
                scene_add_exhibit(scene,exhibit);
            }
        }
    }
    walkmasks = sj_object_get_value(json,"walkmasks");
    if (walkmasks)
    {
        count = sj_array_get_count(walkmasks);
        for (i = 0;i < count; i++)
        {
            maskjs = sj_array_get_nth(walkmasks,i);
            if (!maskjs)continue;
            walkmask = walkmask_load_from_json(maskjs);
            if (walkmask)
            {
                scene_add_walkmask(scene,walkmask);
            }
        }
    }

    layers = sj_object_get_value(json,"layers");
    if (layers)
    {
        count = sj_array_get_count(layers);
        for (i = 0;i < count; i++)
        {
            layerjs = sj_array_get_nth(layers,i);
            if (!layerjs)continue;
            layer = layer_load_from_json(layerjs);
            if (layer)
            {
                scene_add_layer(scene,layer);
            }
        }
    }
    layer = gfc_list_get_nth(scene->layers,0);
    if (layer)
    {
        camera_set_bounds(0,0,layer->actor.size.x,layer->actor.size.y);
    }

    scene->config = json;
    return scene;
}

void scene_spawn_exhibits(Scene *scene)
{
    Exhibit *exhibit;
    int i,count;
    
    if (!scene)return;
    count = gfc_list_get_count(scene->exhibits);
    for (i = 0; i < count; i++)
    {
        exhibit = gfc_list_get_nth(scene->exhibits,i);
        if (!exhibit)continue;
        scene_add_entity(scene, exhibit_entity_spawn(exhibit));
    }
}

void scene_draw(Scene *scene)
{
    int i,c;
    Walkmask *mask;
    Layer *layer;
    if (!scene)return;
    c = gfc_list_get_count(scene->layers);
    for (i = 0; i < c; i++)
    {
        layer = (Layer*)gfc_list_get_nth(scene->layers,i);
        if (layer)layer_draw(layer);
        gf2d_entity_draw_all_by_layer(i);
    }
    if ((debugMode)||(editorMode))
    {
        c = gfc_list_get_count(scene->walkmasks);
        for (i = 0;i < c; i++)
        {
            mask = (Walkmask*)gfc_list_get_nth(scene->walkmasks,i);
            if (!mask)continue;
            walkmask_draw(mask,gfc_color(0.6,0,0.6,1),vector2d(0,0));
        }
    }
}

Entity *scene_get_active_player(Scene *scene)
{
    if (!scene)return NULL;
    return scene->activePlayer;
}

void scene_set_active_player(Scene *scene,Entity *player)
{
    if ((!scene)||(!player))return;
    scene->activePlayer = player;
}

void scene_active_player_walk_to(Scene *scene,Vector2D position)
{
    Vector2D walkPosition;
    if ((!scene) || (!scene->activePlayer))return;
    scene_walk_check(scene,scene->activePlayer->position, position,&walkPosition);

    player_walk_to(scene->activePlayer,walkPosition);
}

void scene_update(Scene *scene)
{
    Exhibit *exhibit;
    int i, count;
    Vector2D destination,offset;
    if (!scene)
    {
        slog("no scene provided to update");
        return;
    }
    if (!gf2d_mouse_button_released(0))return;
    if (hud_mouse_check())return;
    
    //get the mouse click position in scene relative space
    destination = gf2d_mouse_get_position();
    offset = camera_get_position();
    vector2d_add(destination,destination,offset);
    
    count = gfc_list_get_count(scene->exhibits);
    for (i = 0; i < count; i++)
    {
        exhibit = gfc_list_get_nth(scene->exhibits,i);
        if (!exhibit)continue;
        if (exhibit_mouse_check(exhibit))return;
    }
    if (gf2d_mouse_get_function() == MF_Walk)
    {

        scene_active_player_walk_to(scene,destination);
    }
}

Layer *scene_get_background_layer(Scene *scene)
{
    if (!scene)return NULL;
    return gfc_list_get_nth(scene->layers,0);
}

Layer *scene_get_layer_by_position(Scene *scene, Vector2D point)
{
    int i,c;
    Layer *layer;
    Layer *chosenLayer = NULL;
    if (!scene)return NULL;
    c = gfc_list_get_count(scene->layers);
    for (i = 0; i < c; i++)
    {
        layer = gfc_list_get_nth(scene->layers,i);
        if (!layer)continue;
        if (point.y >= layer->layerBegin)
        {
            if ((chosenLayer == NULL) || (chosenLayer->layerBegin < layer->layerBegin))
            {
                chosenLayer = layer;
            }
        }
    }
    return chosenLayer;
}
void scene_free(Scene *scene)
{
    int count,i;
    Exhibit *exhibit;
    Entity *ent;
    Walkmask *mask;
    Layer *layer;
    if (!scene)return;

    count = gfc_list_get_count(scene->walkmasks);
    for (i = 0; i < count; i++)
    {
        mask = gfc_list_get_nth(scene->walkmasks,i);
        if (!mask)continue;
        walkmask_free(mask);
    }
    gfc_list_delete(scene->walkmasks);
    count = gfc_list_get_count(scene->exhibits);
    for (i = 0; i < count; i++)
    {
        exhibit = gfc_list_get_nth(scene->exhibits,i);
        if (!exhibit)continue;
        exhibit_free(exhibit);
    }
    gfc_list_delete(scene->exhibits);
    count = gfc_list_get_count(scene->entities);
    for (i = 0; i < count; i++)
    {
        ent = gfc_list_get_nth(scene->entities,i);
        if (!ent)continue;
        gf2d_entity_free(ent);
    }
    gfc_list_delete(scene->entities);
    count = gfc_list_get_count(scene->layers);
    for (i = 0; i < count; i++)
    {
        layer = gfc_list_get_nth(scene->layers,i);
        if (!layer)continue;
        layer_free(layer);
    }
    gfc_list_delete(scene->layers);
    sj_free(scene->config);
    memset(scene,0,sizeof(Scene));
}


void scene_set_active(Scene *scene)
{
    _active_scene = scene;
}

Scene *scene_get_active()
{
    return _active_scene;
}


void scene_next_scene(char *nextScene, Entity *player, char *positionExhibit)
{
    Exhibit *exhibit;
    Scene *scene;
    if (!player)
    {
        slog("no player entity!");
        return;
    }
    
    
    //load the new scene
    scene = scene_load(nextScene);
    scene_set_active_player(scene,player);
    
    scene_spawn_exhibits(scene);
    
    exhibit = exhibit_get_from_scene(scene,positionExhibit);
    if (!exhibit)
    {
        slog("failed to find exhibit %s, cannot place player!",positionExhibit);
        return;
    }

    player_set_position(player,exhibit->near);
    camera_set_focus(player->position);
    
    //free up the last scene
    scene_free(scene_get_active());
    scene_set_active(scene);
}

Walkmask *scene_get_walkmask_by_point(Scene *scene, Vector2D point)
{
    Walkmask *mask = NULL;
    Walkmask *best = NULL;
    float distance,bestDistance;
    PointData *maskPoint;

    int i,c;
    if (!scene)return NULL;
    c = gfc_list_get_count(scene->walkmasks);
    for (i = 0;i < c; i++)
    {
        mask = (Walkmask *)gfc_list_get_nth(scene->walkmasks,i);
        if (!mask)continue;
        if (!walkmask_point_in_check(mask, point))continue;
        
        maskPoint = walkmask_get_nearest_point(mask, point);
        if (!maskPoint)continue;
        distance = vector2d_magnitude_between(maskPoint->position,point);
        if ((!best)||(distance < bestDistance))
        {
            bestDistance = distance;
            best = mask;
        }
    }
    return best;//not found
}

int scene_walk_check(Scene *scene,Vector2D start, Vector2D end,Vector2D *contact)
{
    Walkmask *mask = NULL;
    Vector2D closestPoint,checkPoint;
    int i,c,clipped = 0;
    float    distance, bestDistance = -1;
    // check walkmask and any exhibits that may be in the way to see if a player can walk to the desired point
    if (!scene)return 0;
    c = gfc_list_get_count(scene->walkmasks);
    vector2d_copy(checkPoint,end);
    for (i = 0; i < c;i++)
    {
        mask = (Walkmask*)gfc_list_get_nth(scene->walkmasks,i);
        if (!mask)continue;
        if (walkmask_edge_clip(mask,start, end,&checkPoint))
        {
            clipped = 1;
            distance = vector2d_magnitude_between(start,checkPoint);
            if ((bestDistance == -1)||(distance < bestDistance))
            {
                bestDistance = distance;
                vector2d_copy(closestPoint,checkPoint);
            }
        }
    }
    if (contact)
    {
        if (clipped)
        {
            vector2d_move_towards(contact, closestPoint, start, 2);
        }
        else
        {
            vector2d_move_towards(contact, end, start, 2);
        }
    }
    return clipped;
}


/*eol@eof*/
