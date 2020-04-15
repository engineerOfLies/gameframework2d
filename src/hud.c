#include "simple_logger.h"

#include "gf2d_elements.h"
#include "gf2d_element_label.h"
#include "gf2d_element_entry.h"
#include "gf2d_mouse.h"
#include "gf2d_draw.h"

#include "camera.h"
#include "hud.h"
#include "windows_common.h"
#include "options_menu.h"
#include "inventory_menu.h"
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

void hud_show()
{
    if (!_HUD)return;
    _HUD->dimensions.y = 0;
}

void hud_hide()
{
    if (!_HUD)return;
    _HUD->dimensions.y = -_HUD->dimensions.h;
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
        gf2d_mouse_set_function(MF_Pointer);
        hud_show();
    }
    else if (!gf2d_window_mouse_in(win))
    {
        gf2d_mouse_set_function(scene_get_mouse_function(scene_get_active()));
        hud_hide();
    }

    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 11:
                scene_set_mouse_function(scene_get_active(),MF_Interact);
                gf2d_mouse_set_function(MF_Interact);
                hud_hide();
                return 1;
            case 21:
                scene_set_mouse_function(scene_get_active(),MF_Walk);
                gf2d_mouse_set_function(MF_Walk);
                hud_hide();
                return 1;
            case 31:
                scene_set_mouse_function(scene_get_active(),MF_Look);
                gf2d_mouse_set_function(MF_Look);
                hud_hide();
                return 1;
            case 41:
                scene_set_mouse_function(scene_get_active(),MF_Talk);
                gf2d_mouse_set_function(MF_Talk);
                hud_hide();
                return 1;
            case 51:
                hud_hide();
                inventory_menu(player_get_item_inventory(hud->player));
                return 1;
            case 81:
                options_menu(hud->player);
                hud_hide();
                return 1;

        }
    }
    if (gf2d_window_mouse_in(win))
    {
        return 1;
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
