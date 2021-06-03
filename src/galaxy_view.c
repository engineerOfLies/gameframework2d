#include <simple_logger.h>

#include "gfc_input.h"
#include "gf2d_mouse.h"
#include "gf2d_draw.h"

#include "camera.h"
#include "windows_common.h"
#include "message_buffer.h"
#include "galaxy_view.h"
#include "system_view.h"
#include "empire_hud.h"

typedef struct
{
    float scale;
    Empire *empire;
    Vector2D cameraPosition;
    Galaxy *galaxy;
    System *selectedSystem;
    System *highlightedSystem;
    Window *childWindow;
    Sprite *home_star;
}GalaxyWindowData;

void galaxy_draw_empire_systems(Empire *empire,GalaxyWindowData *data)
{
    int i,count;
    System *system;
    Vector4D homeColorShift = {100,100,255,255};
    Vector4D colorShift = {100,255,100,255};
    Vector2D position;
    if (empire->homeSystem)
    {
        position = galaxy_position_to_screen_position(empire->homeSystem->position);
        gf2d_sprite_draw(
            data->home_star,
            vector2d(position.x - 32,position.y - 32),
            NULL,
            NULL,
            NULL,
            NULL,
            &homeColorShift,
            0);
    }
    count = gfc_list_get_count(empire->systems);
    for (i = 0; i < count; i++)
    {
        system = gfc_list_get_nth(empire->systems,i);
        if ((!system)||(system == empire->homeSystem))continue;
        position = galaxy_position_to_screen_position(system->position);
        gf2d_sprite_draw(
            data->home_star,
            vector2d(position.x - 32,position.y - 32),
            NULL,
            NULL,
            NULL,
            NULL,
            &colorShift,
            0);
    }
}

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
    galaxy_draw_empire_systems(data->empire,data);
    
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
    gf2d_window_free(data->childWindow);
    gf2d_sprite_free(data->home_star);
    free(data);
    return 0;
}

void galaxy_view_close_child_window(Window *win)
{
    GalaxyWindowData *data;
    if (!win)return;
    if (!win->data)return;
    data = (GalaxyWindowData*)win->data;
    gf2d_window_free(data->childWindow);
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
            if (system_can_view(data->highlightedSystem,data->empire->id))
            {
                data->cameraPosition = camera_get_position();
                data->childWindow = system_view_window(data->empire,data->highlightedSystem,win);
                empire_hud_bubble();
            }
            else
            {
                message_new("cannot view this system");
            }
        }
    }
    

    return 0;
}

Window *galaxy_view_window(Empire *empire, Galaxy *galaxy,Window *parent)
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
    gfc_line_cpy(win->name,"galaxy_view");
    data->galaxy = galaxy;
    data->scale = 1;
    data->empire = empire;
    data->home_star = gf2d_sprite_load_image("images/home_star.png");
    win->data = data;
    win->parent = parent;
    camera_set_position(vector2d(0,0));
    empire_hud_bubble();
    return win;
}


/*file's end*/
