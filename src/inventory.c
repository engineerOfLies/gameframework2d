#include "simple_logger.h"

#include "items.h"
#include "inventory.h"

void inventory_item_free(InventoryItem *item);
InventoryItem *inventory_item_new();
InventoryItem *inventory_item_from_json(SJson *json);
SJson *inventory_item_to_json(InventoryItem *item);

int inventory_get_count(Inventory *inven)
{
    if (!inven)
    {
        slog("NULL inventory provided");
        return 0;
    }
    return gfc_list_get_count(inven->items);
}

InventoryItem *inventory_get_nth(Inventory *inven,int i)
{
    if (!inven)return NULL;
    return gfc_list_get_nth(inven->items,i);
}

int inventory_give_item(Inventory *inven,char *name, int count, int skill)
{
    Item *itemInfo;
    InventoryItem *item;
    if (!inven)return 0;
    itemInfo = item_list_get_by_name(name);
    if (!itemInfo)return 0;
    item = inventory_get_item(inven,name);
    if (item != NULL)
    {
        if (!itemInfo->stackable)
        {
            return 0;
        }
        if (item->count >= itemInfo->stackLimit)
        {
            return 0;
        }
        item->count += count;
        item->skill = skill;
        if (item->count >= itemInfo->stackLimit)
        {
            item->count = itemInfo->stackLimit;
        }
        return 1;
    }
    item = inventory_item_new();
    if (!item)return 0;
    gfc_line_cpy(item->name,name);
    item->count = count;
    item->skill = skill;
    inven->items = gfc_list_append(inven->items,item);
    return 1;
}

InventoryItem *inventory_get_item(Inventory *inven,char *name)
{
    int i,c;
    InventoryItem *item;
    if (!inven)return NULL;
    c = gfc_list_get_count(inven->items);
    for (i = 0; i < c; i++)
    {
        item = gfc_list_get_nth(inven->items,i);
        if (!item)continue;
        if (gfc_line_cmp(name,item->name)==0)return item;
    }
    return NULL;
}

int inventory_get_item_count(Inventory *inven,char *name)
{
    InventoryItem *item;
    item = inventory_get_item(inven,name);
    if (!item)return 0;
    return item->count;
}

int inventory_get_item_skill(Inventory *inven,char *name)
{
    InventoryItem *item;
    item = inventory_get_item(inven,name);
    if (!item)return 0;
    return item->skill;
}

Inventory *inventory_parse_from_json(SJson *json)
{
    int i,c;
    SJson *array,*sjItem;
    Inventory *inven;
    InventoryItem *item;
    if (!json)return NULL;
    inven = inventory_new();
    if (!inven)return NULL;
    array = sj_object_get_value(json,"items");
    if (!array)
    {
        slog("inventory json missing items object");
        inventory_free(inven);
        return NULL;
    }
    c = sj_array_get_count(array);
    for (i = 0; i < c; i++)
    {
        sjItem = sj_array_get_nth(array,i);
        if (!sjItem)continue;
        item = inventory_item_from_json(sjItem);
        if (item)
        {
            gfc_list_append(inven->items,item);
        }
    }
    
    return inven; 
}

SJson *inventory_to_json(Inventory *inven)
{
    int i,c;
    InventoryItem *item;
    SJson *json,*array,*jsItem;
    if (!inven)return NULL;
    json = sj_object_new();
    if (!json)return NULL;
    
    array = sj_array_new();
    
    sj_object_insert(json,"items",array);
    
    c = gfc_list_get_count(inven->items);
    for (i = 0; i < c; i++)
    {
        item = gfc_list_get_nth(inven->items,i);
        if (!item)continue;
        jsItem = inventory_item_to_json(item);
        if (jsItem)
        {
            sj_array_append(array,jsItem);
        }
    }
    return json;
}

Inventory *inventory_new()
{
    Inventory *inven;
    inven = gfc_allocate_array(sizeof(Inventory),1);
    if (!inven)return NULL;
    
    inven->items = gfc_list_new();
    
    return inven;
}

void inventory_free(Inventory *inven)
{
    int i,c;
    InventoryItem *item;
    if (!inven)return;
    c = gfc_list_get_count(inven->items);
    for (i = 0; i < c; i++)
    {
        item = gfc_list_get_nth(inven->items,i);
        if (!item)continue;
        inventory_item_free(item);
    }
    gfc_list_delete(inven->items);
    free(inven);
}

InventoryItem *inventory_item_new()
{
    InventoryItem *item;
    item = gfc_allocate_array(sizeof(InventoryItem),1);
    if (!item)return NULL;
    
    return item;
}

void inventory_item_free(InventoryItem *item)
{
    if (!item)return;
    free(item);
}

InventoryItem *inventory_item_from_json(SJson *json)
{
    InventoryItem *item;
    const char *str;
    if (!json)return NULL;
    item = inventory_item_new();
    if (!item)return NULL;
    
    str = sj_get_string_value(sj_object_get_value(json,"name"));
    if (str)gfc_line_cpy(item->name,str);
    
    sj_get_integer_value(sj_object_get_value(json,"count"),&item->count);
    sj_get_integer_value(sj_object_get_value(json,"skill"),&item->skill);
    
    return item;
}

SJson *inventory_item_to_json(InventoryItem *item)
{
    SJson *json;
    if (!item)return NULL;
    json = sj_object_new();
    if (!json)return NULL;
    
    sj_object_insert(json,"name",sj_new_str(item->name));
    sj_object_insert(json,"count",sj_new_int(item->count));
    sj_object_insert(json,"skill",sj_new_int(item->skill));
    
    return json;
}

/*eol@eof*/
