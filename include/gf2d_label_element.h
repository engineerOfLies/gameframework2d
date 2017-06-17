#ifndef __GF2D_LABEL_ELEMENT_H__
#define __GF2D_LABEL_ELEMENT_H__

#include "gf2d_elements.h"

typedef enum
{
    LJ_Left,
    LJ_Center,
    LJ_Right
}LabelJustification;

typedef struct
{
    TextBlock text;     /**<the label text*/
    Color color;        /**<color for the text*/
    int style;          /**<which font style to use*/
    int justify;        /**<leaning left or right*/
}LabelElement;

LabelElement *gf2d_label_element_new(char *text,Color color,int style,int justify);

/**
 * @brief set an element to be the label provided
 * @param e the element to set
 * @param label the label to set it too
 */
void gf2d_element_make_label(Element *e,LabelElement *label);

#endif
