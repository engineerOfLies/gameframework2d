#ifndef __GF2D_ELEMENT_LIST_H__
#define __GF2D_ELEMENT_LIST_H__

#include "gf2d_list.h"
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


ListElement *gf2d_element_list_new_full(Vector2D itemSize,ListStyle ls,int wraps,int scrolls);


void gf2d_element_list_add_item(Element *e,Element *item);

/**
 * @brief set an element to be the list provided
 * @param e the element to set
 * @param list the list to set it too
 */
void gf2d_element_make_list(Element *e,ListElement *list);

#endif
