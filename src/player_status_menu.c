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
#include "player_status_menu.h"

typedef struct
{
    Entity *player;
}PlayerStatsData;

int player_status_menu_free(Window *win)
{
    PlayerStatsData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    free(data);
    return 0;
}


int player_status_menu_draw(Window *win)
{
    return 0;
}

int player_status_menu_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    PlayerStatsData* data;
    if (!win)return 0;
    if (!updateList)return 0;
    data = (PlayerStatsData*)win->data;
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
        }
    }
    return 1;
}

Window *player_status_menu(Entity *player)
{
    Window *win;
    PlayerStatsData* data;
    win = gf2d_window_load("menus/player_status_menu.json");
    if (!win)
    {
        slog("failed to load player status menu menu");
        return NULL;
    }
    win->update = player_status_menu_update;
    win->free_data = player_status_menu_free;
    win->draw = player_status_menu_draw;
    data = (PlayerStatsData*)gfc_allocate_array(sizeof(PlayerStatsData),1);
    win->data = data;
    data->player = player;
    return win;
}


/*eol@eof*/
