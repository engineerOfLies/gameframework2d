#include <stdlib.h>
#include <string.h>

#include "simple_logger.h"

#include "gf2d_graphics.h"

#include "gf2d_elements.h"
#include "gf2d_element_actor.h"
#include "gf2d_element_button.h"
#include "gf2d_element_entry.h"
#include "gf2d_element_list.h"
#include "gf2d_element_label.h"


Element *gf2d_element_new()
{
    Element *e;
    e = (Element *)malloc(sizeof(Element));
    if (!e)
    {
        slog("failed to allocate a new window element");
        return NULL;
    }
    memset(e,0,sizeof(Element));
    return e;
}

Element *gf2d_element_new_full(
    Element *parent,
    int      index,
    TextLine name,
    Rect bounds,
    Color color,
    int state,
    Color backgroundColor,
    int backgroundDraw,
    int justify,
    int align
)
{
    Element *e;
    e = gf2d_element_new();
    if (!e)return NULL;
    gfc_line_cpy(e->name,name);
    e->index = index;
    e->color = color;
    e->state = state;
    e->bounds = bounds;
    e->backgroundColor = backgroundColor;
    e->backgroundDraw = backgroundDraw;
    e->justify = justify;
    e->alignment = align;
    return e;
}

void gf2d_element_free(Element *e)
{
    if (!e)return;
    if (e->free_data)
    {
        e->free_data(e);
    }
    free(e);
}

void gf2d_element_draw(Element *e, Vector2D offset)
{
    Rect rect;
    if ((!e)||(e->state == ES_hidden))
    {
        return;
    }
    gf2d_rect_set(rect,offset.x + e->bounds.x,offset.y + e->bounds.y,e->bounds.w,e->bounds.h);
    if (e->backgroundDraw)
    {
        gf2d_rect_draw_filled(rect,e->backgroundColor);
    }
    if (e->draw)e->draw(e,offset);
    if (gf2d_graphics_debug_mode())
    {
        gf2d_rect_draw(rect,gfc_color8(100,255,100,255));
    }
}

List * gf2d_element_update(Element *e, Vector2D offset)
{
    if (!e)
    {
        return NULL;
    }
    if (e->draw)return e->update(e,offset);
    return NULL;
}

void gf2d_element_calibrate(Element *e,Element *parent, Window *win)
{
    Rect res;
    int negx = 0,negy = 0;
    if (!e)return;
    if (parent != NULL)
    {
        gf2d_rect_copy(res,parent->bounds);
    }
    else if (win != NULL)
    {
        gf2d_rect_copy(res,win->dimensions);
    }
    else
    {
        slog("error: need a parent element or a window");
        return;
    }
    if (e->bounds.x < 0)
    {
        negx = 1;
        e->bounds.x *= -1;
    }
    if (e->bounds.y < 0)
    {
        negy = 1;
        e->bounds.y *= -1;
    }
    if ((e->bounds.x > 0)&&(e->bounds.x < 1.0))
    {
        e->bounds.x *= res.w;
    }
    if ((e->bounds.y > 0)&&(e->bounds.y < 1.0))
    {
        e->bounds.y *= res.h;
    }
    if ((e->bounds.w > 0)&&(e->bounds.w <= 1.0))
    {
        e->bounds.w *= res.w;
    }
    if ((e->bounds.h > 0)&&(e->bounds.h <= 1.0))
    {
        e->bounds.h *= res.h;
    }
    
    if (negx)
    {
        e->bounds.x = res.w - e->bounds.x;
    }
    if (negy)
    {
        e->bounds.y = res.h - e->bounds.y;
    }
}

void gf2d_element_set_color(Element *element, Color color)
{
    if (!element)return;
    gfc_color_copy(element->color,color);
}

Element *gf2d_element_load_from_config(SJson *json,Element *parent,Window *win)
{
    Element *e = NULL;
    SJson *value;
    const char *buffer;
    int justify = LJ_Left;  
    int align = LA_Top;
    const char *type;
    Vector4D vector;
    if (!sj_is_object(json))return NULL;
    e = gf2d_element_new();
    if (!e)return NULL;
    value = sj_object_get_value(json,"name");
    if (value)
    {
        gfc_line_cpy(e->name,sj_get_string_value(value));
    }
    
    value = sj_object_get_value(json,"id");
    sj_get_integer_value(value,&e->index);

    value = sj_object_get_value(json,"state");
    sj_get_integer_value(value,&e->index);

    value = sj_object_get_value(json,"color");
    vector4d_set(vector,255,255,255,255);
    sj_value_as_vector4d(value,&vector);
    e->color = gfc_color_from_vector4(vector);

    value = sj_object_get_value(json,"backgroundColor");
    vector4d_set(vector,255,255,255,0);
    sj_value_as_vector4d(value,&vector);
    e->color = gfc_color_from_vector4(vector);

    value = sj_object_get_value(json,"backgroundDraw");
    sj_get_integer_value(value,&e->backgroundDraw);
    

    value = sj_object_get_value(json,"bounds");
    sj_value_as_vector4d(value,&vector);
    gf2d_rect_set(e->bounds,vector.x,vector.y,vector.z,vector.w);
    gf2d_element_calibrate(e,parent, win);
    
    value = sj_object_get_value(json,"justify");
    buffer = sj_get_string_value(value);
    if (buffer)
    {
        if (strcmp(buffer,"left") == 0)
        {
            justify = LJ_Left;
        }
        else if (strcmp(buffer,"center") == 0)
        {
            justify = LJ_Center;
        }
        else if (strcmp(buffer,"right") == 0)
        {
            justify = LJ_Right;
        }
    }

    value = sj_object_get_value(json,"align");
    buffer = sj_get_string_value(value);
    if (buffer)
    {
        if (strcmp(buffer,"top") == 0)
        {
            align = LA_Top;
        }
        else if (strcmp(buffer,"middle") == 0)
        {
            align = LA_Middle;
        }
        else if (strcmp(buffer,"bottom") == 0)
        {
            align = LA_Bottom;
        }
    }
    e->justify = justify;
    e->alignment = align;

    
    value = sj_object_get_value(json,"type");
    if (value)
    {
        type = sj_get_string_value(value);
        if (strcmp(type,"list") == 0)
        {
            gf2d_element_load_list_from_config(e,json,win);
        }
        else if (strcmp(type,"label") == 0)
        {
            gf2d_element_load_label_from_config(e,json);
        }
        else if (strcmp(type,"actor") == 0)
        {
            gf2d_element_load_actor_from_config(e,json);
        }
        else if (strcmp(type,"button") == 0)
        {
            gf2d_element_load_button_from_config(e,json,win);
        }
        else if (strcmp(type,"entry") == 0)
        {
            gf2d_element_load_entry_from_config(e,json,win);
        }
    }
    else
    {
        slog("element definition missing type!");
    }
    return e;
}

Rect gf2d_element_get_absolute_bounds(Element *element,Vector2D offset)
{
    Rect r = {0};
    if (!element)return r;
    r.x = element->bounds.x + offset.x;
    r.y = element->bounds.y + offset.y;
    r.w = element->bounds.w;
    r.h = element->bounds.h;
    return r;
}

Element *gf2d_element_get_by_id(Element *e,int id)
{
    if (!e)return NULL;
    if (e->index == id)return e;
    switch(e->type)
    {
        case ET_List:
            return gf2d_element_list_get_item_by_id(e,id);
            break;
        case ET_Button:
            return gf2d_element_button_get_by_id(e,id);
            break;
        case ET_Entry:
        default:
            return NULL;
    }
}

Vector2D gf2d_element_get_alignment(Element *element,Vector2D size)
{
    Vector2D position = {0};
    if (!element)return position;
    
    switch(element->justify)
    {
        case LJ_Left:
            break;
        case LJ_Center:
            position.x += (element->bounds.w - size.x)/2 ;
            break;
        case LJ_Right:
            position.x += (element->bounds.w - size.x);
            break;
    }
    switch(element->alignment)
    {
        case LA_Top:
            break;
        case LA_Middle:
            position.y += (element->bounds.h - size.y)/2 ;
            break;
        case LA_Bottom:
            position.y += (element->bounds.h - size.y);
            break;
    }
    return position;
}

Element *gf2d_get_element_by_name(Element *e,char *name)
{
    if (!e)return NULL;
    if (gfc_line_cmp(e->name,name)==0)return e;
    if (e->get_by_name)return e->get_by_name(e,name);
    return NULL;
}
/*eol@eof*/
