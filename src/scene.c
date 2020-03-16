#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_graphics.h"

#include "scene.h"
#include "camera.h"
#include "player.h"
#include "exhibits.h"

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
        scene_manager.sceneList[i].exhibits = gfc_list_new();
        scene_manager.sceneList[i].entities = gfc_list_new();
        return &scene_manager.sceneList[i];
    }
    slog("failed to find a free scene in memory");
    return NULL;
}

Scene *scene_load(char *filename)
{
    Scene *scene;
    SJson *json;
    SJson *exhibits,*exhibitjs;
    Exhibit *exhibit;
    int i,count;
    char *imageName;
    
    
    scene = scene_new();
    if (!scene)return NULL;
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to load scene file %s",filename);
        scene_free(scene);
        return NULL;
    }
    imageName = (char *)sj_get_string_value(sj_object_get_value(json,"background"));
    if (gf2d_actor_load(&scene->background,imageName))
    {
        imageName = (char *)sj_get_string_value(sj_object_get_value(json,"action"));
        gf2d_actor_set_action(&scene->background,imageName);
    }
    
    camera_set_bounds(0,0,scene->background.size.x,scene->background.size.y);
    exhibits = sj_object_get_value(json,"exhibits");
    if (exhibits)
    {
        count = sj_array_get_count(exhibits);
        for (i = 0;i < count; i++)
        {
            exhibitjs = sj_array_get_nth(exhibits,i);
            if (!exhibitjs)continue;
            exhibit = exhibit_load(exhibitjs);
            if (exhibit)
            {
                scene->exhibits = gfc_list_append(scene->exhibits,exhibit);
                exhibit_set_scene(exhibit,scene);
            }
        }
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
        scene->entities = gfc_list_append(scene->entities,exhibit_entity_spawn(exhibit));
    }
}

void scene_draw(Scene *scene)
{
    if (!scene)return;
    gf2d_actor_draw(
        &scene->background,
        camera_get_offset(),
        NULL,
        NULL,
        NULL,
        NULL);
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

void scene_add_entity(Scene *scene,Entity *entity)
{
    if ((!scene)||(!entity))return;
    gfc_list_append(scene->entities,entity);
}

void scene_active_player_walk_to(Scene *scene,Vector2D position)
{
    if (!scene)return;
    player_walk_to(scene->activePlayer,position);
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
    
    //get the mouse click position in scene relative space
    destination = gf2d_mouse_get_position();
    offset = camera_get_position();
    vector2d_add(destination,destination,offset);
    
    if (!(gf2d_point_in_rect(destination,gf2d_rect(0,0,scene->background.size.x,scene->background.size.y))))
    {
        return;// clicked out of the scene
    }

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


void scene_free(Scene *scene)
{
    int count,i;
    Exhibit *exhibit;
    Entity *ent;
    if (!scene)return;
    gf2d_actor_free(&scene->background);
    
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


/*eol@eof*/
