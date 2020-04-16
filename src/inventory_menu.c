#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gf2d_graphics.h"
#include "gf2d_windows.h"
#include "gf2d_elements.h"
#include "gf2d_element_list.h"
#include "gf2d_element_label.h"
#include "gf2d_element_actor.h"
#include "gf2d_draw.h"
#include "gf2d_shape.h"
#include "gf2d_mouse.h"

#include "camera.h"
#include "windows_common.h"
#include "player.h"
#include "items.h"
#include "inventory_menu.h"

typedef struct
{
    Inventory *inven;
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

ActorElement *inventory_menu_item_set(InventoryItem *item)
{
    Item *itemInfo;
    if (!item)return NULL;
    itemInfo = item_list_get_by_name(item->name);
    if (!itemInfo)return NULL;
    return gf2d_element_actor_new_full(itemInfo->actor, itemInfo->action,vector2d(1,1));
}

void inventory_menu_list_setup(Window *win,InventoryMenuData* data)
{
    int i,c;
    Element *e;
    ActorElement *ae;
    InventoryItem *item;
    if ((!win)||(!data))return;
    c = inventory_get_count(data->inven);
    slog("attempting to add %i items to the inventory screen",c);
    for (i = 0; i < c; i++)
    {
        item = inventory_get_nth(data->inven,i);
        if (!item)continue;
        ae = inventory_menu_item_set(item);
        if (!ae)continue;
        slog("building inventory element");
        e = gf2d_element_new_full(
            gf2d_window_get_element_by_id(win,1000),
            1000+i,
            item->name,
            gf2d_rect(0,0,70,70),
            gfc_color(1,1,1,1),
            0);
        gf2d_element_make_actor(e,ae);
        gf2d_element_list_add_item(gf2d_window_get_element_by_id(win,1000),e);
    }
}

Window *inventory_menu(Inventory *inven)
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
    data->inven = inven;
    inventory_menu_list_setup(win,data);
    return win;
}


/*eol@eof*/
