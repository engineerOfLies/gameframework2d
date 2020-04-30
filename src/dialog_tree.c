#include "simple_logger.h"

#include "simple_json.h"

#include "gf2d_elements.h"
#include "gf2d_element_button.h"
#include "gf2d_element_list.h"
#include "gf2d_element_label.h"
#include "gf2d_element_actor.h"
#include "gf2d_font.h"

#include "dialog_tree.h"

typedef struct
{
    char *file;
    SJson *json;
    Actor actor;
}DialogTreeData;

int dialog_tree_draw(Window *win)
{
    return 0;
}

int dialog_tree_free(Window *win)
{
    DialogTreeData *dialog_tree;
    if ((!win)||(!win->data))return 0;
    dialog_tree = (DialogTreeData *)win->data;
    sj_free(dialog_tree->json);
    free(dialog_tree);
    return 0;
}


int dialog_tree_update(Window *win,List *updateList)
{
    
    return 1;
}

void dialog_tree_add_topic(Window *win,SJson *topicJson, int i)
{
    Element *e,*b;
    LabelElement *le;
    SJson *value;
    const char *topic;
    if ((!win)||(!topicJson))
    {
        return;
    }
    value = sj_object_get_value(topicJson,"topic");
    if (!value)
    {
        slog("dialog topic missing 'topic'");
        return;
    }
    topic = sj_get_string_value(value);
    if (!strlen(topic))
    {
        slog("topic string empty");
        return;
    }
    slog("building dialog option for topic '%s'",topic);
    b = gf2d_element_new_full(
            gf2d_window_get_element_by_id(win,1000),
            1001+i,
            (char *)topic,
            gf2d_rect(0,0,690,34),
            gfc_color(1,1,1,1),
            0,
            gfc_color(.5,.5,.5,1),1);
    e = gf2d_element_new_full(
            b,
            2000+i,
            (char *)topic,
            gf2d_rect(0,0,690,34),
            gfc_color(1,1,1,1),
            0,
            gfc_color(1,1,1,1),0);
    le = gf2d_element_label_new_full((char *)topic,gfc_color(1,1,1,1),FT_H5,LJ_Left,LA_Middle,true);
    gf2d_element_make_label(e,le);

    gf2d_element_make_button(b,gf2d_element_button_new_full(e,NULL,gfc_color(1,1,1,1),gfc_color(0.9,0.9,0.9,1),1));
    gf2d_element_list_add_item(gf2d_window_get_element_by_id(win,1000),b);
}

void dialog_tree_setup_topics(Window *win, DialogTreeData *data)
{
    SJson *topics,*topic,*value;
    int i,c;
    const char *str;
    if ((!win)||(!data)||(!data->json))return;
    value = sj_object_get_value(data->json,"title");
    if (value)
    {
        str = sj_get_string_value(value);
        if (str)
        {
            gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),(char *)str);
        }
    }
    value = sj_object_get_value(data->json,"description");
    if (value)
    {
        str = sj_get_string_value(value);
        if (str)
        {
            gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,2),(char *)str);
        }
    }
    topics = sj_object_get_value(data->json,"topics");
    if (!topics)
    {
        slog("dialog json missing topics");
        return;
    }
    c = sj_array_get_count(topics);
    slog("%i topics for dialog tree",c);
    for (i = 0; i < c; i ++)
    {
        topic = sj_array_get_nth(topics,i);
        if (!topic)return;
        dialog_tree_add_topic(win,topic,i);
    }
}


Window *dialog_tree_new(char *filename,Entity *player)
{
    SJson *json;
    Window *win;
    DialogTreeData* data;
    win = gf2d_window_load("menus/dialog_tree.json");
    if (!win)
    {
        slog("failed to load dialog tree");
        return NULL;
    }
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to load dialog json file %s",filename);
        gf2d_window_free(win);
        return NULL;
    }
    win->update = dialog_tree_update;
    win->free_data = dialog_tree_free;
    win->draw = dialog_tree_draw;
    data = (DialogTreeData*)gfc_allocate_array(sizeof(DialogTreeData),1);
    win->data = data;
    data->json = json;
    dialog_tree_setup_topics(win,data);
    return win;
}

/*eol@eof*/
