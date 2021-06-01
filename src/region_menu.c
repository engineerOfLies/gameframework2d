#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_input.h"
#include "gf2d_graphics.h"
#include "gf2d_windows.h"
#include "gf2d_elements.h"
#include "gf2d_element_label.h"
#include "gf2d_draw.h"
#include "gf2d_shape.h"
#include "gf2d_mouse.h"

#include "camera.h"
#include "windows_common.h"
#include "regions.h"
#include "planet_view.h"
#include "region_menu.h"

typedef struct
{
    Empire *empire;
    Region *region;
    TextLine filename;
}RegionMenuData;

int region_menu_free(Window *win)
{
    RegionMenuData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    free(data);

    return 0;
}

int region_menu_draw(Window *win)
{
    return 0;
}

static const char *options[] = 
{
    "Fertility",
    "Minerals",
    "Habitable"
};

void onFertility(void *data)
{
    slog("Fertility survey ordered");
}
void onMinerals(void *data)
{
    slog("Mineral survey ordered");
}
void onHabitable(void *data)
{
    slog("Habitable survey ordered");
}

static void(*onOption[])(void *) = 
{
    onFertility,
    onMinerals,
    onHabitable
};

int region_menu_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    RegionMenuData* data;
    if (!win)return 0;
    if (!updateList)return 0;
    data = (RegionMenuData*)win->data;
    if (!data)return 0;
    
    if (win->parent)
    {
        if (gfc_input_command_released("cancel"))
        {
            planet_view_close_child_window(win->parent);
            gf2d_window_free(win);
            return 1;
        }
    }

    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 40:
                window_list_options("Survey Type", 3, options, onOption,win);
                return 1;
        }
    }
    return 1;
}


Window *region_menu(Empire *empire, Region *region,Window *parent)
{
    SurveyState state;
    TextLine line;
    Window *win;
    RegionMenuData* data;
    if (!region)
    {
        slog("no region provided");
        return NULL;
    }
    win = gf2d_window_load("menus/region_menu.json");
    if (!win)
    {
        slog("failed to load region menu");
        return NULL;
    }
    
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),region->name);

    
    state = empire_region_get_survey_state(empire,region->id,ST_Fertility);
    if (state > SS_Unserveyed)
    {
        
    }

    
    win->update = region_menu_update;
    win->free_data = region_menu_free;
    win->draw = region_menu_draw;
    data = (RegionMenuData*)gfc_allocate_array(sizeof(RegionMenuData),1);
    data->region = region;
    data->empire = empire;
    win->parent = parent;
    win->data = data;
    return win;
}


/*eol@eof*/
