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
#include "options_menu.h"

extern void exitGame();
extern void exitCheck();


typedef struct
{
    Entity *player;
    TextLine filename;
}OptionsMenuData;

void onPlayerSaveCancel(void *Data)
{
    OptionsMenuData* data;
    if (!Data)return;
    data = Data;
    gfc_line_cpy(data->filename,data->filename);
    return;
}

void onPlayerSaveOk(void *Data)
{
    OptionsMenuData* data;
    if (!Data)return;
    data = Data;
    player_save(data->player, data->filename);
    
    return;
}


int options_menu_free(Window *win)
{
    OptionsMenuData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    free(data);

    return 0;
}

int options_menu_draw(Window *win)
{
    return 0;
}

int options_menu_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    OptionsMenuData* data;
    if (!win)return 0;
    if (!updateList)return 0;
    data = (OptionsMenuData*)win->data;
    if (!data)return 0;
    
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 50:
                gf2d_window_free(win);
                return 1;
            case 51:
                gfc_line_cpy(data->filename,player_get_filename(data->player));
                window_text_entry("Enter filename to save", data->filename, win->data, GFCLINELEN, onPlayerSaveOk,onPlayerSaveCancel);
                return 1;
            case 52:
                return 1;
            case 81:
                gf2d_window_free(win);
                exitGame();
                return 1;
        }
    }
    return 1;
}


Window *options_menu(Entity *player)
{
    Window *win;
    OptionsMenuData* data;
    win = gf2d_window_load("config/options_menu.json");
    if (!win)
    {
        slog("failed to load editor menu");
        return NULL;
    }
    win->update = options_menu_update;
    win->free_data = options_menu_free;
    win->draw = options_menu_draw;
    data = (OptionsMenuData*)gfc_allocate_array(sizeof(OptionsMenuData),1);
    win->data = data;
    data->player = player;
    return win;
}


/*eol@eof*/
