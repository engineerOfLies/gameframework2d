#ifndef __SCENE_H__
#define __SCENE_H__

#include "simple_json.h"
#include "gfc_types.h"

#include "gf2d_sprite.h"

typedef struct
{
    Uint8      _inuse;              /**<no touchy*/
    Sprite     *background;         /**<background image*/
    Sprite     *mask;               /**<an image used to determine what positions are walk-able*/
    Vector2D    camera;             /**<draw postion for the scene*/
    SJson      *config;             /**<additional configuration for the scene*/
}Scene;


/**
 * @brief initialize scene manager subsystem
 * @param sceneMax how many scenes to support in memory at once
 */
void scene_manager_init(Uint32 sceneMax);

/**
 * @brief load a scene from a json file
 * @param filename the name of the file to load
 * @returns NULL on error or a valid scene pointer
 * @note: free with scene_free()
 */
Scene *scene_load(char *filename);

/**
 * @brief get a pointer to a new plank scene
 * @returns NULL on error, or a pointer to an empty scene
 */
Scene *scene_new();

/**
 * @brief draw the scene
 * @param scene the scene to draw
 */
void scene_draw(Scene *scene);

/**
 * @brief frees a scene from memory
 * @param scene the scene to free
 */
void scene_free(Scene *scene);

/**
 * @brief center the camera on this point in the scene
 * @param scene the scene to focus within.
 * @param point the point to focus on.
 */
void scene_camera_focus(Scene *scene, Vector2D point);

/**
 * @brief Move the camera in the direction specified
 * @param scene the scene to focus within.
 * @param direction the vector to move the camera by
 */
void scene_camera_move(Scene *scene, Vector2D direction);

#endif
