#ifndef __LAYERS_H__
#define __LAYERS_H__

#include "simple_json.h"

#include "gfc_types.h"

#include "gf2d_actor.h"

typedef struct
{
    Uint32      index;      /**<layer index is set internally*/
    Actor       actor;      /**<if this layer has an actor to draw as a background image that the player an be drawn behind*/
    TextLine    action;     /**<which action the actor should be set to*/
    float       parallax;       /**<if this layer should move with the camera, range 0 (does not move) to 1 (moves with camera, like UI).*/
    int         layerBegin;     /**<at what Y position does this a player enter this layer?*/
    Vector2D    playerScale;    /**<scale the player to this size when in this layer.  to help with the illusion of depth*/
}Layer;

/**
 * @brief allocate a new layer, initialized to zero
 * @return NULL on error or a new zeroed layer otherwise
 */
Layer *layer_new();

/**
 * @brief allocate and load a layer based on json config
 * @param json the json to configure by
 * @return NULL on error (see logs) or a configured new layer
 */
Layer *layer_load_from_json(SJson *json);

/**
 * @brief save a layer to json for writing to actor
 * @param layer the layer to save
 * @return NULL on error a json object.  Note: must be free'd
 */
SJson *layer_save_to_json(Layer *layer);

/**
 * @brief free a previously allocated layer
 * @param layer the layer to free, if NULL this is a no-op
 */
void layer_free(Layer *layer);

/**
 * @brief draw a layer's actor
 * @param layer the layer to draw
 */
void layer_draw(Layer *layer);

#endif
