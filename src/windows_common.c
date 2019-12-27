#include "simple_logger.h"

#include "gfc_list.h"
#include "gfc_callbacks.h"

#include "gf2d_elements.h"
#include "gf2d_element_label.h"
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

Window *window_yes_no(char *text, void(*onYes)(void *),void(*onNo)(void *),void *yesData,void *noData)
{
    Window *win;
    List *callbacks;
    win = gf2d_window_load("config/yes_no_window.json");
    if (!win)
    {
        slog("failed to load yes/no window");
        return NULL;
    }
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),text);
    win->update = yes_no_update;
    win->free_data = yes_no_free;
    callbacks = gfc_list_new();
    callbacks = gfc_list_append(callbacks,gfc_callback_new(onYes,yesData));
    callbacks = gfc_list_append(callbacks,gfc_callback_new(onNo,noData));
    win->data = callbacks;
    return win;
}

Window *window_text_entry(char *question, char *defaultText, size_t length, void(*onOk)(void *),void(*onCancel)(void *))
{
    Window *win;
    List *callbacks;
    win = gf2d_window_load("config/text_entry_window.json");
    if (!win)
    {
        slog("failed to load yes/no window");
        return NULL;
    }
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),question);
    gf2d_element_entry_set_text_pointer(gf2d_window_get_element_by_id(win,2),defaultText,length);
    win->update = yes_no_update;
    win->free_data = yes_no_free;
    callbacks = gfc_list_new();
    callbacks = gfc_list_append(callbacks,gfc_callback_new(onOk,defaultText));
    callbacks = gfc_list_append(callbacks,gfc_callback_new(onCancel,defaultText));
    win->data = callbacks;
    return win;
}

/*eol@eof*/
