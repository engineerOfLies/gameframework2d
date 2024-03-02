#ifndef __ITEMS_H__
#define __ITEMS_H__

#include "simple_json.h"

#include "gfc_text.h"

#include "gf2d_sprite.h"

typedef struct
{
    TextLine name;
    Sprite *sprite;
    int count;  //how many of the item I have
    int price;
    
}Item;

/**
 * @brief initialize and load item definitions
 * @param filename the json file containing item data
 */
void items_initialize(const char *filename);

/**
 * @brief get an item definition by its name 
 * @param name the search criteria
 * @return NULL if not found, otherwise the definition information of the item
 * @note DO NOT FREE THAT DATA
 */
SJson *items_get_def_by_name(const char *name);

Item *item_new(const char *name);

void item_free(Item *item);

#endif
