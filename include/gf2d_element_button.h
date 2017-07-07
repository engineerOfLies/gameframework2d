#ifndef __GF2D_ELEMENT_BUTTON_H__
#define __GF2D_ELEMENT_BUTTON_H__

#include "gf2d_elements.h"

typedef enum
{
    BE_Hidden = 0,
    BE_Text = 1,
    BE_Actor = 2,
    BE_Both = 3
}BE_Style;

typedef struct
{
    Element *label;
    Element *actor;
    BE_Style style;
    // base color comes from element
    Color highColor;    /**<color used when button is in highlight*/
    Color pressColor;   /**<color used while pressed*/
}ButtonElement;

/**
 * @brief sets the element to be the button
 * @note the button is now owned by the element, and should not be freed
 * @param e the element to make into a button
 * @param button the button to use
 */
void gf2d_element_make_button(Element *e,ButtonElement *button);

/**
 * @brief allocate a new button and set the parameters
 * @param label (optional) use this element for the text display for the button
 * @param actor (optional) use this element for the image display for the button
 * @param highColor this color will be used when drawing the button when it has highlight
 * @param pressColor this color will be used when drawing the button when it is pressed
 * @return NULL on error or a newly create button element
 */
ButtonElement *gf2d_element_button_new_full(Element *label,Element *actor,Color highColor,Color pressColor);

/**
 * @brief load button configuration for a button element from config
 * @param e the element to configure
 * @param json the json config to use
 */
void gf2d_element_load_button_from_config(Element *e,SJson *json);

#endif
