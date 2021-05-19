#include <simple_logger.h>

#include "gf2d_mouse.h"
#include "gf2d_draw.h"

#include "galaxy_view.h"
#include "system_view.h"

typedef struct
{
    Galaxy *galaxy;
    System *selectedSystem;
    System *highlightedSystem;
    Window *systemViewWindow;
}GalaxyWindowData;


int galaxy_view_draw(Window *win)
{
    System *system;
    Vector2D circlePosition;
    Vector2D mouseposition;
    GalaxyWindowData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (GalaxyWindowData*)win->data;
    galaxy_draw(data->galaxy,vector2d(win->dimensions.x,win->dimensions.y));
    mouseposition = gf2d_mouse_get_position();
    mouseposition.x -= win->dimensions.x;
    mouseposition.y -= win->dimensions.y;
    mouseposition = galaxy_position_from_screen_position(mouseposition);
    system = galaxy_get_nearest_system(data->galaxy,NULL,mouseposition,0.02);
    if (system)
    {
        data->highlightedSystem = system;
        circlePosition = galaxy_position_to_screen_position(system->position);
        circlePosition.x += win->dimensions.x;
        circlePosition.y += win->dimensions.y;
        gf2d_draw_circle(circlePosition, 20, vector4d(100,255,255,255));
    }
    else
    {
        data->highlightedSystem = NULL;
    }
    if (data->selectedSystem != NULL)
    {
        circlePosition = galaxy_position_to_screen_position(data->selectedSystem->position);
        circlePosition.x += win->dimensions.x;
        circlePosition.y += win->dimensions.y;
        gf2d_draw_circle(circlePosition, 20, vector4d(255,255,100,255));
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

void galaxy_view_close_child_window(Window *win)
{
    GalaxyWindowData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (GalaxyWindowData*)win->data;
    data->systemViewWindow = NULL;
    if (win->parent)
    {
        gf2d_window_bring_to_front(win->parent);
    }
}

int galaxy_view_update(Window *win,List *updateList)
{
    GalaxyWindowData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (GalaxyWindowData*)win->data;
    if (data->systemViewWindow)return 0;
    if (gf2d_mouse_button_released(2))
    {
        if (data->highlightedSystem)
        {
            data->selectedSystem = data->highlightedSystem;
        }
        else
        {
            data->selectedSystem = NULL;
        }
    }
    if (gf2d_mouse_button_released(1))
    {
        if (data->highlightedSystem)
        {
            data->systemViewWindow = system_view_window(data->highlightedSystem,win);
        }
    }
    return 0;
}

Window *galaxy_view_window(Galaxy *galaxy,Window *parent)
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
    win->parent = parent;
    return win;

}


/*file's end*/
