#include "simple_logger.h"

#include "simple_json.h"

#include "gf2d_elements.h"
#include "gf2d_element_button.h"
#include "gf2d_element_list.h"
#include "gf2d_element_label.h"
#include "gf2d_element_actor.h"
#include "gf2d_font.h"
#include "gf2d_mouse.h"

#include "windows_common.h"
#include "scene.h"
#include "exhibits.h"
#include "dialog_tree.h"

typedef struct
{
    const char *speaker;
    SJson *json;
    Entity *player;
    Window *parent;
    Window *child;
}DialogBranchData;

void dialog_child_closed(Window *win);
Window *dialog_branch_new(Window *parent, SJson *json,Entity *player,const char *speaker);

int dialog_branch_draw(Window *win)
{
    return 0;
}

int dialog_branch_free(Window *win)
{
    DialogBranchData *dialog_branch;
    if ((!win)||(!win->data))return 0;
    dialog_branch = (DialogBranchData *)win->data;
    dialog_child_closed(dialog_branch->parent);
    free(dialog_branch);
    return 0;
}

void dialog_branch_handle_topic(Window *win, DialogBranchData *data,int n)
{
    int i,c;
    SJson *topic = NULL,*topics = NULL,*effects = NULL,*effect,*value = NULL,*type;
    const char *str = NULL;
    
    
    topics = sj_object_get_value(data->json,"topics");
    if (!topics)
    {
        slog("no topics found for the chosen topic??");
        return;
    }
    topic = sj_array_get_nth(topics,n);
    if (!topic)
    {
        slog("topic missing!");
        return;
    }
    effects = sj_object_get_value(topic,"effects");
    if (effects)
    {
        c = sj_array_get_count(effects);
        for (i = 0; i < c; i++)
        {
            effect = sj_array_get_nth(effects,i);
            if (!effect)continue;
            type = sj_object_get_value(effect,"type");
            if (!type)continue;
            str = sj_get_string_value(type);
            if (!str)continue;
            if (strcmp(str,"dialog")==0)
            {
                value = sj_object_get_value(effect,"dialog");
                if (!value)continue;
                data->child = dialog_branch_new(win, value,data->player,data->speaker);

                continue;
            }
        }
    }
    value = sj_object_get_value(topic,"response");
    if (value)
    {
        str = sj_get_string_value(value);
        if (str != NULL)
        {
            window_dialog((char *)data->speaker, (char*)str, NULL,NULL);
        }
    }
}

int dialog_branch_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    DialogBranchData* data;
    if (!win)return 0;
    if (!updateList)return 0;
    data = (DialogBranchData*)win->data;
    if (!data)return 1;
    if (data->child != NULL)return 1;
    
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 51:
                gf2d_window_free(win);
                return 1;
        }
        if (e->index > 1000)
        {
            dialog_branch_handle_topic(win, data,e->index - 1001);
            return 1;
        }
    }
    return 1;
}

void dialog_branch_add_done(Window *win,char *doneText)
{
    Element *e,*b;
    LabelElement *le;
    b = gf2d_element_new_full(
            gf2d_window_get_element_by_id(win,1000),
            51,
            doneText,
            gf2d_rect(0,0,690,34),
            gfc_color(1,1,1,1),
            0,
            gfc_color(.5,.5,.5,1),1);
    e = gf2d_element_new_full(
            b,
            510,
            doneText,
            gf2d_rect(0,0,690,34),
            gfc_color(1,1,1,1),
            0,
            gfc_color(1,1,1,1),0);
    le = gf2d_element_label_new_full(doneText,gfc_color(1,1,1,1),FT_H5,LJ_Left,LA_Middle,true);
    gf2d_element_make_label(e,le);

    gf2d_element_make_button(b,gf2d_element_button_new_full(e,NULL,gfc_color(1,1,1,1),gfc_color(0.9,0.9,0.9,1),1));
    gf2d_element_list_add_item(gf2d_window_get_element_by_id(win,1000),b);
}


void dialog_branch_add_topic(Window *win,SJson *topicJson, int i)
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

void dialog_branch_setup_topics(Window *win, DialogBranchData *data)
{
    SJson *topics,*topic;
    int i,c;
    if ((!win)||(!data)||(!data->json))return;
    topics = sj_object_get_value(data->json,"topics");
    if (!topics)
    {
        slog("dialog json missing topics");
        return;
    }
    c = sj_array_get_count(topics);
    for (i = 0; i < c; i ++)
    {
        topic = sj_array_get_nth(topics,i);
        if (!topic)return;
        dialog_branch_add_topic(win,topic,i);
    }
}


Window *dialog_branch_new(Window *parent, SJson *json,Entity *player,const char *speaker)
{
    SJson *value;
    const char *doneText = "Done";
    Window *win;
    DialogBranchData* data;
    win = gf2d_window_load("menus/dialog_branch.json");
    if (!win)
    {
        slog("failed to load dialog branch");
        return NULL;
    }
    if (!json)
    {
        slog("dialog branch not given json");
        gf2d_window_free(win);
        return NULL;
    }
    win->update = dialog_branch_update;
    win->free_data = dialog_branch_free;
    win->draw = dialog_branch_draw;
    data = (DialogBranchData*)gfc_allocate_array(sizeof(DialogBranchData),1);
    gfc_line_cpy(win->name,"dialog_branch");
    
    win->data = data;
    data->json = json;
    data->player = player;
    data->parent = parent;
    data->speaker = speaker;
    dialog_branch_setup_topics(win,data);
    
    value = sj_object_get_value(data->json,"done");
    if (value)
    {
        doneText = sj_get_string_value(value);
    }
    dialog_branch_add_done(win,(char *)doneText);
    
    gf2d_mouse_set_function(MF_Pointer);
    return win;
}

typedef struct
{
    char *file;
    SJson *json;
    Actor actor;
    Entity *player;
}DialogTreeData;

void dialog_child_closed(Window *win)
{
    DialogBranchData *branchData = NULL;
    DialogTreeData *treeData = NULL;
    if (!win)return;
    if (gfc_line_cmp(win->name,"dialog_tree")==0)
    {
        treeData = win->data;
        if (!treeData)return;
        gf2d_window_free(win);
        return;
    }
    if (gfc_line_cmp(win->name,"dialog_branch")==0)
    {
        branchData = win->data;
        if (!branchData)return;
        branchData->child = NULL;
    }
}

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
    gf2d_mouse_set_function(scene_get_mouse_function(scene_get_active()));
    exhibit_unpause(NULL);
    return 0;
}


int dialog_tree_update(Window *win,List *updateList)
{
    return 1;
}

Window *dialog_tree_new(char *filename,Entity *player)
{
    SJson *json;
    SJson *value;
    Window *win;
    const char *str = NULL,*speaker = NULL;
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
    gfc_line_cpy(win->name,"dialog_tree");
    win->update = dialog_tree_update;
    win->free_data = dialog_tree_free;
    win->draw = dialog_tree_draw;
    data = (DialogTreeData*)gfc_allocate_array(sizeof(DialogTreeData),1);
    win->data = data;
    data->json = json;
    data->player = player;
    
    value = sj_object_get_value(data->json,"title");
    if (value)
    {
        speaker = sj_get_string_value(value);
        if (speaker)
        {
            gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),(char *)speaker);
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
    dialog_branch_new(win, data->json,player,speaker);
    gf2d_mouse_set_function(MF_Pointer);
    return win;
}

/*eol@eof*/
