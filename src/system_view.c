#include <simple_logger.h>

#include "gf2d_mouse.h"
#include "gf2d_draw.h"
#include "gfc_input.h"

#include "camera.h"
#include "windows_common.h"
#include "empire_hud.h"
#include "galaxy_view.h"
#include "system_view.h"
#include "planet_view.h"

typedef struct
{
    Vector2D cameraPosition;
    System *system;
    Window *childWindow;
    Planet *selectedPlanet;
    Planet *highlightedPlanet;
}SystemWindowData;


void system_view_close_child_window(Window *win)
{
    SystemWindowData *data;
    if (!win)return;
    if (!win->data)return;
    data = (SystemWindowData*)win->data;
    data->childWindow = NULL;
    camera_set_position(data->cameraPosition);
    empire_hud_bubble();
}

int system_view_draw(Window *win)
{
    Planet *planet;
    Vector2D mouseposition;
    SystemWindowData *data;
    Vector2D drawOffset;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (SystemWindowData*)win->data;
    if (data->childWindow)return 0;

    
    system_draw_system_background(data->system,vector2d(win->dimensions.x,win->dimensions.y));
    drawOffset = camera_get_offset();
    system_draw_system_lines(data->system,drawOffset);
    system_draw_system_view(data->system,drawOffset);
    
    if (!gf2d_window_mouse_in(win))
    {
        return 0;//if outside the window rect, its over something else
    }

    mouseposition = camera_get_mouse_position();
    data->highlightedPlanet = system_get_nearest_planet(data->system,NULL,mouseposition,100);
    if (data->highlightedPlanet)
    {
        gf2d_draw_circle(camera_position_to_screen(data->highlightedPlanet->systemPosition), (int)(data->highlightedPlanet->drawSize * 0.5), vector4d(100,255,255,255));
    }

    return 1;
}

int system_view_free(Window *win)
{
    SystemWindowData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (SystemWindowData*)win->data;
    free(data);
    return 0;
}

int system_view_update(Window *win,List *updateList)
{
    SystemWindowData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (SystemWindowData*)win->data;
    if (data->childWindow)return 0;

    camera_update_by_keys();
    if (!gf2d_window_mouse_in(win))
    {
        return 0;//if outside the window rect, its over something else
    }
    camera_mouse_pan();
    if (win->parent)
    {
        if (gfc_input_command_released("cancel"))
        {
            galaxy_view_close_child_window(win->parent);
            gf2d_window_free(win);
            return 1;
        }
    }
    
    if (gf2d_mouse_button_released(2))
    {
        if (data->highlightedPlanet)
        {
            data->selectedPlanet = data->highlightedPlanet;
            window_alert("planet", data->selectedPlanet->name, NULL,NULL);
            slog("planet name: [%s]",data->selectedPlanet->name);
        }
        else
        {
            data->selectedPlanet = NULL;
        }
    }
    if (gf2d_mouse_button_released(0))
    {
        if (data->highlightedPlanet)
        {
            data->cameraPosition = camera_get_position();
            data->childWindow =  planet_view_window(data->highlightedPlanet,win);
            empire_hud_bubble();
        }
    }

    
    return 0;
}

Window *system_view_window(System *system,Window *parent)
{
    Window *win;
    SystemWindowData *data;
    win = gf2d_window_load("menus/system_view.json");
    if (!win)
    {
        slog("failed to load system view window");
        return NULL;
    }
    win->no_draw_generic = 1;
    win->draw = system_view_draw;
    win->update = system_view_update;
    win->free_data = system_view_free;
    data = gfc_allocate_array(sizeof(SystemWindowData),1);
    data->system = system;
    win->data = data;
    win->parent = parent;
    camera_set_position(vector2d(-128,-128));
    return win;

}


/*file's end*/
