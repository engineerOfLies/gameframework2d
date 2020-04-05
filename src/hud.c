#include "simple_logger.h"

#include "gf2d_elements.h"
#include "gf2d_element_label.h"
#include "gf2d_element_entry.h"
#include "gf2d_mouse.h"
#include "gf2d_draw.h"

#include "camera.h"
#include "windows_common.h"
#include "hud.h"

static Window *_HUD = NULL; /**<hud is a singleton.  don't spawn more than one*/

typedef struct
{
    Entity *player;// the player whose in control at the moment
}HUD;

int hud_mouse_check()
{
    if (!_HUD)return 0;
    return gf2d_window_mouse_in(_HUD);
}

Window *hud_get()
{
    return _HUD;
}

int hud_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    HUD* hud;
    Vector2D mouse;
    
    if (!win)return 0;
    if (!updateList)return 0;
    hud = (HUD*)win->data;
    if (!hud)return 0;

    mouse = gf2d_mouse_get_position();
    
    
    mouse = gf2d_mouse_get_position();
    if (mouse.y < 8)
    {
        win->dimensions.y = 0;
    }
    else if (!gf2d_window_mouse_in(win))
    {
        win->dimensions.y = -win->dimensions.h;
    }

    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 0:
                break;
        }
    }
    return 0;
}

int hud_draw(Window *win)
{
    return 0;
}

int hud_free(Window *win)
{
    HUD *hud;
    if ((!win)||(!win->data))return 0;
    hud = (HUD *)win->data;
    free(hud);
    return 0;
}

Window *hud_open(Entity *player)
{
    Window *win;
    HUD *hud = NULL;
    if (_HUD)
    {
        hud = (HUD*)_HUD->data;
        if (hud)
        {
            hud->player = player;
        }
        return _HUD;
    }
    win = gf2d_window_load("config/hud.json");
    if (!win)
    {
        slog("failed to load hud");
        return NULL;
    }
    hud = gfc_allocate_array(sizeof(HUD),1);
    hud->player = player;
    win->update = hud_update;
    win->free_data = hud_free;
    win->draw = hud_draw;
    win->data = hud;
    _HUD = win;
    return win;
}
