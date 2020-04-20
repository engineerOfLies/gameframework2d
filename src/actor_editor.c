#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_callbacks.h"

#include "gf2d_elements.h"
#include "gf2d_element_label.h"
#include "gf2d_element_actor.h"
#include "gf2d_element_entry.h"

#include "windows_common.h"
#include "actor_editor.h"


typedef struct
{
    List *callbacks;    /**<functions for callbacks when window is complete*/
    char *actorfile;    /**<location for storing the actor file name*/
    char *action;       /**<location for storing the action name*/
    TextLine entryText; /**<used for window popups*/
    void *data;         /**<*/
    Window *subwindow;
}AEData;

int action_editor_free(Window *win)
{
    List *list;
    int count,i;
    Callback *callback;
    AEData *aedata = NULL;

    if (!win)return 0;
    if (!win->data)return 0;

    aedata = (AEData*)win->data;
    list = aedata->callbacks;
    count = gfc_list_get_count(list);

    for (i = 0; i < count; i++)
    {
        callback = (Callback*)gfc_list_get_nth(list,i);
        if (callback)
        {
            gfc_callback_free(callback);
        }
    }

    gfc_list_delete(list);
    free(aedata);
    return 0;
}

void onActorFilenameOK(void *data)
{
    Window *win;
    TextLine label;
    AEData *aedata;
    const char *name;
    if (!data)return;
    win = (Window*)data;
    aedata = (AEData*)win->data;
    if (!aedata)return;
    aedata->subwindow = NULL;
    gf2d_element_actor_set_actor(gf2d_window_get_element_by_id(win,2),aedata->entryText);
    gf2d_element_actor_auto_scale(gf2d_window_get_element_by_id(win,2));

    gfc_line_sprintf(label, "Actor file: %s",aedata->entryText);
    
    gfc_line_cpy(aedata->actorfile,aedata->entryText);
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,31),label);

    name = gf2d_element_actor_get_action_name(gf2d_window_get_element_by_id(win,2));
    gfc_line_sprintf(label, "action: %s",name);
    gfc_line_cpy(aedata->action,name);
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,41),label);

}

void onActorFilenameCancel(void *data)
{
    Window *win;
    AEData *aedata;
    if (!data)return;
    win = (Window*)data;
    aedata = (AEData*)win->data;
    if (!aedata)return;
    aedata->subwindow = NULL;
}

int actor_editor_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    AEData *aedata;
    List *callbacks;
    const char *name;
    Callback *callback;
    TextLine label;

    if (!win)return 0;
    aedata = (AEData*)win->data;
    if (!aedata)return 0;
    callbacks = (List *)aedata->callbacks;
    if (!updateList)return 0;

    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 452:
                gf2d_element_actor_next_action(gf2d_window_get_element_by_id(win,2));
                name = gf2d_element_actor_get_action_name(gf2d_window_get_element_by_id(win,2));
                gfc_line_sprintf(label, "action: %s",name);
                gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,41),label);
                gfc_line_cpy(aedata->action,name);
                break;
            case 350:
                if (aedata->subwindow)break;
                gfc_line_sprintf(aedata->entryText,"actors/");
                aedata->subwindow = window_text_entry("Enter Actor File Name", aedata->entryText, win, GFCLINELEN, onActorFilenameOK,onActorFilenameCancel);
                break;
            case 55:
                callback = (Callback*)gfc_list_get_nth(callbacks,0);
                if (callback)
                {
                    gfc_callback_call(callback);
                }
                gf2d_window_free(win);
                return 1;
                break;
            case 56:
                callback = (Callback*)gfc_list_get_nth(callbacks,1);
                if (callback)
                {
                    gfc_callback_call(callback);
                }
                gf2d_window_free(win);
                return 1;
                break;
        }
    }
    return 0;
}


Window *actor_editor_menu(char *actorFile,char *action,void *data, void(*onOk)(void *),void(*onCancel)(void *))
{
    Window *win;
    AEData *aedata;
    List *callbacks;
    TextLine label;
    win = gf2d_window_load("menus/actor_window.json");
    if (!win)
    {
        slog("failed to load actor window");
        return NULL;
    }
    win->data = gfc_allocate_array(sizeof(AEData),1);
    if (!win->data)
    {
        gf2d_window_free(win);
        return NULL;
    }
    aedata = (AEData *)win->data;
    aedata->actorfile = actorFile;
    aedata->action = action;
    
    if ((actorFile)&&(strlen(actorFile) > 0))
    {
        gf2d_element_actor_set_actor(gf2d_window_get_element_by_id(win,2),actorFile);
        gf2d_element_actor_set_action(gf2d_window_get_element_by_id(win,2), action);
        gfc_line_sprintf(label, "Actor file: %s",actorFile);
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,31),label);
        gfc_line_sprintf(label, "action: %s",action);
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,41),label);
        
    }
    
    win->update = actor_editor_update;
    win->free_data = action_editor_free;
    
    callbacks = gfc_list_new();
    callbacks = gfc_list_append(callbacks,gfc_callback_new(onOk,data));
    callbacks = gfc_list_append(callbacks,gfc_callback_new(onCancel,data));
    aedata->callbacks = callbacks;
    return win;
}

/*eol@eof*/
