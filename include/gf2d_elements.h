#ifndef __GF2D_ELEMENTS_H__
#define __GF2D_ELEMENTS_H__

/**
 * @purpose gf2d elements are window elements like labels, buttons, status bars, etc
 * This is a generic container class for all other elements
 * There are going to be 
 */

#include "gf2d_sprite.h"
#include "gf2d_list.h"
#include "gf2d_shape.h"
#include "gf2d_text.h"
#include "gf2d_color.h"

typedef enum
{
    ES_idle,        /**<normal*/
    ES_disable,     /**<greyed out*/
    ES_highlight,   /**<selected*/
    ES_hidden,
    ES_active       /**<pressed or whatever*/
}ElementState;

typedef enum
{
    EUR_None,
    EUR_Handled
}ElementUpdateReturn;

typedef struct Element_S
{
    int      index; /**<order of highlight in the menu, -1 for does not receive highlight*/
    TextLine name;  /**<name of the element should be unique per window*/
    
    Rect bounds;    /**<drawing bounds for the element*/
    Color color;    /**<color for the element*/
    
    int state;      /**<if true, drawn with highlight*/
    void (*draw)        (struct Element_S *element,Vector2D offset); /**<draw function, offset comes from draw position of window*/
    List *(*update)     (struct Element_S *element,Vector2D offset); /**<function called for updates  returns alist of all elements updated with input*/
    void (*free_data)   (struct Element_S *element);    /**<free function for the element to clean up any loaded custom data*/
    void *data;     /**<custom element data*/
}Element;

/**
 * @brief allocate and initialize a new element
 * @return NULL on error or a new element otherwise;
 */
Element *gf2d_element_new();

/**
 * @brief allocate and set values for a new element
 * @param index the index of the element
 * @param name the name of the element
 * @param bounds the drawing bounds of the element
 * @param color the draw color of the element
 * @param state the initial state of the element
 * @return NULL on error or a new element otherwise;
 */
Element *gf2d_element_new_full(
    int      index,
    TextLine name,
    Rect bounds,
    Color color,
    int state
);

/**
 * @brief free an element.  Calls the custom free element as needed
 * @param element the element to free
 */
void gf2d_element_free(Element *element);

/**
 * @brief draw an element.  Calls the custom draw function
 * @param element the element to draw
 * @param offset comes from parent window position
 */
void gf2d_element_draw(Element *element, Vector2D offset);

/**
 * @brief update an element.  checks input
 * @param element the element to draw
 * @param offset comes from parent window position
 * @return NULL if there was nothing to report or a pointer to a List of elements that have been updated.  This list needs to be freed
 */
List *gf2d_element_update(Element *element, Vector2D offset);

#endif
