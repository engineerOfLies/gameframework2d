#include "simple_logger.h"

#include "gf2d_elements.h"
#include "gf2d_element_label.h"
#include "gf2d_element_entry.h"
#include "gf2d_mouse.h"
#include "gf2d_draw.h"

#include "camera.h"
#include "windows_common.h"
#include "actor_editor.h"
#include "walkmask_editor.h"

typedef enum
{
    WM_Default,
    WM_Point,
    WM_Edge,
    WM_MAX
}WalkmaskModes;

typedef struct
{
    Walkmask        *mask;
    PointData       *selectedPoint;
    WalkmaskModes    mode;
    Vector2D         click;
    Vector2D         delta;
}WalkmaskData;

void walkmask_editor_set_style(Window *win, Walkmask *mask)
{
    if ((!win)||(!mask))return;
    if (mask->exterior)
    {
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,2),"exterior mask");
    }
    else
    {
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,2),"interior mask");
    }
}

void walkmask_editor_set_mode(Window *win,WalkmaskModes mode)
{
    if (!win)return;
    switch (mode)
    {
        case WM_MAX:
        case WM_Default:
            gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,3),"Mask Select");
            break;
        case WM_Point:
            gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,3),"Point Select");
            break;
        case WM_Edge:
            gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,3),"Edge Select");
            break;
    }
}

int walkmask_editor_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    WalkmaskData* walkmask_data;
    if (!win)return 0;
    if (!updateList)return 0;
    walkmask_data = (WalkmaskData*)win->data;
    if (!walkmask_data)return 0;
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 53:
                if ((walkmask_data)&&(walkmask_data->mask))
                {
                    walkmask_data->mask->exterior = !walkmask_data->mask->exterior;
                    walkmask_editor_set_style(win, walkmask_data->mask);
                }
                return 0;
            case 54:
                // cycle action mode
                walkmask_data->mode = (walkmask_data->mode +1) % WM_MAX;
                walkmask_editor_set_mode(win,walkmask_data->mode);
                return 0;
            case 56:
                //subdivide
                if (walkmask_data->selectedPoint)
                {
                    walkmask_subdivide_point(walkmask_data->mask,walkmask_data->selectedPoint);
                }
                return 0;
        }
    }
    if (gf2d_window_mouse_in(win))
    {
        return 0;
    }
    if (gf2d_mouse_button_pressed(0))
    {
        // begin selection
        walkmask_data->click = gf2d_mouse_get_position();
        switch(walkmask_data->mode)
        {
            case WM_MAX:
            case WM_Default:
                break;
            case WM_Edge:
                break;
            case WM_Point:
                walkmask_data->selectedPoint = walkmask_get_nearest_point(walkmask_data->mask, walkmask_data->click);
                break;
        }
        
    }
    if (gf2d_mouse_button_held(0))
    {
        walkmask_data->delta = gf2d_mouse_get_position();
        vector2d_sub(walkmask_data->delta,walkmask_data->delta,walkmask_data->click);
    }
    if (gf2d_mouse_button_released(0))
    {
        switch(walkmask_data->mode)
        {
            case WM_MAX:
            case WM_Default:
                walkmask_move(walkmask_data->mask,walkmask_data->delta);
                break;
            case WM_Edge:
                break;
            case WM_Point:
                vector2d_add(walkmask_data->selectedPoint->position,walkmask_data->selectedPoint->position,walkmask_data->delta);
                break;
        }
        walkmask_data->delta.x = walkmask_data->delta.y = 0;
    }


    return 0;
}

int walkmask_editor_free(Window *win)
{
    WalkmaskData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (WalkmaskData*)win->data;
    free(data);
    return 0;
}

int walkmask_editor_draw(Window *win)
{
    WalkmaskData *walkmask_data = NULL;
    Vector2D offset,p1,p2;
    PointData *nextpoint;
    if (!win)return 0;
    if (!win->data)return 0;
    offset = camera_get_offset();
    walkmask_data = (WalkmaskData*)win->data;
    switch(walkmask_data->mode)
    {
        case WM_MAX:
        case WM_Default:
            walkmask_draw(walkmask_data->mask,gfc_color(0.9,0,0.9,1),walkmask_data->delta);
            break;
        case WM_Point:
            walkmask_draw(walkmask_data->mask,gfc_color(0.9,0,0.9,1),vector2d(0,0));
            if (walkmask_data->selectedPoint != NULL)
            {
                vector2d_add(p1,walkmask_data->selectedPoint->position,offset);
                vector2d_add(p1,p1,walkmask_data->delta);
                gf2d_draw_circle(p1, 6, vector4d(255,255,255,255));
            }
            break;
        case WM_Edge:
            walkmask_draw(walkmask_data->mask,gfc_color(0.9,0,0.9,1),vector2d(0,0));
            if (walkmask_data->selectedPoint != NULL)
            {
                nextpoint = gfc_list_get_nth(walkmask_data->mask->points,walkmask_data->selectedPoint->nextPoint);
                if (!nextpoint)
                {
                    slog("no next point for current mask point");
                    return 0;
                }
                vector2d_add(p1,walkmask_data->selectedPoint->position,offset);
                vector2d_add(p1,walkmask_data->delta,p1);
                vector2d_add(p2,nextpoint->position,offset);
                vector2d_add(p2,walkmask_data->delta,p2);
                
                gf2d_draw_line(p1,p2, vector4d(1,1,0,1));
                gf2d_draw_circle(p1, 5, vector4d(1,1,0,1));
                gf2d_draw_circle(p2, 5, vector4d(1,1,0,1));
            }
            break;
    }
    return 0;
}

Window *walkmask_editor(Walkmask *mask,Vector2D position)
{
    Window *win;
    WalkmaskData *walkmask_data = NULL;
    win = gf2d_window_load("config/mask_editor.json");
    if (!win)
    {
        slog("failed to load exhibit editor menu");
        return NULL;
    }
    walkmask_data = gfc_allocate_array(sizeof(WalkmaskData),1);
    walkmask_data->mask = mask;
    gf2d_window_set_position(win,position);
    win->update = walkmask_editor_update;
    win->free_data = walkmask_editor_free;
    win->draw = walkmask_editor_draw;
    win->data = walkmask_data;
    walkmask_editor_set_style(win, mask);
    walkmask_editor_set_mode(win,walkmask_data->mode);
    return win;
}


/*eol@eof*/


/*eol@eof*/
