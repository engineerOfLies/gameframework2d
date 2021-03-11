#include <simple_logger.h>
#include "gfc_input.h"

#include "gfc_list.h"
#include "gfc_callbacks.h"

#include "gf2d_elements.h"
#include "gf2d_element_label.h"
#include "gf2d_element_actor.h"
#include "gf2d_element_entry.h"
#include "gf2d_mouse.h"

#include "build.h"
#include "level.h"
#include "keep.h"

typedef struct
{
    KeepDirection dir;
}BuildWindowData;

int build_free(Window *win)
{

    if (!win)return 0;
    if (!win->data)return 0;

    return 0;
}

void build_set_selection(Window *win,char * segment, KeepDirection dir)
{
    Actor *actor;
    Element *e;
    e = gf2d_window_get_element_by_id(win,2);
    if (!e)return;
    
    actor = gf2d_element_actor_get_actor(e);
    if (!actor)return;
    
    gf2d_element_actor_set_actor(e, segment);
    gf2d_element_actor_set_action(e, keep_get_direction_name(dir));
    gf2d_actor_next_frame(actor);
    actor->frame += 4;
}

int build_update(Window *win,List *updateList)
{
    int i,count;
    Vector2D mouseTile;
    Element *e;
    Level *level;
    BuildWindowData *data;
    if (!win)return 0;
    if (!updateList)return 0;
    data =  (BuildWindowData *)win->data;

    level = level_get_current();
    if (level)
    {
        level_get_mouse_tile(level,&mouseTile);
        
        if (gf2d_mouse_button_pressed(1))
        {
            slog("mouse 1");
            if (data->dir == 0)data->dir = KD_MAX - 1;
            else data->dir--;
            build_set_selection(win,"actors/wall_segment.json", data->dir);
        }
        else if (gf2d_mouse_button_pressed(2))
        {
            slog("mouse 2");
            data->dir = (data->dir +1)%KD_MAX;
            build_set_selection(win,"actors/wall_segment.json", data->dir);
        }
        
        
        if (gf2d_mouse_button_pressed(0))
        {
            //check if an world entity is already at this location
            //tell the level to spawn an entity there.
            keep_segment_new(vector2d(mouseTile.x*level->levelTileSize.x,mouseTile.y*level->levelTileSize.y),"actors/wall_segment.json",(int)(gfc_random()*KS_MAX), data->dir);
        }
    }
    
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
        }
    }
    return 0;
}

Window *window_build()
{
    Window *win;
    BuildWindowData *data;
    win = gf2d_window_load("config/build_window.json");
    if (!win)
    {
        slog("failed to load build window");
        return NULL;
    }
    win->update = build_update;
    win->free_data = build_free;
    data = (BuildWindowData*)gfc_allocate_array(sizeof(BuildWindowData),1);
    win->data = data;
    
    build_set_selection(win,"actors/wall_segment.json", data->dir);
    return win;
}


/*eol@eof*/
