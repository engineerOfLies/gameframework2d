#include <simple_logger.h>

#include "gf2d_mouse.h"
#include "gf2d_draw.h"
#include "gfc_input.h"

#include "camera.h"
#include "galaxy_view.h"
#include "system_view.h"

typedef struct
{
    System *system;
    Window *childWindow;
}SystemWindowData;


int system_view_draw(Window *win)
{
    Planet *planet;
    Vector2D mouseposition;
    SystemWindowData *data;
    Vector2D drawOffset;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (SystemWindowData*)win->data;
    system_draw_system_background(data->system,vector2d(win->dimensions.x,win->dimensions.y));
    drawOffset = camera_get_offset();
    system_draw_system_lines(data->system,drawOffset);
    system_draw_system_view(data->system,drawOffset);
    
    
    mouseposition = camera_get_mouse_position();
    planet = system_get_nearest_planet(data->system,NULL,mouseposition,100);
    if (planet)
    {
        gf2d_draw_circle(camera_position_to_screen(planet->systemPosition), (int)(planet->drawSize * 0.5), vector4d(100,255,255,255));
        slog("planet position: (%f,%f)",planet->systemPosition.x,planet->systemPosition.y);
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
    camera_update_by_keys();
    if (win->parent)
    {
        if (gfc_input_command_released("cancel"))
        {
            galaxy_view_close_child_window(win->parent);
            gf2d_window_free(win);
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
