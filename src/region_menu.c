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
#include "message_buffer.h"
#include "regions.h"
#include "planet_view.h"
#include "region_menu.h"

typedef struct
{
    Empire     *empire;
    Region     *region;
    TextLine    filename;
    Window     *childWindow;
}RegionMenuData;

int region_menu_free(Window *win)
{
    RegionMenuData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    if (data->childWindow)gf2d_window_free(data->childWindow);
    free(data);
    return 0;
}

int region_menu_draw(Window *win)
{
    return 0;
}

static const char *surveyOptions[] = 
{
    "Fertility",
    "Minerals",
    "Habitable"
};

static const char *developOptions[] = 
{
    "Fertility",
    "Minerals",
    "Habitable"
};


static void onCancel(void *cData)
{
    RegionMenuData* data;
    Window *win = (Window *)cData;
    if (!win)return;
    data = win->data;
    data->childWindow = NULL;
}

void onFertility(void *cData)
{
    int survey;
    RegionMenuData* data;
    Window *win = (Window *)cData;
    if (!win)return;
    onCancel(cData);
    data = win->data;
    survey = empire_survery_region(data->empire,data->region->id,ST_Fertility);
    if (survey < 0)
    {
        slog("Servey","Error, cannot survery");
        return;
    }
    if (survey == SS_Started)
    {
        message_new("Fertility Survey has been started");
    }
    else if (survey == SS_Underway)
    {
        message_new("Fertility servey already underway!");
    }
    else if (survey == SS_Completed)
    {
        message_new("Fertility servey already completed!");
    }
}
void onMinerals(void *cData)
{
    int survey;
    RegionMenuData* data;
    Window *win = (Window *)cData;
    if (!win)return;
    onCancel(cData);
    data = win->data;
    survey = empire_survery_region(data->empire,data->region->id,ST_Minerals);
    if (survey < 0)
    {
        slog("Servey","Error, cannot survery");
        return;
    }
    if (survey == SS_Started)
    {
        message_new("Mineral Survey has been started");
    }
    else if (survey == SS_Underway)
    {
        message_new("Mineral servey already underway!");
    }
    else if (survey == SS_Completed)
    {
        message_new("Mineral servey already completed!");
    }
}
void onHabitable(void *cData)
{
    int survey;
    RegionMenuData* data;
    Window *win = (Window *)cData;
    if (!win)return;
    onCancel(cData);
    data = win->data;
    survey = empire_survery_region(data->empire,data->region->id,ST_Habitable);
    if (survey < 0)
    {
        slog("Servey","Error, cannot survery");
        return;
    }
    if (survey == SS_Started)
    {
        message_new("Habitability Survey has been started");
    }
    else if (survey == SS_Underway)
    {
        message_new("Habitability servey already underway!");
    }
    else if (survey == SS_Completed)
    {
        message_new("Habitability servey already completed!");
    }
}

static void(*onSurveyOption[])(void *) = 
{
    onFertility,
    onMinerals,
    onHabitable
};



int region_menu_update(Window *win,List *updateList)
{
    SurveyState state;
    TextLine line;
    int i,count;
    Element *e;
    Empire *empire;
    RegionMenuData* data;
    if (!win)return 0;
    if (!updateList)return 0;
    data = (RegionMenuData*)win->data;
    if (!data)return 0;
    empire = data->empire;
    
    if (win->parent)
    {
        if (gfc_input_command_released("cancel"))
        {
            planet_view_close_child_window(win->parent);
            gf2d_window_free(win);
            return 1;
        }
    }
    
    state = empire_region_get_survey_state(empire,data->region->id,ST_Habitable);
    if (state == SS_Started)
    {
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,4),"Habitability: Started");
    }
    if (state == SS_Underway)
    {
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,4),"Habitability: Surveying");
    }
    else if (state == SS_Completed)
    {
        gfc_line_sprintf(line,"Habitability: %i",data->region->habitable);
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,4),line);
    }

    state = empire_region_get_survey_state(empire,data->region->id,ST_Fertility);
    if (state == SS_Started)
    {
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,3),"Fertility: Started");
    }
    if (state == SS_Underway)
    {
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,3),"Fertility: Surveying");
    }
    else if (state == SS_Completed)
    {
        gfc_line_sprintf(line,"Fertility: %i",data->region->fertility);
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,3),line);
    }

    state = empire_region_get_survey_state(empire,data->region->id,ST_Minerals);
    if (state == SS_Started)
    {
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,2),"Minerals: Started");
    }
    if (state == SS_Underway)
    {
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,2),"Minerals: Surveying");
    }
    else if (state == SS_Completed)
    {
        gfc_line_sprintf(line,"Minerals: %i",data->region->minerals);
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,2),line);
    }

    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 40:
                if (data->childWindow == NULL)data->childWindow = window_list_options("Survey Type", 3, surveyOptions, onSurveyOption,onCancel,win);
                return 1;
        }
    }
    return gf2d_window_mouse_in(win);
}


Window *region_menu(Empire *empire, Region *region,Window *parent)
{
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
