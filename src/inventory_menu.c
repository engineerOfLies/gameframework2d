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
#include "windows_common.h"
#include "player.h"
#include "inventory_menu.h"

typedef struct
{
    TextLine filename;
}InventoryMenuData;

int inventory_menu_free(Window *win)
{
    InventoryMenuData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    free(data);

    return 0;
}

int inventory_menu_draw(Window *win)
{
    InventoryMenuData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    return 0;
}

int inventory_menu_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    InventoryMenuData* data;
    if (!win)return 0;
    if (!updateList)return 0;
    data = (InventoryMenuData*)win->data;
    if (!data)return 0;
    
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 51:
                gf2d_window_free(win);
                return 1;
            case 52:
                gf2d_window_free(win);
                return 1;
        }
    }
    return 1;
}


Window *inventory_menu()
{
    Window *win;
    InventoryMenuData* data;
    win = gf2d_window_load("config/inventory_menu.json");
    if (!win)
    {
        slog("failed to load inventory menu");
        return NULL;
    }
    win->update = inventory_menu_update;
    win->free_data = inventory_menu_free;
    win->draw = inventory_menu_draw;
    data = (InventoryMenuData*)gfc_allocate_array(sizeof(InventoryMenuData),1);
    win->data = data;
    return win;
}


/*eol@eof*/
