#include <simple_logger.h>

#include "gf2d_mouse.h"
#include "gf2d_draw.h"
#include "gfc_input.h"

#include "camera.h"
#include "empire_hud.h"
#include "galaxy_view.h"
#include "system_view.h"
#include "planet_view.h"

typedef struct
{
    Vector2D pressPosition;
    Vector2D cameraPosition;
    Planet *planet;
    Window *childWindow;
}PlanetWindowData;


int planet_view_draw(Window *win)
{
    Planet *planet;
    PlanetWindowData *data;
    Vector2D drawOffset;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (PlanetWindowData*)win->data;
    drawOffset = camera_get_offset();
    planet_draw_planet_view(data->planet,drawOffset);
    
    return 1;
}

int planet_view_free(Window *win)
{
    PlanetWindowData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (PlanetWindowData*)win->data;
    free(data);
    return 0;
}

int planet_view_update(Window *win,List *updateList)
{
    PlanetWindowData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (PlanetWindowData*)win->data;

    camera_mouse_pan();
    if (!gf2d_window_mouse_in(win))
    {
        return 0;//if outside the window rect, its over something else
    }
    
    camera_update_by_keys();
    if (win->parent)
    {
        if (gfc_input_command_released("cancel"))
        {
            system_view_close_child_window(win->parent);
            gf2d_window_free(win);
            return 1;
        }
    }
    return 0;
}

Window *planet_view_window(Planet *planet,Window *parent)
{
    Window *win;
    PlanetWindowData *data;
    win = gf2d_window_load("menus/planet_view.json");
    if (!win)
    {
        slog("failed to load planet view window");
        return NULL;
    }
    win->no_draw_generic = 1;
    win->draw = planet_view_draw;
    win->update = planet_view_update;
    win->free_data = planet_view_free;
    data = gfc_allocate_array(sizeof(PlanetWindowData),1);
    data->planet = planet;
    win->data = data;
    win->parent = parent;
    camera_set_position(vector2d(-128,-128));
    empire_hud_bubble();
    return win;

}


/*file's end*/
