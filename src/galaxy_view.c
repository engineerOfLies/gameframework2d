#include <simple_logger.h>

#include "gfc_input.h"
#include "gf2d_mouse.h"
#include "gf2d_draw.h"

#include "camera.h"
#include "windows_common.h"
#include "galaxy_view.h"
#include "system_view.h"
#include "empire_hud.h"

typedef struct
{
    float scale;
    Vector2D cameraPosition;
    Galaxy *galaxy;
    System *selectedSystem;
    System *highlightedSystem;
    Window *childWindow;
}GalaxyWindowData;


int galaxy_view_draw(Window *win)
{
    System *system;
    Vector2D circlePosition;
    Vector2D mouseposition;
    Vector2D cameraOffset;
    GalaxyWindowData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (GalaxyWindowData*)win->data;
    if (data->childWindow)return 0;
    
    cameraOffset = camera_get_offset();
    
    galaxy_draw(data->galaxy,vector2d(win->dimensions.x + cameraOffset.x,win->dimensions.y + cameraOffset.y),data->scale);
    
    if (!gf2d_window_mouse_in(win))
    {
        return 0;//if outside the window rect, its over something else
    }

    mouseposition = gf2d_mouse_get_position();
    mouseposition.x -= win->dimensions.x + cameraOffset.x;
    mouseposition.y -= win->dimensions.y + cameraOffset.y;
    mouseposition = galaxy_position_from_screen_position(mouseposition);
    system = galaxy_get_nearest_system(data->galaxy,NULL,mouseposition,0.02);
    if (system)
    {
        data->highlightedSystem = system;
        circlePosition = galaxy_position_to_screen_position(system->position);
        circlePosition.x += win->dimensions.x + cameraOffset.x;
        circlePosition.y += win->dimensions.y + cameraOffset.y;
        gf2d_draw_circle(circlePosition, 20, vector4d(100,255,255,255));
    }
    else
    {
        data->highlightedSystem = NULL;
    }
    if (data->selectedSystem != NULL)
    {
        circlePosition = galaxy_position_to_screen_position(data->selectedSystem->position);
        circlePosition.x += win->dimensions.x + cameraOffset.x;
        circlePosition.y += win->dimensions.y + cameraOffset.y;
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
    if (!win)return;
    if (!win->data)return;
    data = (GalaxyWindowData*)win->data;
    data->childWindow = NULL;
    camera_set_position(data->cameraPosition);
    empire_hud_bubble();
}

int galaxy_view_update(Window *win,List *updateList)
{
    GalaxyWindowData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (GalaxyWindowData*)win->data;
    if (data->childWindow)return 0;
    
    camera_update_by_keys();
    
    // mouse stuff
    if (!gf2d_window_mouse_in(win))
    {
        return 0;//if outside the window rect, its over something else
    }
    camera_mouse_pan();
  
    if (gf2d_mouse_button_released(2))
    {
        if (data->highlightedSystem)
        {
            data->selectedSystem = data->highlightedSystem;
            window_alert("system", data->highlightedSystem->name, NULL,NULL);
        }
        else
        {
            data->selectedSystem = NULL;
        }
    }
    if (gf2d_mouse_button_released(0))
    {
        if (data->highlightedSystem)
        {
            data->cameraPosition = camera_get_position();
            data->childWindow = system_view_window(data->highlightedSystem,win);
            empire_hud_bubble();
        }
    }
    
    if (gfc_input_mouse_wheel_up())
    {
        data->scale += 0.1;
    }
    if (gfc_input_mouse_wheel_down())
    {
        data->scale -= 0.1;
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
    data->scale = 1;
    win->data = data;
    win->parent = parent;
    empire_hud_bubble();
    return win;
}


/*file's end*/
