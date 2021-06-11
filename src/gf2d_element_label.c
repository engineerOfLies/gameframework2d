#include <stdlib.h>
#include <stdio.h>
#include "simple_logger.h"
#include "gfc_text.h"
#include "gf2d_font.h"
#include "gf2d_element_label.h"

void gf2d_element_label_draw(Element *element,Vector2D offset)
{
    LabelElement *label;
    Vector2D position;
    Vector2D justifyOffset;
    Vector2D size = {0};
    Rect r;
    if (!element)return;
    label = (LabelElement*)element->data;
    if (!label)return;
    if (strlen(label->text) <= 0)return;
    if (label->wraps)
    {
        r = gf2d_font_get_text_wrap_bounds_tag(
            label->text,
            label->style,
            element->bounds.w,
            element->bounds.h);
        size.x = r.w;
        size.y = r.h;
    }
    else
    {
        size = gf2d_font_get_bounds_tag(label->text,label->style);
    }
    if (size.x < 0)
    {
        return;
    }
    // adjust position to top left
    vector2d_add(position,offset,element->bounds);
    justifyOffset = gf2d_element_get_alignment(element,size);

    vector2d_add(position,position,justifyOffset);

    if (label->wraps)
    {
        gf2d_font_draw_text_wrap_tag(label->text,label->style,element->color, gf2d_rect(position.x, position.y, element->bounds.w, element->bounds.h));
    }
    else 
    {
        gf2d_font_draw_line_tag(label->text,label->style,element->color, position);
    }
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


LabelElement *gf2d_element_label_new_full(char *text,Color color,int style,int wraps)
{
    LabelElement *label;
    label = gf2d_element_label_new();
    if (!label)
    {
        return NULL;
    }
    gfc_block_cpy(label->text,text);
    label->bgcolor = color;
    label->style = style;
    label->wraps = wraps;
    return label;
}

void gf2d_element_make_label(Element *e,LabelElement *label)
{
    if (!e)return;
    e->data = label;
    e->type = ET_Label;
    e->draw = gf2d_element_label_draw;
    e->update = gf2d_element_label_update;
    e->free_data = gf2d_element_label_free;
}

const char *gf2d_element_label_get_text(Element *e)
{
    if (!e)return NULL;
    if (e->type != ET_Label)return NULL;
    LabelElement *label;
    label = (LabelElement *)e->data;
    if (!label)return NULL;
    return label->text;
}

void gf2d_element_label_set_text(Element *e,char *text)
{
    if (!e)return;
    if (e->type != ET_Label)return;
    LabelElement *label;
    label = (LabelElement *)e->data;
    if (!label)return;
    gfc_block_cpy(label->text,text);
}

void gf2d_element_load_label_from_config(Element *e,SJson *json)
{
    SJson *value;
    Vector4D vector;
    Color color;
    const char *buffer;
    int style = FT_Normal;
    int wraps = 0;
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

    value = sj_object_get_value(json,"wraps");
    if (value)
    {
        sj_get_bool_value(value,(short int *)&wraps);
    }
    value = sj_object_get_value(json,"color");
    vector4d_set(vector,255,255,255,255);
    sj_value_as_vector4d(value,&vector);
    color = gfc_color_from_vector4(vector);

    value = sj_object_get_value(json,"text");
    buffer = sj_get_string_value(value);
    gf2d_element_make_label(e,gf2d_element_label_new_full((char *)buffer,color,style,wraps));
}

/*eol@eof*/
