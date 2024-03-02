#include "simple_logger.h"

#include "items.h"
#include "inventory.h"

void inventory_init(Inventory *inventory)
{
    if (!inventory)return;
    inventory->itemsList = gfc_list_new();
}

Item *inventory_get_item_by_name(Inventory *inventory,const char *name)
{
    Item *item;
    int i,c;
    if (!inventory)return NULL;
    c = gfc_list_get_count(inventory->itemsList);
    for (i = 0; i < c; i++)
    {
        item = gfc_list_get_nth(inventory->itemsList,i);
        if (!item)continue;
        if (gfc_strlcmp(item->name,name) == 0)
        {
            return item;
        }
    }
    return NULL;
}

void inventory_cleanup(Inventory *inventory)
{
    if (!inventory)return;
    gfc_list_foreach(inventory->itemsList,(gfc_work_func*)item_free);
    gfc_list_delete(inventory->itemsList);
    inventory->itemsList = NULL;
}

void inventory_add_item(Inventory *inventory,const char *name)
{
    Item *item;
    if ((!inventory)||(!name))return;
    item = inventory_get_item_by_name(inventory,name);
    if (item)
    {
        item->count++;
        return;
    }
    
    item = item_new(name);
    if (!item)return;
    gfc_list_append(inventory->itemsList,item);
}


/*eol@eof*/
