#include "simple_logger.h"

#include "gf2d_elements.h"
#include "gf2d_element_label.h"
#include "gf2d_element_entry.h"
#include "gf2d_mouse.h"
#include "gf2d_draw.h"

#include "camera.h"
#include "windows_common.h"
#include "actor_editor.h"
#include "layer_editor.h"
#include "actor_editor.h"
#include "editor.h"

typedef enum
{
    LM_Default,
    LM_Begin,
    LM_Placement,
    LM_MAX
}LayerModes;

typedef struct
{
    Layer          *layer;
    Scene          *scene;
    LayerModes      mode;
    Vector2D        click;
    Vector2D        delta;
    Window         *parent;
    TextLine        actionName;
    TextLine        actorName;
    TextLine        scaleStr;
    Window         *subwindow;
}LayerData;

void level_editor_set_actor(Window *win);


void onBackgroundActorChange(void *data)
{
    Window *win;
    LayerData *layer_data;
    Layer *bgLayer = NULL;
    Vector2D zero = {0};
    if (!data)return;
    win = data;
    if (!win)return;
    layer_data = win->data;
    if (!layer_data)return;
    gf2d_actor_free(&layer_data->layer->actor);
    gfc_line_cpy(layer_data->layer->action,layer_data->actionName);
    if (gf2d_actor_load(&layer_data->layer->actor,layer_data->actorName))
    {
        gf2d_actor_set_action(&layer_data->layer->actor,layer_data->actionName);
        bgLayer = scene_get_background_layer(layer_data->scene);
        if (bgLayer == layer_data->layer)
        {
            camera_set_bounds(0,0,layer_data->layer->actor.size.x,layer_data->layer->actor.size.y);
            camera_set_focus(zero);
        }
    }
    level_editor_set_actor(win);
    layer_data->subwindow = NULL;
}

void onBackgroundCancel(void *data)
{
    Window *win;
    LayerData *layer_data;
    if (!data)return;
    win = data;
    if (!win)return;
    layer_data = win->data;
    if (!layer_data)return;
    layer_data->subwindow = NULL;
}


void layer_editor_set_mode(Window *win,LayerModes mode)
{
    if (!win)return;
    switch (mode)
    {
        case LM_MAX:
        case LM_Default:
            gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),"Mode: View");
            break;
        case LM_Placement:
            gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),"Mode: Placement");
            break;
        case LM_Begin:
            gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),"Mode: Layer Begin");
            break;
    }
}

void level_editor_set_title(Window *win)
{
    LayerData *layer_data;
    TextLine label;
    if ((!win)||(!win->data))return;
    layer_data = (LayerData *)win->data;
    gfc_line_sprintf(label,"Layer %i",layer_data->layer->index);
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,0),label);
}

void level_editor_set_actor(Window *win)
{
    LayerData *layer_data;
    TextLine label;
    if ((!win)||(!win->data))return;
    layer_data = (LayerData *)win->data;
    if (layer_data->layer->actor.al != NULL)
    {
        gfc_line_sprintf(label,"Actor:%s",layer_data->layer->actor.al->filename);
    }
    else gfc_line_sprintf(label,"Actor:<none>");
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,2),label);
}


void level_editor_set_player_scale_label(Window *win)
{
    LayerData *layer_data;
    TextLine label;
    if ((!win)||(!win->data))return;
    layer_data = (LayerData *)win->data;
    gfc_line_sprintf(label,"Scale (%.2f,%.2f)",layer_data->layer->playerScale.x,layer_data->layer->playerScale.y);
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,3),label);
}

void onScaleSet(void *data)
{
    Window *win;
    LayerData *layer_data = NULL;
    if (!data)return;
    win = (Window *)data;
    if (!win->data)return;
    layer_data = (LayerData *)win->data;
    layer_data->subwindow = NULL;
    sscanf(layer_data->scaleStr,"%f,%f",&layer_data->layer->playerScale.x,&layer_data->layer->playerScale.y);
    level_editor_set_player_scale_label(win);
}

void onScaleCancel(void *data)
{
    Window *win;
    LayerData *layer_data = NULL;
    if (!data)return;
    win = (Window *)data;
    if (!win->data)return;
    layer_data = (LayerData *)win->data;
    layer_data->subwindow = NULL;
}

int layer_editor_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    LayerData* layer_data;
    Vector2D mouse,offset;
    
    if (!win)return 0;
    if (!updateList)return 0;
    layer_data = (LayerData*)win->data;
    if (!layer_data)return 0;

    mouse = gf2d_mouse_get_position();
    count = gfc_list_get_count(updateList);
    offset = camera_get_position();
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 1000:
                editor_deselect_layer(layer_data->parent);
                return 0;
            case 51:
                // cycle action mode
                layer_data->mode = (layer_data->mode +1) % LM_MAX;
                layer_editor_set_mode(win,layer_data->mode);
                return 0;
            case 52:
                if (!layer_data->subwindow)
                {
                    layer_data->subwindow = actor_editor_menu(layer_data->actorName,layer_data->actionName,win, onBackgroundActorChange,onBackgroundCancel);
                }
                break;
            case 53:
                if (!layer_data->subwindow)
                {
                    gfc_line_cpy(layer_data->scaleStr,"1,1");
                    layer_data->subwindow = window_text_entry("Enter Player Scale", layer_data->scaleStr, win, GFCLINELEN, onScaleSet,onScaleCancel);
                }
                break;
        }
    }
    if (layer_data->subwindow)return 0;
    if (mouse.x < 8)
    {
        win->dimensions.x = 0;
    }
    else if (!gf2d_window_mouse_in(win))
    {
        win->dimensions.x = -win->dimensions.w;
    }

    if (gf2d_window_mouse_in(win))
    {
        return 0;
    }
    if (gf2d_mouse_button_pressed(0))
    {
        // begin selection
        layer_data->click = gf2d_mouse_get_position();
        switch(layer_data->mode)
        {
            case LM_MAX:
            case LM_Default:
                break;
            case LM_Begin:
                break;
            case LM_Placement:
                break;
        }
        
    }
    if (gf2d_mouse_button_held(0))
    {
        layer_data->delta = gf2d_mouse_get_position();
        vector2d_sub(layer_data->delta,layer_data->delta,layer_data->click);
        switch(layer_data->mode)
        {
            case LM_MAX:
            case LM_Default:
                break;
            case LM_Begin:
                layer_data->layer->layerBegin = mouse.y + offset.y;
                break;
            case LM_Placement:
                break;
        }
    }
    if (gf2d_mouse_button_released(0))
    {
        switch(layer_data->mode)
        {
            case LM_MAX:
            case LM_Default:
                break;
            case LM_Begin:
                break;
            case LM_Placement:
                break;
        }
        
        layer_data->delta.x = layer_data->delta.y = 0;
    }


    return 0;
}

int layer_editor_free(Window *win)
{
    LayerData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (LayerData*)win->data;
    free(data);
    return 0;
}

int layer_editor_draw(Window *win)
{
    LayerData *layer_data = NULL;
    if (!win)return 0;
    if (!win->data)return 0;
    layer_data = (LayerData*)win->data;
    switch(layer_data->mode)
    {
        case LM_MAX:
        case LM_Default:
            break;
        case LM_Begin:
            break;
        case LM_Placement:
            break;
    }
    return 0;
}

Window *layer_editor(Layer *layer,Window *parent,Scene *scene)
{
    Window *win;
    LayerData *layer_data = NULL;
    win = gf2d_window_load("config/layer_editor.json");
    if (!win)
    {
        slog("failed to load layer editor menu");
        return NULL;
    }
    layer_data = gfc_allocate_array(sizeof(LayerData),1);
    layer_data->layer = layer;
    layer_data->parent = parent;
    layer_data->scene = scene;
    win->update = layer_editor_update;
    win->free_data = layer_editor_free;
    win->draw = layer_editor_draw;
    win->data = layer_data;
    level_editor_set_title(win);
    layer_editor_set_mode(win,layer_data->mode);
    level_editor_set_player_scale_label(win);
    level_editor_set_actor(win);
    return win;
}

/*eol@eof*/
