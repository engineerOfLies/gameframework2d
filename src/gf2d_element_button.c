#include "gf2d_element_button.h"
#include "gf2d_mouse.h"
#include "gf2d_element_actor.h"
#include "simple_logger.h"

void gf2d_element_button_draw(Element *element,Vector2D offset)
{
    ButtonElement *button;
    Vector2D position;
    Actor *actor;
    if (!element)return;
    button = (ButtonElement*)element->data;
    if (!button)return;
    vector2d_add(position,offset,element->bounds);
    actor = gf2d_element_actor_get_actor(button->actor);
    switch(element->state)
    {
        case ES_hidden:
        case ES_disable:
            return;
        case ES_idle:
            gf2d_actor_set_action(actor,"idle");
            break;
        case ES_highlight:
            gf2d_actor_set_action(actor,"high");
            break;
        case ES_active:
            gf2d_actor_set_action(actor,"press");
            break;
    }
    gf2d_element_draw(button->actor,position);
    gf2d_element_draw(button->label,position);
}

List *gf2d_element_button_update(Element *element,Vector2D offset)
{
    Actor *actor;
    Rect bounds;
    List *list;
    ButtonElement *button;
    if (!element)return NULL;
    button = (ButtonElement*)element->data;
    if (!button)return NULL;
    actor = gf2d_element_actor_get_actor(button->actor);
    gf2d_actor_next_frame(actor);
    bounds = gf2d_element_get_absolute_bounds(element,offset);
    if(gf2d_mouse_in_rect(bounds))
    {
        element->state = ES_highlight;
        if (gf2d_mouse_button_state(0))
        {
            element->state = ES_active;
        }
        else if (gf2d_mouse_button_released(0))
        {
            list = gf2d_list_new();
            gf2d_list_append(list,element);
            return list;
        }
    }
    else
    {
        element->state = ES_idle;
    }

    return NULL;
}

void gf2d_element_button_free(Element *element)
{
    ButtonElement *button;
    if (!element)return;
    button = (ButtonElement*)element->data;
    if (button != NULL)
    {
        gf2d_element_free(button->label);
        gf2d_element_free(button->actor);
        free(button);
    }
}

ButtonElement *gf2d_element_button_new()
{
    ButtonElement *button;
    button = (ButtonElement *)malloc(sizeof(ButtonElement));
    if (!button)
    {
        slog("failed to allocate memory for button");
        return NULL;
    }
    memset(button,0,sizeof(ButtonElement));
    return button;
}


void gf2d_element_make_button(Element *e,ButtonElement *button)
{
    if (!e)return;
    e->data = button;
    e->type = ET_Button;
    e->state = ES_idle;
    e->draw = gf2d_element_button_draw;
    e->update = gf2d_element_button_update;
    e->free_data = gf2d_element_button_free;
}

ButtonElement *gf2d_element_button_new_full(Element *label,Element *actor,Color highColor,Color pressColor)
{
    ButtonElement *button;
    button = gf2d_element_button_new();
    if (!button)return NULL;
    button->label = label;
    button->actor = actor;
    button->highColor = highColor;
    button->pressColor = pressColor;
    return button;
}

void gf2d_element_load_button_from_config(Element *e,SJson *json)
{
    Vector4D highColor = {255,255,255,255},pressColor = {255,255,255,255};
    Element *label = NULL;
    Element *actor = NULL;
    SJson *value;
    
    if ((!e) || (!json))
    {
        slog("call missing parameters");
        return;
    }
    
    value = sj_object_get_value(json,"highColor");
    if (!sj_value_as_vector4d(value,&highColor))
    {
        slog("highColor not provided");
    }

    value = sj_object_get_value(json,"pressColor");
    if (!sj_value_as_vector4d(value,&pressColor))
    {
        slog("pressColor not provided");
    }


    value = sj_object_get_value(json,"label");
    if (value)
    {
        label = gf2d_element_load_from_config(value);
    }
    value = sj_object_get_value(json,"actor");
    if (value)
    {
        actor = gf2d_element_load_from_config(value);
    }
    gf2d_element_make_button(e,gf2d_element_button_new_full(label,actor,gf2d_color_from_vector4(highColor),gf2d_color_from_vector4(pressColor)));
}


/*eol@eof*/
