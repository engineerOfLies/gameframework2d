#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_graphics.h"

#include "scene.h"

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
        return &scene_manager.sceneList[i];
    }
    slog("failed to find a free scene in memory");
    return NULL;
}

Scene *scene_load(char *filename)
{
    Scene *scene;
    SJson *json;
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
    scene->background = gf2d_sprite_load_image(imageName);
    imageName = (char *)sj_get_string_value(sj_object_get_value(json,"mask"));
    scene->mask = gf2d_sprite_load_image(imageName);
    scene->config = json;
    return scene;
}

void scene_draw(Scene *scene)
{
    Vector2D ncam;
    if (!scene)return;
    vector2d_negate(ncam,scene->camera);
    gf2d_sprite_draw_image(scene->background,ncam);
}

void scene_free(Scene *scene)
{
    if (!scene)return;
    gf2d_sprite_free(scene->background);
    gf2d_sprite_free(scene->mask);
    sj_free(scene->config);
    memset(scene,0,sizeof(Scene));
}

void scene_camera_clamp(Scene *scene)
{
    Vector2D extent;
    if (!scene)return;
    if (!scene->background)return;
    extent = gf2d_graphics_get_resolution();
    if (scene->camera.x < 0)scene->camera.x = 0;
    if (scene->camera.x + extent.x > scene->background->frame_w)scene->camera.x = scene->background->frame_w - extent.x;
    if (scene->camera.y < 0)scene->camera.y = 0;
    if (scene->camera.y + extent.y > scene->background->frame_h)scene->camera.y = scene->background->frame_h - extent.y;    
}

void scene_camera_move(Scene *scene, Vector2D direction)
{
    if (!scene)return;
    vector2d_add(scene->camera,scene->camera,direction);
    scene_camera_clamp(scene);
}

void scene_camera_focus(Scene *scene, Vector2D point)
{
    Vector2D extent;
    if (!scene)return;
    extent = gf2d_graphics_get_resolution();
    slog("window extent: %f,%f",extent.x,extent.y);
    scene->camera.x = point.x - (extent.x / 2);
    scene->camera.y = point.y - (extent.y / 2);
    scene_camera_clamp(scene);
}

/*eol@eof*/
