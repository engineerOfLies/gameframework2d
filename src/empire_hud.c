#include <simple_logger.h>

#include "gfc_text.h"

#include "gf2d_mouse.h"
#include "gf2d_draw.h"
#include "gf2d_font.h"
#include "gf2d_elements.h"

#include "camera.h"
#include "message_buffer.h"

#include "galaxy.h"
#include "empire.h"

#include "galaxy_view.h"
#include "system_view.h"
#include "empire_hud.h"

typedef struct
{
    Sprite *titlebar;
    Sprite *icons;
    Galaxy *galaxy;
    Empire *empire;
    Window *view;
}EmpireHudData;

static Window *empire_window = NULL;

void empire_hud_bubble()
{
    gf2d_window_bring_to_front(empire_window);
    message_buffer_bubble();
}

void empire_hud_draw_titlebar(Empire *empire,EmpireHudData *data)
{
    int i;
    Color color;
    TextLine line;
    Vector2D bounds;
    if ((!empire)||(!data))return;
    
    gf2d_sprite_draw_image(data->titlebar,vector2d(0,0));
    
    for (i = 0;i < 5;i++)
    {
        gf2d_sprite_draw(
            data->icons,
            vector2d(562 + (i * 128),10),
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            i);
    }
    // titlebar
    gf2d_font_draw_line_tag(empire->empireName,FT_H6,gfc_color(1,1,1,1), vector2d(10,4));
    
    //credits
    if (empire->resourcesDelta.credits > 0)
    {
        color = gfc_color_hsl(120,1,0.5,1);
    }
    else if (empire->resourcesDelta.credits < 0)
    {
        color = gfc_color_hsl(0,1,0.5,1);
    }
    else
    {
        color = gfc_color8(255,255,255,255);
    }
    gfc_line_sprintf(line,"%i",(int)empire->resources.credits);
    bounds = gf2d_font_get_bounds_tag(line,FT_H5);
    gf2d_font_draw_line_tag(line,FT_H5,color, vector2d(680 - bounds.x,6));
    //minerals
    if (empire->resourcesDelta.minerals> 0)
    {
        color = gfc_color_hsl(120,1,0.5,1);
    }
    else if (empire->resourcesDelta.minerals< 0)
    {
        color = gfc_color_hsl(0,1,0.5,1);
    }
    else
    {
        color = gfc_color8(255,255,255,255);
    }
    gfc_line_sprintf(line,"%i",(int)empire->resources.minerals);
    bounds = gf2d_font_get_bounds_tag(line,FT_H5);
    gf2d_font_draw_line_tag(line,FT_H5,color, vector2d(808 - bounds.x,6));
    //agriculture
    if (empire->resourcesDelta.agriculture > 0)
    {
        color = gfc_color_hsl(120,1,0.5,1);
    }
    else if (empire->resourcesDelta.agriculture < 0)
    {
        color = gfc_color_hsl(0,1,0.5,1);
    }
    else
    {
        color = gfc_color8(255,255,255,255);
    }
    gfc_line_sprintf(line,"%i",(int)empire->resources.agriculture);
    bounds = gf2d_font_get_bounds_tag(line,FT_H5);
    gf2d_font_draw_line_tag(line,FT_H5,color, vector2d(936 - bounds.x,6));
    //population
    if (empire->resourcesDelta.population > 0)
    {
        color = gfc_color_hsl(120,1,0.5,1);
    }
    else if (empire->resourcesDelta.population < 0)
    {
        color = gfc_color_hsl(0,1,0.5,1);
    }
    else
    {
        color = gfc_color8(255,255,255,255);
    }
    gfc_line_sprintf(line,"%i",(int)empire->resources.population);
    bounds = gf2d_font_get_bounds_tag(line,FT_H5);
    gf2d_font_draw_line_tag(line,FT_H5,color, vector2d(1064 - bounds.x,6));
    //approval
    gfc_line_sprintf(line,"%i",(int)empire->resources.approval);
    bounds = gf2d_font_get_bounds_tag(line,FT_H5);
    gf2d_font_draw_line_tag(line,FT_H5,gfc_color8(255,255,255,255), vector2d(1192 - bounds.x,6));
}

int empire_hud_draw(Window *win)
{
    EmpireHudData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    gf2d_window_draw(win);

    data = (EmpireHudData*)win->data;
    empire_hud_draw_titlebar(data->empire,data);
    

    
    return 0;
}

int empire_hud_free(Window *win)
{
    EmpireHudData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (EmpireHudData*)win->data;
    
    gf2d_sprite_free(data->titlebar);
    gf2d_sprite_free(data->icons);
    free(data);
    return 0;
}

int empire_hud_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    EmpireHudData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = (EmpireHudData*)win->data;
    empire_update(data->empire);
    
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 11:
                if (gfc_line_cmp(data->view->name,"galaxy_view")==0)
                {
                    slog("child view is already galaxy view");
                    galaxy_view_close_child_window(data->view);
                }
                else
                {
                    gf2d_window_free(data->view);
                    data->view = galaxy_view_window(data->empire,data->galaxy,win); 
                }
                return 1;
        }
    }

    return 0;
}

Window *empire_hud_window(Empire *empire,Galaxy *galaxy)
{
    Window *win;
    EmpireHudData *data;
    win = gf2d_window_load("menus/empire_hud.json");
    if (!win)
    {
        slog("failed to load empire hud window");
        return NULL;
    }
    win->no_draw_generic = 1;
    win->draw = empire_hud_draw;
    win->update = empire_hud_update;
    win->free_data = empire_hud_free;
    data = gfc_allocate_array(sizeof(EmpireHudData),1);
    data->galaxy = galaxy;
    data->empire = empire;
    data->view = galaxy_view_window(empire,galaxy,win); 
    data->titlebar = gf2d_sprite_load_image("images/ui/titlebar.png");
    data->icons = gf2d_sprite_load_all("images/ui/iconsprite.png",24,24,1,0);
    win->data = data;
    empire_window = win;
    empire_hud_bubble();
    message_new("Welcome Emperor");
    return win;

}


/*file's end*/
