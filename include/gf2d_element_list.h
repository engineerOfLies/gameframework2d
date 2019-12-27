#ifndef __GF2D_ELEMENT_LIST_H__
#define __GF2D_ELEMENT_LIST_H__

#include "gfc_list.h"
#include "gf2d_elements.h"

typedef enum
{
    LS_Horizontal,
    LS_Vertical
}ListStyle;

typedef struct
{
    List *list;
    Vector2D itemSize;
    ListStyle listStyle;
    int   wraps;
    int   scrolls;
}ListElement;


ListElement *gf2d_element_list_new_full(Rect bounds,Vector2D itemSize,ListStyle ls,int wraps,int scrolls);

void gf2d_element_list_add_item(Element *e,Element *item);

/**
 * @brief set an element to be the list provided
 * @param e the element to set
 * @param list the list to set it too
 */
void gf2d_element_make_list(Element *e,ListElement *list);

/**
 * @brief load list configuration for a list element from config
 * @param e the element to configure
 * @param json the json config to use
 */
void gf2d_element_load_list_from_config(Element *e,SJson *json,Window *win);

/**
 * @brief seach an element list for an item with id provided
 * @param e the list element to search
 * @param id the index to search for
 * @returns NULL on not found or error, the element with index that matches otherwise
 */
Element *gf2d_element_list_get_item_by_id(Element *e,int id);

#endif
