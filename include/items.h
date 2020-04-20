#ifndef __ITEMS_H__
#define __ITEMS_H__

#include "simple_json.h"
#include "gfc_types.h"
#include "gfc_text.h"

/**
 * @brief items are generic enough to work for items, skills, spells, and attributes
 */

typedef struct
{
    TextLine    name;
    TextLine    actor;
    TextLine    action;
    TextBlock   description;
    int         cost;
    int         healthCost;
    int         manaCost;
    int         staminaCost;
    int         stackable;
    int         stackLimit;
}Item;


/**
 * @brief load a global item list from json file
 * @param filename the file to load
 */
void item_list_load(char *filename);

/**
 * @brief search the item list for an item by name
 * @param name the item to search for
 * @return NULL on not found or error, a pointer to the item data otherwise
 */
Item *item_list_get_by_name(char *name);

/**
 * @brief allocate a new item
 * @return NULL on error, or an initialized item pointer otherwise
 */
Item *item_new();

/**
 * @brief free an item from memory
 * @param item the item to free
 */
void item_free(Item *item);

/**
 * @brief create and configure an Item from a json object
 * @param json the json object
 * @return NULL on error, or a configured Item otherwise
 */
Item *item_parse_from_json(SJson *json);


#endif
