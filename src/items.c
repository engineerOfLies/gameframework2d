#include "simple_logger.h"

#include "items.h"

static SJson *_itemJson = NULL;
static SJson *_itemDefs = NULL;

void items_close();

void items_initialize(const char *filename)
{
    if (!filename)
    {
        slog("no filename provided for item initialization.");
        return;
    }
    _itemJson = sj_load(filename);
    if (!_itemJson)
    {
        slog("failed to load the json for item definition");
        return;
    }
    _itemDefs = sj_object_get_value(_itemJson,"items");
    if (!_itemDefs)
    {
        slog("item definition file %s does not contain items list",filename);
        sj_free(_itemJson);
        _itemJson = NULL;
       return;
    }
    atexit(items_close);
}

void items_close()
{
    if (_itemJson)
    {
Item *item_new(const char *name);

void item_free(Item *item);

        sj_free(_itemJson);
    }
    _itemJson = NULL;
    _itemDefs = NULL;
}


SJson *items_get_def_by_name(const char *name)
{
    int i,c;
    SJson *item;
    const char *itemName = NULL;
    if (!name)return NULL;
    if (!_itemDefs)
    {
        slog("no item definitions loaded");
        return NULL;
    }
    c = sj_array_get_count(_itemDefs);
    for (i = 0; i < c; i++)
    {
        item = sj_array_get_nth(_itemDefs,i);
        if (!item)continue;
        itemName = sj_object_get_value_as_string(item,"name");
        if (!itemName)continue;
        if (gfc_strlcmp(name,itemName) == 0)
        {
            //found it
            return item;
        }
    }
    slog("no item found by name %s",name);
    return NULL;
}


Item *item_new(const char *name)
{
    Item *item;
    SJson *itemDef;
    
    itemDef = items_get_def_by_name(name);
    if (!itemDef)return NULL;
    item = gfc_allocate_array(sizeof(Item),1);
    if (!item)return NULL;
    gfc_line_cpy(item->name,name);
    sj_object_get_value_as_int(itemDef,"price",&item->price);
    //get other default values here
    //load the sprite here
    item->count = 1;
    return item;
}

void item_free(Item *item)
{
    if (!item)return;
    gf2d_sprite_free(item->sprite);
    free(item);
}

/*eol@eof*/
