#include "simple_logger.h"

#include "gf2d_elements.h"
#include "gf2d_element_label.h"
#include "gf2d_element_entry.h"

#include "camera.h"
#include "windows_common.h"
#include "actor_editor.h"
#include "exhibit_editor.h"

typedef enum
{
    EM_Default,
    EM_Rect,
    EM_Near
}ExhibitModes;

typedef struct
{
    Exhibit        *exhibit;
    ExhibitModes    mode;
    Vector2D        click;
    Window         *subwindow;
    TextLine        actionName;
    TextLine        actorName;
    TextLine        layerStr;
}ExhibitData;

void exhibit_editor_set_layer(Window *win)
{
    ExhibitData *exhibit_data;
    TextLine label;
    if ((!win)||(!win->data))return;
    exhibit_data = (ExhibitData *)win->data;
    gfc_line_sprintf(label,"Layer: %i",exhibit_data->exhibit->drawLayer);
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,3),label);
}

void exhibit_editor_set_actor(Window *win)
{
    ExhibitData *exhibit_data;
    if ((!win)||(!win->data))return;
    exhibit_data = (ExhibitData *)win->data;
    if (strlen(exhibit_data->exhibit->actor))
    {
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,4),exhibit_data->exhibit->actor);
    }
}

void onActorChange(void *data)
{
    Window *win;
    ExhibitData *exhibit_data;
    Exhibit *exhibit;
    if (!data)return;
    win = data;
    if (!win)return;
    exhibit_data = win->data;
    if (!exhibit_data)return;
    exhibit = exhibit_data->exhibit;
    gfc_line_cpy(exhibit_data->exhibit->actor,exhibit_data->actorName);
    gfc_line_cpy(exhibit_data->exhibit->action,exhibit_data->actionName);
    if (exhibit->entity)
    {
        gf2d_actor_free(&exhibit->entity->actor);
        if (gf2d_actor_load(&exhibit->entity->actor,exhibit_data->actorName))
        {
            gf2d_actor_set_action(&exhibit->entity->actor,exhibit_data->actionName);
        }
    }
    
    exhibit_editor_set_actor(win);
    exhibit_data->subwindow = NULL;
}

void onActorCancel(void *data)
{
    Window *win;
    ExhibitData *exhibit_data;
    if (!data)return;
    win = data;
    if (!win)return;
    exhibit_data = win->data;
    if (!exhibit_data)return;
    exhibit_data->subwindow = NULL;
}


void onLayerSet(void *data)
{
    Window *win;
    int layer;
    ExhibitData *exhibit_data = NULL;
    if (!data)return;
    win = (Window *)data;
    if (!win->data)return;
    exhibit_data = (ExhibitData *)win->data;
    exhibit_data->subwindow = NULL;
    sscanf(exhibit_data->layerStr,"%i",&layer);
    exhibit_set_draw_layer(exhibit_data->exhibit, layer);
    exhibit_editor_set_layer(win);
}

void onLayerCancel(void *data)
{
    Window *win;
    ExhibitData *exhibit_data = NULL;
    if (!data)return;
    win = (Window *)data;
    if (!win->data)return;
    exhibit_data = (ExhibitData *)win->data;
    exhibit_data->subwindow = NULL;
}


void onExhibitNameCancel(void *data)
{
    Window *win;
    ExhibitData* exhibit_data;
    if (!data)return;
    win = (Window *)data;
    if (!win->data)return;
    exhibit_data = (ExhibitData*)win->data;
    
    gfc_line_cpy(exhibit_data->exhibit->name,gf2d_element_label_get_text(gf2d_window_get_element_by_id(win,1)));
}

void onExhibitNameChange(void *data)
{
    Window *win;
    ExhibitData* exhibit_data;
    if (!data)return;
    win = (Window *)data;
    if (!win->data)return;
    exhibit_data = (ExhibitData*)win->data;
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),exhibit_data->exhibit->name);
}

int exhibit_editor_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    Rect rect;
    ExhibitData* exhibit_data;
    Vector2D mouse;
    if (!win)return 0;
    exhibit_data = (ExhibitData*)win->data;
    if (!exhibit_data)return 0;
    
    mouse = gf2d_mouse_get_position();
    if (mouse.x < 8)
    {
        win->dimensions.x = 0;
    }
    else if (!gf2d_window_mouse_in(win))
    {
        win->dimensions.x = -win->dimensions.w;
    }

    if (gf2d_mouse_button_pressed(0))
    {
        if (exhibit_data->mode == EM_Rect)
        {
            exhibit_data->click = gf2d_mouse_get_position();
        }
    }
    if (gf2d_mouse_button_released(0))
    {
        switch(exhibit_data->mode)
        {
            case EM_Default:
                break;
            case EM_Near:
                exhibit_data->exhibit->near = gf2d_mouse_get_position();
                vector2d_sub(exhibit_data->exhibit->near,exhibit_data->exhibit->near,camera_get_offset());
                exhibit_data->mode = EM_Default;
                break;
            case EM_Rect:
                mouse = gf2d_mouse_get_position();
                rect.w = fabs(mouse.x - exhibit_data->click.x);
                rect.h = fabs(mouse.y - exhibit_data->click.y);
                rect.x = MIN(mouse.x,exhibit_data->click.x);
                rect.y = MIN(mouse.y,exhibit_data->click.y);
                vector2d_sub(rect,rect,camera_get_offset());
                exhibit_set_rect(exhibit_data->exhibit,rect);
                exhibit_data->mode = EM_Default;
                break;
        }
    }


    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 51:
                window_text_entry("Exhibit Name", exhibit_data->exhibit->name, win, GFCLINELEN, onExhibitNameChange,onExhibitNameCancel);
                break;
            case 53:
                if (!exhibit_data->subwindow)
                {
                    gfc_line_sprintf(exhibit_data->layerStr,"%i",exhibit_data->exhibit->drawLayer);
                    exhibit_data->subwindow = window_text_entry("Enter Exhibit Draw Layer", exhibit_data->layerStr, win, GFCLINELEN, onLayerSet,onLayerCancel);
                }
                break;
            case 54:
                if (!exhibit_data->subwindow)
                {
                    exhibit_data->subwindow = actor_editor_menu(exhibit_data->actorName,exhibit_data->actionName,win, onActorChange,onActorCancel);
                }
                break;
            case 55:
                exhibit_data->mode = EM_Rect;
                break;
            case 56:
                exhibit_data->mode = EM_Near;
                break;
        }
    }

    return 0;
}

int exhibit_editor_free(Window *win)
{
    ExhibitData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (ExhibitData*)win->data;
    free(data);
    return 0;
}

int exhibit_editor_draw(Window *win)
{
    ExhibitData *exhibit_data = NULL;
    Vector2D mouse;
    if (!win)return 0;
    if (!win->data)return 0;
    exhibit_data = (ExhibitData*)win->data;
    switch(exhibit_data->mode)
    {
        case EM_Default:
            break;
        case EM_Near:
            if (gf2d_mouse_button_held(0))
            {
                gf2d_shape_draw(gf2d_shape_circle(0,0, 5),gfc_color(1,1,0,1),gf2d_mouse_get_position());
            }
            break;
        case EM_Rect:
            if (gf2d_mouse_button_held(0))
            {
                mouse = gf2d_mouse_get_position();
                gf2d_shape_draw(
                    gf2d_shape_rect(
                        MIN(mouse.x,exhibit_data->click.x),
                        MIN(mouse.y,exhibit_data->click.y),
                        fabs(mouse.x - exhibit_data->click.x),
                        fabs(mouse.y - exhibit_data->click.y)),
                    gfc_color(1,1,0,1),
                    vector2d(0,0));
            }
            break;
    }
    return 0;
}

Window *exhibit_editor(Exhibit *exhibit,Vector2D position)
{
    Window *win;
    ExhibitData *exhibit_data = NULL;
    win = gf2d_window_load("config/exhibit_editor.json");
    if (!win)
    {
        slog("failed to load exhibit editor menu");
        return NULL;
    }
    exhibit_data = gfc_allocate_array(sizeof(ExhibitData),1);
    exhibit_data->exhibit = exhibit;
    gf2d_window_set_position(win,position);
    win->update = exhibit_editor_update;
    win->free_data = exhibit_editor_free;
    win->draw = exhibit_editor_draw;
    win->data = exhibit_data;
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),exhibit->name);
    exhibit_editor_set_layer(win);
    return win;
}


/*eol@eof*/
