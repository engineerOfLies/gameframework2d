#include "simple_logger.h"

#include "gf2d_elements.h"
#include "gf2d_element_label.h"
#include "gf2d_element_entry.h"
#include "gf2d_mouse.h"

#include "camera.h"
#include "windows_common.h"
#include "actor_editor.h"
#include "walkmask_editor.h"

typedef enum
{
    WM_Default,
    WM_Point,
    WM_Edge
}WalkmaskModes;

typedef struct
{
    Walkmask        *mask;
    WalkmaskModes    mode;
    Vector2D         click;
}WalkmaskData;

int walkmask_editor_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    WalkmaskData* walkmask_data;
    if (!win)return 0;
    if (!updateList)return 0;
    walkmask_data = (WalkmaskData*)win->data;
    if (!walkmask_data)return 0;

    if (gf2d_mouse_button_pressed(0))
    {
    }
    if (gf2d_mouse_button_released(0))
    {
        switch(walkmask_data->mode)
        {
            case WM_Default:
                break;
            case WM_Edge:
                break;
            case WM_Point:
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
                break;
            case 55:
                walkmask_data->mode = WM_Point;
                break;
            case 56:
                walkmask_data->mode = WM_Edge;
                break;
        }
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
    Vector2D mouse;
    if (!win)return 0;
    if (!win->data)return 0;
    walkmask_data = (WalkmaskData*)win->data;
    switch(walkmask_data->mode)
    {
        case WM_Default:
            break;
        case WM_Edge:
            if (gf2d_mouse_button_held(0))
            {
                gf2d_shape_draw(gf2d_shape_circle(0,0, 5),gfc_color(1,1,0,1),gf2d_mouse_get_position());
            }
            break;
        case WM_Point:
            if (gf2d_mouse_button_held(0))
            {
                mouse = gf2d_mouse_get_position();
                gf2d_shape_draw(
                    gf2d_shape_rect(
                        MIN(mouse.x,walkmask_data->click.x),
                        MIN(mouse.y,walkmask_data->click.y),
                        fabs(mouse.x - walkmask_data->click.x),
                        fabs(mouse.y - walkmask_data->click.y)),
                    gfc_color(1,1,0,1),
                    vector2d(0,0));
            }
            break;
    }
    return 0;
}

Window *walkmask_editor(Walkmask *mask,Vector2D position)
{
    Window *win;
    WalkmaskData *walkmask_data = NULL;
    win = gf2d_window_load("config/walkmask_editor.json");
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
    return win;
}


/*eol@eof*/


/*eol@eof*/
