#include "simple_logger.h"

#include "gfc_list.h"

#include "items.h"

static List *_item_list = NULL;


void item_list_free()
{
    Item *item;
    int c,i;
    
    c = gfc_list_get_count(_item_list);
    for (i = 0;i < c;i++)
    {
        item = gfc_list_get_nth(_item_list,i);
        if (!item)continue;
        item_free(item);
    }
    gfc_list_delete(_item_list);
    _item_list = NULL;
}

void item_list_load(char *filename)
{
    int c,i;
    SJson *json,*js_item_list,*js_item;
    Item *item;
    if (!filename)return;
    if (_item_list != NULL)
    {
        item_list_free();
    }
    else
    {
        atexit(item_list_free);
    }
    json = sj_load(filename);
    if (!json)return;
    
    _item_list = gfc_list_new();
    
    js_item_list = sj_object_get_value(json,"item_list");
    
    if (!js_item_list)
    {
        slog("item list json missing tag item_list");
        sj_free(json);
        return;
    }
    
    c = sj_array_get_count(js_item_list);
    for (i = 0; i < c; i++)
    {
        js_item = sj_array_get_nth(js_item_list,i);
        if (!js_item)continue;
        item = item_parse_from_json(js_item);
        _item_list = gfc_list_append(_item_list,item);
    }
    sj_free(json);
}

Item *item_list_get_by_name(char *name)
{
    int i,c;
    Item *item;
    c = gfc_list_get_count(_item_list);
    for (i = 0; i < c; i++)
    {
        item = gfc_list_get_nth(_item_list,i);
        if (!item)continue;
        if (gfc_line_cmp(item->name,name)==0)
        {
            return item;
        }
    }
    return NULL;
}

Item *item_new()
{
    Item *item;
    item = gfc_allocate_array(sizeof(Item),1);
    
    return item;
}

void item_free(Item *item)
{
    if (!item)return;
    
    free(item);
}

Item *item_parse_from_json(SJson *json)
{
    Item *item;
    const char *buffer;
    if (!json)return NULL;
    
    item = item_new();
    if (!item)return NULL;
    
    buffer = sj_get_string_value(sj_object_get_value(json,"name"));
    if ((buffer)&&(strlen(buffer)))
        gfc_line_cpy(item->name,buffer);
    
    buffer = sj_get_string_value(sj_object_get_value(json,"actor"));
    if ((buffer)&&(strlen(buffer)))
        gfc_line_cpy(item->actor,buffer);

    buffer = sj_get_string_value(sj_object_get_value(json,"action"));
    if ((buffer)&&(strlen(buffer)))
        gfc_line_cpy(item->action,buffer);

    buffer = sj_get_string_value(sj_object_get_value(json,"description"));
    if ((buffer)&&(strlen(buffer)))
        gfc_line_cpy(item->description,buffer);
    
    sj_get_integer_value(sj_object_get_value(json,"healthCost"),&item->healthCost);
    sj_get_integer_value(sj_object_get_value(json,"staminaCost"),&item->staminaCost);
    sj_get_integer_value(sj_object_get_value(json,"manaCost"),&item->manaCost);
    sj_get_integer_value(sj_object_get_value(json,"cost"),&item->cost);
    sj_get_integer_value(sj_object_get_value(json,"stackable"),&item->stackable);
    sj_get_integer_value(sj_object_get_value(json,"stackLimit"),&item->stackLimit);
    return item;
}


/*eol@eof*/
