#include <simple_logger.h>

#include "gf2d_mouse.h"
#include "gf2d_draw.h"

#include "galaxy_view.h"

typedef struct
{
    Galaxy *galaxy;
}GalaxyWindowData;


int galaxy_view_draw(Window *win)
{
    System *system;
    Vector2D mouseposition;
    GalaxyWindowData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (GalaxyWindowData*)win->data;
    galaxy_draw(data->galaxy);
    mouseposition = galaxy_position_from_screen_position(gf2d_mouse_get_position());
    system = galaxy_get_nearest_system(data->galaxy,NULL,mouseposition,0.02);
    if (system)
    {
        gf2d_draw_circle(galaxy_position_to_screen_position(system->position), 20, vector4d(100,255,255,255));
    }
    return 1;
}

int galaxy_view_free(Window *win)
{
    GalaxyWindowData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (GalaxyWindowData*)win->data;
    free(data);
    return 0;
}

int galaxy_view_update(Window *win,List *updateList)
{
    GalaxyWindowData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (GalaxyWindowData*)win->data;
    return 0;
}

Window *galaxy_view_window(Galaxy *galaxy)
{
    Window *win;
    GalaxyWindowData *data;
    win = gf2d_window_load("menus/galaxy_view.json");
    if (!win)
    {
        slog("failed to load galaxy view window");
        return NULL;
    }
    win->no_draw_generic = 1;
    win->draw = galaxy_view_draw;
    win->update = galaxy_view_update;
    win->free_data = galaxy_view_free;
    data = gfc_allocate_array(sizeof(GalaxyWindowData),1);
    data->galaxy = galaxy;
    win->data = data;
    return win;

}


/*file's end*/
