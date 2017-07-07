#ifndef __GF2D_LABEL_ELEMENT_H__
#define __GF2D_LABEL_ELEMENT_H__

#include "gf2d_elements.h"

typedef enum
{
    LJ_Left,
    LJ_Center,
    LJ_Right
}LabelJustification;

typedef enum
{
    LA_Top,
    LA_Middle,
    LA_Bottom
}LabelAlignment;

typedef struct
{
    TextBlock text;     /**<the label text*/
    Color bgcolor;      /**<background color for the text*/
    int style;          /**<which font style to use*/
    int justify;        /**<leaning left or right*/
    int alignment;      /**<vertical alignment top, middle or bottom*/
}LabelElement;

/**
 * @brief make a new label element
 * @param text the label text
 * @param color the label text color
 * @param style the font style
 * @param justify the LabelJustification
 * @param align the vertical alignment
 * @return NULL on error or a new label element
 */
LabelElement *gf2d_element_label_new_full(char *text,Color color,int style,int justify,int align);

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

#endif
