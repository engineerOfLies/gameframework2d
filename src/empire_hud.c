#include <simple_logger.h>

#include "gf2d_mouse.h"
#include "gf2d_draw.h"

#include "camera.h"

#include "galaxy.h"
#include "empire.h"

#include "galaxy_view.h"
#include "system_view.h"
#include "empire_hud.h"


typedef struct
{
    Galaxy *galaxy;
    Empire *empire;
    Window *view;
}EmpireHudData;


int empire_hud_draw(Window *win)
{
    EmpireHudData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (EmpireHudData*)win->data;

    return 0;
}

int empire_hud_free(Window *win)
{
    EmpireHudData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (EmpireHudData*)win->data;
    free(data);
    return 0;
}

int empire_hud_update(Window *win,List *updateList)
{
    EmpireHudData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (EmpireHudData*)win->data;
    return 0;
}

Window *empire_hud_window(Empire *empire,Galaxy *galaxy)
{
    Window *win;
    EmpireHudData *data;
    win = gf2d_window_load("menus/empire_hud.json");
    if (!win)
    {
        slog("failed to load empire hud window");
        return NULL;
    }
    win->draw = empire_hud_draw;
    win->update = empire_hud_update;
    win->free_data = empire_hud_free;
    data = gfc_allocate_array(sizeof(EmpireHudData),1);
    data->galaxy = galaxy;
    data->empire = empire;
    data->view = galaxy_view_window(galaxy); 
    win->data = data;
    gf2d_window_bring_to_front(win);
    return win;

}


/*file's end*/
