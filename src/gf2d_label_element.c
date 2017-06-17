#include <stdlib.h>
#include <stdio.h>
#include "gf2d_label_element.h"
#include "simple_logger.h"

void label_draw(Element *element,Vector2D offset)
{
    LabelElement *label;
    Vector2D position;
    if (!element)return;
    label = (LabelElement*)element->data;
    if (!label)return;
    vector2d_add(position,offset,element->bounds);
    //TODO: limit based on size of bounds
    gf2d_text_draw_line(label->text,label->style,label->color, position);
}

int  label_update(Element *element,Vector2D offset)
{
    return 0;
}

void label_free(Element *element)
{
    LabelElement *label;
    if (!element)return;
    label = (LabelElement*)element->data;
    if (label != NULL)
    {
        free(label);
    }
}

LabelElement *label_new()
{
    LabelElement *label;
    label = (LabelElement *)malloc(sizeof(LabelElement));
    if (!label)
    {
        slog("failed to allocate memory for label");
        return NULL;
    }
    memset(label,0,sizeof(LabelElement));
    return label;
}


LabelElement *gf2d_label_element_new(char *text,Color color,int style,int justify)
{
    LabelElement *label;
    label = label_new();
    if (!label)
    {
        return NULL;
    }
    gf2d_block_cpy(label->text,text);
    label->color = color;
    label->style = style;
    label->justify = justify;
    return label;
}

void gf2d_element_make_label(Element *e,LabelElement *label)
{
    if (!e)return;
    e->data = label;
    e->draw = label_draw;
    e->update = label_update;
    e->free_data = label_free;
}
/*eol@eof*/
