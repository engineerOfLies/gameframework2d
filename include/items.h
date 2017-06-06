#ifndef __ITEM_H__
#define __ITEM_H__

#include "gf2d_entity.h"

/**
 * @purpose the item information is what is loaded from disk to describe how the item entity
 * is loaded
 */
typedef struct
{
    TextLine name;      /**<name of the item*/
    TextLine sprite;    /**<sprite file to use*/
    TextLine actor;     /**<actor file to use*/
    int count;          /**<how much of the something you get*/
    float radius;       /**<for pickup collision*/
}Item;

/**
 * @purpose the InventoryItem is meant to be used as a player inventory.
 * A list of these will keep track of how much you have in your inventory
 */
typedef struct
{
    TextLine name;      /**<name of the item*/
    int count;          /**<how much of the something you get*/
}InventoryItem;

/**
 * @brief initializes the item manager
 * @note does not load any items
 */
void item_manager_init();

/**
 * @brief load the list of items from file into memory
 * @param filename the file to load the items list from
 */
void item_load_list(char *filename);

/**
 * @brief search loaded items list for an item by name
 * @param name the name to search by
 * @return NULL on error or not found or the item information
 */
Item *item_get_by_name(char *name);

/**
 * @brief spawn an item at the specified position
 * @param name the name of the item to spawn
 * @param position where in the game world to spawn it
 * @return a pointer to the item spawned or NULL on error
 */
Entity *item_spawn(char *name,Vector2D position);

#endif
