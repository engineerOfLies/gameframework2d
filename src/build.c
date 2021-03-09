#include <simple_logger.h>
#include "gfc_input.h"

#include "gfc_list.h"
#include "gfc_callbacks.h"

#include "gf2d_elements.h"
#include "gf2d_element_label.h"
#include "gf2d_element_entry.h"

#include "build.h"
#include "level.h"


int build_free(Window *win)
{

    if (!win)return 0;
    if (!win->data)return 0;

    return 0;
}

int build_update(Window *win,List *updateList)
{
    int i,count;
    Vector2D mouseTile;
    Uint32 mouseButtons;
    Element *e;
    Level *level;
    if (!win)return 0;
    if (!updateList)return 0;
    
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
        }
    }
    return 0;
}

Window *window_build()
{
    Window *win;
    win = gf2d_window_load("config/build_window.json");
    if (!win)
    {
        slog("failed to load build window");
        return NULL;
    }
    win->update = build_update;
    win->free_data = build_free;
    return win;
}


/*eol@eof*/
