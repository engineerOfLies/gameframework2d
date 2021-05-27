#include <simple_logger.h>

#include "gf2d_mouse.h"
#include "gf2d_draw.h"
#include "gfc_input.h"

#include "camera.h"
#include "empire_hud.h"
#include "windows_common.h"
#include "galaxy_view.h"
#include "system_view.h"
#include "planet_view.h"

typedef struct
{
    Vector2D pressPosition;
    Vector2D cameraPosition;
    Planet *planet;
    Region *selectedRegion;
    Region *highlightedRegion;
    Window *childWindow;
}PlanetWindowData;


int planet_view_draw(Window *win)
{
    Region *region;
    PlanetWindowData *data;
    Vector2D drawOffset, mousePosition;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (PlanetWindowData*)win->data;
    drawOffset = camera_get_offset();
    drawOffset.y += 40;
    planet_draw_planet_view(data->planet,drawOffset);
    
    
    if (!gf2d_window_mouse_in(win))
    {
        return 0;//if outside the window rect, its over something else
    }

    mousePosition = camera_get_mouse_position();
    region = planet_get_region_by_position(data->planet,mousePosition);
    if (region)
    {
        data->highlightedRegion = region;
        drawOffset = camera_position_to_screen(region->drawPosition);
        drawOffset.y += 40;
        gf2d_draw_circle(drawOffset, 128, vector4d(100,255,255,255));
    }
    
    if (data->selectedRegion)
    {
        drawOffset = camera_position_to_screen(data->selectedRegion->drawPosition);
        drawOffset.y += 40;
        gf2d_draw_circle(drawOffset, 128, vector4d(255,255,100,255));        
    }
    
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
    camera_bind();
    if (win->parent)
    {
        if (gfc_input_command_released("cancel"))
        {
            system_view_close_child_window(win->parent);
            gf2d_window_free(win);
            return 1;
        }
    }
    
    if (gf2d_mouse_button_released(2))
    {
        if (data->highlightedRegion)
        {
            data->selectedRegion = data->highlightedRegion;
            window_alert("region", region_name_from_biome(data->selectedRegion->biome), NULL,NULL);
        }
        else
        {
            data->selectedRegion = NULL;
        }
    }
    if (gf2d_mouse_button_released(0))
    {
        if (data->highlightedRegion)
        {
            //open up region window
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
    camera_set_bounds(-128,-128,planet->area.x,planet->area.y);
    camera_set_position(vector2d(-128,-128));
    empire_hud_bubble();
    slog("planet draw area: (%f,%f)",planet->area.x,planet->area.y);
    return win;

}


/*file's end*/
