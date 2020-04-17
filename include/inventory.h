#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include "simple_json.h"
#include "gfc_list.h"
#include "gfc_text.h"

typedef struct
{
    TextLine    name;
    int         count;
    int         skill;
}InventoryItem;

typedef struct
{
    List *items;
}Inventory;


/**
 * @brief get count of item in the inventory
 */
int inventory_get_item_count(Inventory *inven,char *name);
int inventory_get_item_skill(Inventory *inven,char *name);
InventoryItem *inventory_get_item(Inventory *inven,char *name);

/**
 * @brief get the number of items in the inventory
 * @param inven the inventory to get the count for
 * @return 0 on empty or error, the count otherwise
 */
int inventory_get_count(Inventory *inven);

/**
 * @brief get an inventory item from the list
 * @param inven the inventory to get an item from
 * @param i the index of the item to get
 * @return NULL on not found, or error, a pointer to the inventortItem otherwise
 */
InventoryItem *inventory_get_nth(Inventory *inven,int i);

/**
 * @brief add an item to an inventory
 * @param inven the inventory to add to
 * @param name the item to add
 * @param count how much the player gets 
 * @param skill skill level of the item for the player
 * @return 0 on failure or the inventory is full of the item in question, 1 on success
 */
int inventory_give_item(Inventory *inven,char *name, int count, int skill);

/**
 * @brief parse inventory information from a json object
 * @param json the json to parse
 * @return NULL on error or the inventory otherwise
 */
Inventory *inventory_parse_from_json(SJson *json);

/**
 * @brief convert inventory data to json
 * @param inven the inventory to convert
 * @return NULL on error, or configured json object that must be freed
 */
SJson *inventory_to_json(Inventory *inven);

/**
 * @brief make a new inventory
 * @return NULL on error or an empty inventory
 */
Inventory *inventory_new();

/**
 * @brief free an inventory
 * @param inven the inventory to free
 */
void inventory_free(Inventory *inven);

#endif
