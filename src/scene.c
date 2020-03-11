#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_graphics.h"

#include "exhibits.h"
#include "scene.h"
#include "camera.h"

typedef struct
{
    Scene * sceneList;
    Uint32  sceneMax;
}SceneManager;

static SceneManager scene_manager = {0};

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

void scene_update(Scene *scene)
{
    Exhibit *exhibit;
    int i, count;
    if (!scene)
    {
        slog("no scene provided to update");
        return;
    }
    if (!gf2d_mouse_button_released(0))return;
    count = gfc_list_get_count(scene->exhibits);
    for (i = 0; i < count; i++)
    {
        exhibit = gfc_list_get_nth(scene->exhibits,i);
        if (!exhibit)continue;
        if (exhibit_mouse_check(exhibit))return;
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


/*eol@eof*/
