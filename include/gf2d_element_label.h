#ifndef __GF2D_LABEL_ELEMENT_H__
#define __GF2D_LABEL_ELEMENT_H__

#include "gf2d_elements.h"


typedef struct
{
    TextBlock text;     /**<the label text*/
    Color bgcolor;      /**<background color for the text*/
    int wraps;          /**<if the text should word wrap to match the dimensional width of its element space*/
    int style;          /**<which font style to use*/
}LabelElement;

/**
 * @brief make a new label element
 * @param text the label text
 * @param color the label text color
 * @param style the font style
 * @param wraps if the label should word wrap
 * @return NULL on error or a new label element
 */
LabelElement *gf2d_element_label_new_full(char *text,Color color,int style,int wraps);

/**
 * @brief set an element to be the label provided
 * @param e the element to set
 * @param label the label to set it too
 */
void gf2d_element_make_label(Element *e,LabelElement *label);

/**
 * @brief load label configuration for a label element from config
 * @param e the element to configure
 * @param json the json config to use
 */
void gf2d_element_load_label_from_config(Element *e,SJson *json);

/**
 * @brief set the label text
 * @param e the element to set the text to
 * @param text the text
 */
void gf2d_element_label_set_text(Element *e,char *text);

/**
 * @brief get the label text
 * @param e the element to get the text from
 * @return a pointer to the label text
 */
const char *gf2d_element_label_get_text(Element *e);
#endif
