#include <stdio.h>
#include "simple_logger.h"
#include "gf2d_windows.h"
#include "gf2d_elements.h"

extern void exitGame();

int editor_window_free(Window *win)
{
    if (!win)return 0;
    if (!win->data)return 0;

    return 0;
}

int editor_window_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    if (!win)return 0;
    if (!updateList)return 0;
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
    }
    return 0;
}

Window *editor_window()
{
    Window *win;
    win = gf2d_window_load("config/editor_window.json");
    if (!win)
    {
        slog("failed to load editor window");
        return NULL;
    }
    win->update = editor_window_update;
    win->free_data = editor_window_free;
    return win;
}

int editor_menu_free(Window *win)
{
    if (!win)return 0;
    if (!win->data)return 0;

    return 0;
}

int editor_menu_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    if (!win)return 0;
    if (!updateList)return 0;
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 51:
                return 1;
            case 52:
                return 1;
            case 53:
                exitGame();
                return 1;
        }
    }
    return 0;
}


Window *editor_menu()
{
    Window *win;
    win = gf2d_window_load("config/editor_menu.json");
    if (!win)
    {
        slog("failed to load editor menu");
        return NULL;
    }
    win->update = editor_menu_update;
    win->free_data = editor_menu_free;
    return win;
}


/*eol@eof*/
