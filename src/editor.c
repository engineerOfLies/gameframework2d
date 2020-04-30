#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gf2d_graphics.h"
#include "gf2d_windows.h"
#include "gf2d_elements.h"
#include "gf2d_element_label.h"
#include "gf2d_draw.h"
#include "gf2d_shape.h"
#include "gf2d_mouse.h"

#include "camera.h"
#include "actor_editor.h"
#include "windows_common.h"
#include "exhibits.h"
#include "exhibit_editor.h"
#include "walkmask_editor.h"
#include "layer_editor.h"
#include "scene.h"
#include "layers.h"

extern void exitGame();
extern void exitCheck();

typedef enum
{
    EM_Exhibit,
    EM_Mask,
    EM_Layers,
    EM_MAX
}EditorModes;

typedef struct
{
    TextLine    filename;
    Scene      *scene;
    Exhibit    *selectedExhibit;
    Walkmask   *selectedWalkmask;
    Layer      *selectedLayer;
    TextLine    backgroundFileName;
    TextLine    backgroundActionName;
    Window     *subWindow;
    EditorModes editorMode;
}EditorData;


void onFileSaveCancel(void *Data)
{
    EditorData* data;
    if (!Data)return;
    data = Data;
    gfc_line_cpy(data->filename,data->scene->filename);
    return;
}

void onFileSaveOk(void *Data)
{
    EditorData* data;
    if (!Data)return;
    data = Data;
    gfc_line_cpy(data->scene->filename,data->filename);
    
    scene_save(data->scene, data->filename);
    
    return;
}

void editor_window_close_subwindow(Window *win)
{
    EditorData *data;
    if (!win)return;
    if (!win->data)return;
    data = (EditorData*)win->data;
    gf2d_window_free(data->subWindow);
    data->subWindow = NULL;
}

void editor_window_new_subwindow(Window *win,Window *sub)
{
    EditorData *data;
    if (!win)return;
    data = (EditorData*)win->data;
    editor_window_close_subwindow(win);
    if (!sub)return;
    data->subWindow = sub;
}

int editor_window_draw(Window *win)
{
    int i,c;
    Layer *layer;
    Vector2D res,offset;
    EditorData *data;
    if (!win->data)return 0;
    data = win->data;
    scene_draw(data->scene);
    
    res = gf2d_graphics_get_resolution();
    c = gfc_list_get_count(data->scene->layers);
    for (i = 0;i < c; i++)
    {
        gf2d_entity_draw_all_by_layer(i);
        layer = gfc_list_get_nth(data->scene->layers,i);
        if (!layer)continue;
        if (data->editorMode == EM_Layers)
        {
            if ((data->selectedLayer == NULL) || (data->selectedLayer == layer))
            {
                offset = camera_get_offset();
                gf2d_draw_rect_filled(gfc_sdl_rect(0,layer->layerBegin + offset.y,res.x,res.y-layer->layerBegin - offset.y),vector4d(255,255,0,128));
            }
        }
    }
    return 0;
}

int editor_window_free(Window *win)
{
    EditorData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    free(data);
    return 0;
}

void editor_deselect_mask(Window *win)
{
    EditorData *data;
    if (!win)return;
    data = (EditorData *)win->data;
    if ((!data)||(!data->selectedWalkmask))return;
    data->selectedWalkmask = NULL;
    editor_window_close_subwindow(win);
}

void editor_deselect_layer(Window *win)
{
    EditorData *data;
    if (!win)return;
    data = (EditorData *)win->data;
    if ((!data)||(!data->selectedLayer))return;
    data->selectedLayer = NULL;
    editor_window_close_subwindow(win);
}

void editor_deselect_exhibit(Window *win)
{
    EditorData *data;
    if (!win)return;
    data = (EditorData *)win->data;
    if ((!data)||(!data->selectedExhibit)||(!data->selectedExhibit->entity))return;
    data->selectedExhibit->entity->drawColor = gfc_color(0,0.5,0.5,1);
    data->selectedExhibit = NULL;
    editor_window_close_subwindow(win);
}

void editor_select_mask(Window *win, Walkmask *mask)
{
    EditorData *data;
    if ((!win)||(!mask))return;
    editor_deselect_mask(win);
    data = (EditorData *)win->data;
    data->selectedWalkmask = mask;
    editor_window_new_subwindow(win,walkmask_editor(mask,vector2d(0,80)));
}

void editor_select_layer(Window *win, Layer *layer)
{
    EditorData *data;
    if ((!win)||(!layer))return;
    editor_deselect_mask(win);
    data = (EditorData *)win->data;
    data->selectedLayer = layer;
    editor_window_new_subwindow(win,layer_editor(layer,win,data->scene));
}


void editor_select_exhibit(Window *win, Exhibit *exhibit)
{
    EditorData *data;
    if ((!win)||(!exhibit))return;
    editor_deselect_exhibit(win);
    data = (EditorData *)win->data;
    data->selectedExhibit = exhibit;
    exhibit->entity->drawColor = gfc_color(0,1,1,1);
    editor_window_new_subwindow(win,exhibit_editor(exhibit,vector2d(0,0)));
}

void editor_window_set_mode(Window *win, EditorModes mode)
{
    EditorData *data;
    TextLine label;
    if (!win)return;
    data = (EditorData *)win->data;
    if (!data)return;
    data->editorMode = mode;
    switch (data->editorMode)
    {
        case EM_MAX:
        case EM_Exhibit:
            gfc_line_sprintf(label, "Mode: Exhibit");
            break;
        case EM_Mask:
            gfc_line_sprintf(label, "Mode: Mask");
            break;
        case EM_Layers:
            gfc_line_sprintf(label, "Mode: Layers");
            break;
    }
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1001),label);
}

int editor_window_update(Window *win,List *updateList)
{
    int i,count;
    Walkmask *mask = NULL;
    Layer *layer = NULL;
    Element *e;
    Exhibit *exhibit = NULL;
    EditorData *data;
    Vector2D mouse;
    if (!win)return 0;
    data = (EditorData *)win->data;
    mouse = gf2d_mouse_get_position();
    if (mouse.y < 8)
    {
        win->dimensions.y = 0;
    }
    else if (!gf2d_window_mouse_in(win))
    {
        win->dimensions.y = -win->dimensions.h;
    }
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 1000:
                if (data->subWindow)break;// don't allow mode switch while a subWindow is active
                data->editorMode = (data->editorMode + 1) % EM_MAX;
                editor_window_set_mode(win, data->editorMode);
                break;
            case 51:
                //new layer
                editor_window_close_subwindow(win);
                layer = layer_new();
                scene_add_layer(data->scene,layer);
                data->selectedLayer = layer;
                editor_window_set_mode(win, EM_Layers);
                editor_select_layer(win, layer);
                break;
            case 52:
                // new mask
                editor_window_close_subwindow(win);
                mask = walkmask_new_by_rect(gf2d_rect(100,100,100,100));
                scene_add_walkmask(data->scene,mask);
                data->selectedWalkmask = mask;
                editor_window_set_mode(win, EM_Mask);
                editor_select_mask(win, mask);
                break;
            case 53:
                // new exhibit
                editor_window_close_subwindow(win);
                exhibit = exhibit_new();
                exhibit_set_rect(exhibit,gf2d_rect(100,100,100,100));
                scene_add_exhibit(data->scene,exhibit);
                scene_add_entity(data->scene, exhibit_entity_spawn(exhibit));
                editor_window_set_mode(win, EM_Exhibit);
                editor_select_exhibit(win, exhibit);
                break;
            case 54:
                // save
                window_text_entry("Enter Scene to Save", data->filename, win->data, GFCLINELEN, onFileSaveOk,onFileSaveCancel);
                break;
            case 56:
                //exit
                exitCheck();
                return 1;
        }
    }
    if (gf2d_mouse_button_held(1))
    {
        camera_set_focus(mouse);
    }
    else if (gf2d_mouse_button_released(2))
    {
        switch (data->editorMode)
        {
            case EM_Exhibit:
                exhibit = exhibit_get_mouse_over_from_scene(data->scene);
                if (exhibit != NULL)
                {
                    editor_select_exhibit(win, exhibit);
                }
                else
                {
                    editor_deselect_exhibit(win);
                }
                break;
            case EM_Mask:
                mask = scene_get_walkmask_by_point(data->scene, mouse);
                if (mask != NULL)
                {
                    editor_select_mask(win, mask);
                }
                else
                {
                    editor_deselect_mask(win);
                }
                break;
            case EM_Layers:
                layer = scene_get_layer_by_position(data->scene, mouse);
                if (layer != NULL)
                {
                    editor_select_layer(win, layer);
                }
                else
                {
                    editor_deselect_layer(win);
                }
                break;
            default:
                break;
        }
    }
    return 0;
}

Window *editor_window(Scene * scene)
{
    Window *win;
    EditorData *data;
    win = gf2d_window_load("menus/editor_window.json");
    if (!win)
    {
        slog("failed to load editor window");
        return NULL;
    }
    win->update = editor_window_update;
    win->free_data = editor_window_free;
    win->draw = editor_window_draw;
    data = (EditorData*)gfc_allocate_array(sizeof(EditorData),1);
    gfc_line_cpy(data->filename,scene->filename);
    win->data = data;
    data->scene = scene;

    return win;
}

/*
 * 
 * Intro menu for the editor
 * 
 */

typedef struct
{
    TextLine filename;
    Window *editorMenu;
}EditorMenuData;

int editor_menu_free(Window *win)
{
    EditorMenuData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    free(data);

    return 0;
}

void onFileNameCancel(void *Data)
{
    EditorMenuData* data;
    if (!Data)return;
    data = Data;
    gfc_line_cpy(data->filename,"scenes/");
}

void onFileNameOk(void *Data)
{
    EditorMenuData* data;
    Scene *scene;
    if (!Data)return;
    data = Data;
    
    scene = scene_load(data->filename);
    if (!scene)
    {
        window_alert("File not found", data->filename,NULL,NULL);
        onFileNameCancel(Data);
        return;
    }
    editor_window(scene);
    scene_spawn_exhibits(scene);

    gf2d_window_free(data->editorMenu);
    return;
}

int editor_menu_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    EditorMenuData* data;
    if (!win)return 0;
    if (!updateList)return 0;
    data = (EditorMenuData*)win->data;
    if (!data)return 0;
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 51:
                window_text_entry("Enter Scene to Load", data->filename, win->data, GFCLINELEN, onFileNameOk,onFileNameCancel);
                return 1;
            case 52:
                editor_window(scene_new());
                gf2d_window_free(win);
                return 1;
            case 53:
                exitGame();
                gf2d_window_free(win);
                return 1;
        }
    }
    return 0;
}


Window *editor_menu()
{
    Window *win;
    EditorMenuData* data;
    win = gf2d_window_load("menus/editor_menu.json");
    if (!win)
    {
        slog("failed to load editor menu");
        return NULL;
    }
    win->update = editor_menu_update;
    win->free_data = editor_menu_free;
    data = (EditorMenuData*)gfc_allocate_array(sizeof(EditorMenuData),1);
    data->editorMenu = win;
    gfc_line_cpy(data->filename,"scenes/");
    win->data = data;
    return win;
}


/*eol@eof*/
