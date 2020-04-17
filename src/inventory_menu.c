#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_callbacks.h"
#include "gf2d_graphics.h"
#include "gf2d_windows.h"
#include "gf2d_elements.h"
#include "gf2d_element_button.h"
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
    int selectedItem;
    int lastSelectedItem;
    Inventory *inven;
    TextLine filename;
    Callback *callback;
    char *actor,*action;
}InventoryMenuData;

int inventory_menu_free(Window *win)
{
    InventoryMenuData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    gfc_callback_free(data->callback);
    free(data);
    return 0;
}

void inventory_menu_select_item(Window *win, InventoryMenuData* data)
{
    TextLine buffer;
    InventoryItem *item = NULL;
    Item *itemInfo = NULL;
    Element *e;
    item = inventory_get_nth(data->inven,data->selectedItem);
    if (!item)
    {
        slog("failed to find inventory item indexed %i",data->selectedItem);
        return;
    }
    
    itemInfo = item_list_get_by_name(item->name);
    if (!itemInfo)
    {
        slog("failed to find item");
        return;
    }
    
    data->lastSelectedItem = data->selectedItem;
    
    e = gf2d_window_get_element_by_id(win,11),
    
    gf2d_element_actor_set_actor(e, itemInfo->actor);
    gf2d_element_actor_set_action(e, itemInfo->action);
    
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,10),itemInfo->name);
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,14),itemInfo->description);
    e = gf2d_window_get_element_by_id(win,12);
    if (item->count > 1)
    {
        gfc_line_sprintf(buffer,"Count: %i",item->count);
        gf2d_element_label_set_text(e,buffer);
        e->state = ES_idle;
    }
    else
    {
        e->state = ES_hidden;
    }

    e = gf2d_window_get_element_by_id(win,13);
    if (item->skill > 0)
    {
        gfc_line_sprintf(buffer,"Skill Level: %i",item->skill);
        gf2d_element_label_set_text(e,buffer);
        e->state = ES_idle;
    }
    else
    {
        e->state = ES_hidden;
    }
}

int inventory_menu_draw(Window *win)
{
    InventoryMenuData* data;
    
    if (!win)return 0;
    data = (InventoryMenuData*)win->data;
    if (!data)return 0;

    if (data->selectedItem != data->lastSelectedItem)
    {
        data->lastSelectedItem = data->selectedItem;
        inventory_menu_select_item(win, data);
    }
    return 0;
}

int inventory_menu_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    Item * itemInfo;
    InventoryItem *item;
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
                item = inventory_get_nth(data->inven,data->selectedItem);
                if (!item)
                {
                    slog("no inventory item with index %i",data->selectedItem);
                    return 1;
                }
                
                itemInfo = item_list_get_by_name(item->name);
                if (!itemInfo)
                {
                    slog("no item named %s",item->name);
                    return 1;
                }
                
                
                gfc_line_cpy(data->action,itemInfo->action);
                gfc_line_cpy(data->actor,itemInfo->actor);

                if (data->callback)
                {
                    gfc_callback_call(data->callback);
                }
                else slog("no callback set");
                gf2d_window_free(win);
                return 1;
        }
        if (e->index > 1000)
        {
            data->selectedItem = e->index - 1001;
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

void inventory_menu_list_setup(Window *win, InventoryMenuData* data)
{
    int i,c;
    Element *e,*b;
    ActorElement *ae;
    InventoryItem *item;
    if ((!win)||(!data))return;
    c = inventory_get_count(data->inven);
    for (i = 0; i < c; i++)
    {
        item = inventory_get_nth(data->inven,i);
        if (!item)continue;
        ae = inventory_menu_item_set(item);
        if (!ae)continue;
        b = gf2d_element_new_full(
            gf2d_window_get_element_by_id(win,1000),
            1001+i,
            item->name,
            gf2d_rect(0,0,64,64),
            gfc_color(1,1,1,1),
            0,
            gfc_color(.5,.5,.5,1),1);
        e = gf2d_element_new_full(
            b,
            2000+i,
            item->name,
            gf2d_rect(0,0,70,70),
            gfc_color(1,1,1,1),
            0,
            gfc_color(1,1,1,1),0);
        gf2d_element_make_actor(e,ae);
        gf2d_element_make_button(b,gf2d_element_button_new_full(NULL,e,gfc_color(1,1,1,1),gfc_color(0.9,0.9,0.9,1),1));
        gf2d_element_list_add_item(gf2d_window_get_element_by_id(win,1000),b);
    }
}

Window *inventory_menu(Inventory *inven,char *actor,char *action,void (*onSelect)(void *data),void *callbackData)
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
    data->action = action;
    data->actor = actor;
    data->callback = gfc_callback_new(onSelect,callbackData);
    inventory_menu_list_setup(win,data);
    inventory_menu_select_item(win, data);
    return win;
}


/*eol@eof*/
