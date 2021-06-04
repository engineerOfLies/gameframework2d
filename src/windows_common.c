#include "simple_logger.h"

#include "gfc_list.h"
#include "gfc_text.h"
#include "gfc_input.h"
#include "gfc_callbacks.h"

#include "gf2d_font.h"
#include "gf2d_mouse.h"
#include "gf2d_elements.h"
#include "gf2d_element_label.h"
#include "gf2d_element_actor.h"
#include "gf2d_element_button.h"
#include "gf2d_element_list.h"
#include "gf2d_element_entry.h"

#include "windows_common.h"

int yes_no_free(Window *win)
{
    List *list;
    int count,i;
    Callback *callback;

    if (!win)return 0;
    if (!win->data)return 0;

    list = (List*)win->data;
    if (list)
    {
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
    }
    return 0;
}

int yes_no_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    List *callbacks;
    Callback *callback;
    if (!win)return 0;
    if (!updateList)return 0;
    callbacks = (List*)win->data;
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 51:
                callback = (Callback*)gfc_list_get_nth(callbacks,0);
                if (callback)
                {
                    gfc_callback_call(callback);
                }
                gf2d_window_free(win);
                return 1;
                break;
            case 52:
                callback = (Callback*)gfc_list_get_nth(callbacks,1);
                if (callback)
                {
                    gfc_callback_call(callback);
                }
                gf2d_window_free(win);
                return 1;
        }
    }
    return 0;
}

Window *window_yes_no(const char *text, void(*onYes)(void *),void(*onNo)(void *),void *yesData,void *noData)
{
    Window *win;
    List *callbacks;
    win = gf2d_window_load("menus/yes_no_window.json");
    if (!win)
    {
        slog("failed to load yes/no window");
        return NULL;
    }
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),(char *)text);
    win->update = yes_no_update;
    win->free_data = yes_no_free;
    callbacks = gfc_list_new();
    if (onYes)
    {
        callbacks = gfc_list_append(callbacks,gfc_callback_new(onYes,yesData));
    }
    if (onNo)
    {
        callbacks = gfc_list_append(callbacks,gfc_callback_new(onNo,noData));
    }
    win->data = callbacks;
    return win;
}


int ok_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    List *callbacks;
    Callback *callback;
    if (!win)return 0;
    if (!updateList)return 0;
    callbacks = (List*)win->data;
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 51:
                callback = (Callback*)gfc_list_get_nth(callbacks,0);
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

int alert_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    List *callbacks;
    Callback *callback;
    if (!win)return 0;
    callbacks = (List*)win->data;
    if (gf2d_mouse_button_released(0)||gf2d_mouse_button_released(1)||gf2d_mouse_button_released(2))
    {
        if (callbacks)
        {
            callback = (Callback*)gfc_list_get_nth(callbacks,0);
            if (callback)
            {
                gfc_callback_call(callback);
            }
        }
        gf2d_window_free(win);
        return 0;
    }
    if (!updateList)return 0;
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 51:
                callback = (Callback*)gfc_list_get_nth(callbacks,0);
                if (callback)
                {
                    gfc_callback_call(callback);
                }
                gf2d_window_free(win);
                return 0;
                break;
        }
    }
    return 0;
}


Window *window_alert(const char *title,const char *text, void(*onOK)(void *),void *okData)
{
    Window *win;
    List *callbacks;
    win = gf2d_window_load("menus/alert_menu.json");
    if (!win)
    {
        slog("failed to load alert window");
        return NULL;
    }
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),(char *)title);
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,2),(char *)text);
    win->update = alert_update;
    win->free_data = yes_no_free;
    if (onOK)
    {
        callbacks = gfc_list_new();
        callbacks = gfc_list_append(callbacks,gfc_callback_new(onOK,okData));
        win->data = callbacks;
    }
    return win;
}

Window *window_dialog(const char *title,const char *text, void(*onOK)(void *),void *okData)
{
    Window *win;
    List *callbacks;
    win = gf2d_window_load("menus/dialog.json");
    if (!win)
    {
        slog("failed to load alert window");
        return NULL;
    }
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),(char *)title);
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,2),(char *)text);
    win->update = alert_update;
    win->free_data = yes_no_free;
    if (onOK)
    {
        callbacks = gfc_list_new();
        callbacks = gfc_list_append(callbacks,gfc_callback_new(onOK,okData));
        win->data = callbacks;
    }
    return win;
}


Window *window_text_entry(const char *question, char *defaultText,void *callbackData, size_t length, void(*onOk)(void *),void(*onCancel)(void *))
{
    Window *win;
    List *callbacks;
    win = gf2d_window_load("menus/text_entry_window.json");
    if (!win)
    {
        slog("failed to load yes/no window");
        return NULL;
    }
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),(char *)question);
    gf2d_element_entry_set_text_pointer(gf2d_window_get_element_by_id(win,2),(char *)defaultText,length);
    win->update = yes_no_update;
    win->free_data = yes_no_free;
    callbacks = gfc_list_new();
    if (onOk)
    {
        callbacks = gfc_list_append(callbacks,gfc_callback_new(onOk,callbackData));
    }
    if (onCancel)
    {
        callbacks = gfc_list_append(callbacks,gfc_callback_new(onCancel,callbackData));
    }
    win->data = callbacks;
    return win;
}

int option_list_free(Window *win)
{
    List *list;
    int count,i;
    Callback *callback;

    if (!win)return 0;
    if (!win->data)return 0;

    list = (List*)win->data;
    if (list)
    {
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
    }
    return 0;
}

int option_list_update(Window *win,List *updateList)
{
    int i,count,n;
    Element *e;
    List *callbacks;
    Callback *callback;
    if (!win)return 0;
    callbacks = (List*)win->data;
    count = gfc_list_get_count(updateList);
    if (gfc_input_command_released("cancel"))
    {
        callback = (Callback*)gfc_list_get_nth(callbacks,count);
        if (callback)
        {   
            gfc_callback_call(callback);
        }
        gf2d_window_free(win);
        return 1;
    }
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        if ((e->index >= 500)&& (e->index < 5000))
        {
            n = e->index - 500;
            callback = (Callback*)gfc_list_get_nth(callbacks,n);
            if (callback)
                {
                    gfc_callback_call(callback);
                }
                gf2d_window_free(win);
                return 1;
                break;
        }
    }
    if (gf2d_window_mouse_in(win))return 1;
    return 0;
}

Window *window_list_options(const char *question, int n, const char*optionText[], void(*onOption[])(void *),void(*onCancel)(void *),void *data)
{
    int i;
    ButtonElement *button;
    LabelElement *label;
    ActorElement *actor;
    Element *e,*p,*l,*a;
    Window *win;
    List *callbacks;
    win = gf2d_window_load("menus/list_options.json");
    if (!win)
    {
        slog("failed to load list of options menu");
        return NULL;
    }
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),(char *)question);
    win->update = option_list_update;
    win->free_data = option_list_free;
    win->dimensions.h += (n * 68);
    gf2d_window_align(win,0);
    callbacks = gfc_list_new();
    p = gf2d_window_get_element_by_id(win,50);

    for (i = 0; i < n; i++)
    {
        e = gf2d_element_new_full(
            p,
            500 + i,
            "button",
            gf2d_rect(0,0,200,37),
            gfc_color8(255,255,255,255),
            0,
            gfc_color8(255,255,255,255),
            0);
        l = gf2d_element_new_full(
            e,
            5000 + i,
            "label",
            gf2d_rect(0,0,200,37),
            gfc_color8(255,255,255,255),
            0,
            gfc_color8(255,255,255,255),
            0);
        a = gf2d_element_new_full(
            e,
            50000 + i,
            "label",
            gf2d_rect(0,0,200,37),
            gfc_color8(255,255,255,255),
            0,
            gfc_color8(255,255,255,255),
            0);
        
        label = gf2d_element_label_new_full(
            (char *)optionText[i],
            gfc_color8(255,255,255,255),
            FT_H5,
            LJ_Center,
            LA_Middle,
            0);
        gf2d_element_make_label(l,label);

        actor = gf2d_element_actor_new_full("actors/list_button.actor", "idle",vector2d(1,1),NULL);
        gf2d_element_make_actor(a,actor);

        
        button = gf2d_element_button_new_full(
            l,
            a,
            gfc_color8(255,255,255,255),
            gfc_color8(255,255,255,255),
            0);
        gf2d_element_make_button(e,button);
        
        gf2d_element_list_add_item(p,e);

        callbacks = gfc_list_append(callbacks,gfc_callback_new(onOption[i],data));
    }
    callbacks = gfc_list_append(callbacks,gfc_callback_new(onCancel,data));
    win->data = callbacks;
    return win;
}
/*eol@eof*/
