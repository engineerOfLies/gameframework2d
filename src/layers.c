#include "simple_logger.h"

#include "gf2d_config.h"

#include "camera.h"
#include "layers.h"

Layer *layer_new()
{
    Layer *layer;
    layer = gfc_allocate_array(sizeof(Layer),1);
    layer->playerScale.x = layer->playerScale.y = 1;
    return layer;
}

SJson *layer_save_to_json(Layer *layer)
{
    SJson *json;
    Action *action;
    if (!layer)return NULL;
    
    json = sj_object_new();
    if (!json)return NULL;

    if (layer->actor.al != NULL)
    {
        sj_object_insert(json,"actor",sj_new_str(layer->actor.al->filename));
        action = gf2d_actor_get_current_action(&layer->actor);
        if (action != NULL)
        {
            sj_object_insert(json,"action",sj_new_str(action->name));
        }
    }
    sj_object_insert(json,"parallax",sj_new_float(layer->parallax));
    sj_object_insert(json,"layerBegin",sj_new_int(layer->layerBegin));
    sj_object_insert(json,"playerScale",sj_vector2d_new(layer->playerScale));
    
    return json;
}

Layer *layer_load_from_json(SJson *json)
{
    char *imageName;
    Layer *layer;
    float tempFloat;
    int   tempInt;
    if (!json)return NULL;
    layer = layer_new();
    if (!layer)return NULL;
    
    imageName = (char *)sj_get_string_value(sj_object_get_value(json,"actor"));
    if ((imageName)&&(gf2d_actor_load(&layer->actor,imageName)))
    {
        imageName = (char *)sj_get_string_value(sj_object_get_value(json,"action"));
        if (imageName)
        {
            gfc_line_cpy(layer->action,imageName);
            gf2d_actor_set_action(&layer->actor,imageName);
        }
    }
    if (sj_get_float_value(sj_object_get_value(json,"parallax"),&tempFloat))
    {
        layer->parallax = tempFloat;
    }

    if (sj_get_integer_value(sj_object_get_value(json,"layerBegin"),&tempInt))
    {
        layer->layerBegin = tempInt;
    }
    
    sj_value_as_vector2d(sj_object_get_value(json,"playerScale"),&layer->playerScale);
    
    return layer;
}

void layer_free(Layer *layer)
{
    if (!layer)return;
    gf2d_actor_free(&layer->actor);
    free(layer);
}

void layer_draw(Layer *layer)
{
    Vector2D drawPosition;
    
    if (!layer)return;
    
    drawPosition = camera_get_offset();
    vector2d_scale(drawPosition,drawPosition,1 - layer->parallax);
    if (!layer)return;
    gf2d_actor_draw(
        &layer->actor,
        drawPosition,
        NULL,
        NULL,
        NULL,
        NULL);
}

/*eol@eof*/
