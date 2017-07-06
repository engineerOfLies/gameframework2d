#include <stdlib.h>
#include <stdio.h>
#include "gf2d_element_label.h"
#include "simple_logger.h"

void gf2d_element_label_draw(Element *element,Vector2D offset)
{
    LabelElement *label;
    Vector2D position;
    if (!element)return;
    label = (LabelElement*)element->data;
    if (!label)return;
    vector2d_add(position,offset,element->bounds);
    //TODO: limit based on size of bounds
    gf2d_text_draw_line(label->text,label->style,element->color, position);
}

List *gf2d_element_label_update(Element *element,Vector2D offset)
{
    return NULL;
}

void gf2d_element_label_free(Element *element)
{
    LabelElement *label;
    if (!element)return;
    label = (LabelElement*)element->data;
    if (label != NULL)
    {
        free(label);
    }
}

LabelElement *gf2d_element_label_new()
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


LabelElement *gf2d_element_label_new_full(char *text,Color color,int style,int justify)
{
    LabelElement *label;
    label = gf2d_element_label_new();
    if (!label)
    {
        return NULL;
    }
    gf2d_block_cpy(label->text,text);
    label->bgcolor = color;
    label->style = style;
    label->justify = justify;
    return label;
}

void gf2d_element_make_label(Element *e,LabelElement *label)
{
    if (!e)return;
    e->data = label;
    e->draw = gf2d_element_label_draw;
    e->update = gf2d_element_label_update;
    e->free_data = gf2d_element_label_free;
}

void gf2d_element_load_label_from_config(Element *e,SJson *json)
{
    SJson *value;
    Vector4D vector;
    Color color;
    const char *buffer;
    int style = FT_Normal;
    int justify = LJ_Left;  
    if ((!e) || (!json))
    {
        slog("call missing parameters");
        return;
    }
    value = sj_object_get_value(json,"style");
    buffer = sj_get_string_value(value);
    if (buffer)
    {
        if (strcmp(buffer,"normal") == 0)
        {
            style = FT_Normal;
        }
        else if (strcmp(buffer,"small") == 0)
        {
            style = FT_Small;
        }
        else if (strcmp(buffer,"H1") == 0)
        {
            style = FT_H1;
        }
        else if (strcmp(buffer,"H2") == 0)
        {
            style = FT_H2;
        }
        else if (strcmp(buffer,"H3") == 0)
        {
            style = FT_H3;
        }
        else if (strcmp(buffer,"H4") == 0)
        {
            style = FT_H4;
        }
        else if (strcmp(buffer,"H5") == 0)
        {
            style = FT_H5;
        }
        else if (strcmp(buffer,"H6") == 0)
        {
        style = FT_H6;
        }
    }

    value = sj_object_get_value(json,"justify");
    buffer = sj_get_string_value(value);
    if (buffer)
    {
        if (strcmp(buffer,"left") == 0)
        {
            style = LJ_Left;
        }
        else if (strcmp(buffer,"center") == 0)
        {
            style = LJ_Center;
        }
        else if (strcmp(buffer,"right") == 0)
        {
            style = LJ_Right;
        }
    }
    value = sj_object_get_value(json,"color");
    vector4d_set(vector,255,255,255,255);
    sj_value_as_vector4d(value,&vector);
    color = gf2d_color_from_vector4(vector);

    value = sj_object_get_value(json,"text");
    buffer = sj_get_string_value(value);
    gf2d_element_make_label(e,gf2d_element_label_new_full((char *)buffer,color,style,justify));
}

/*eol@eof*/
