#include <stdlib.h>
#include <stdio.h>
#include "gf2d_element_list.h"
#include "simple_logger.h"

Vector2D gf2d_element_get_item_position(Element *element,int i)
{
    ListElement* list;
    Vector2D position = {0};
    int itemsPerLine;
    if (!element)return position;
    list = (ListElement*)element->data;
    if (!list)return position;
    if ((list->listStyle == LS_Horizontal) && (list->wraps))
    {
        itemsPerLine = element->bounds.w / list->itemSize.x;
        position.x = element->bounds.x + ((i % itemsPerLine) * list->itemSize.x);
        position.y = element->bounds.y + ((i / itemsPerLine) * list->itemSize.y);
        return position;
    }
    if ((list->listStyle == LS_Vertical) && (list->wraps))
    {
        itemsPerLine = element->bounds.h / list->itemSize.y;
        position.x = element->bounds.x + ((i / itemsPerLine) * list->itemSize.x);
        position.y = element->bounds.y + ((i % itemsPerLine) * list->itemSize.y);
        return position;
    }
    if (list->listStyle == LS_Horizontal)
    {
        position.x = element->bounds.x + (i * list->itemSize.x);
        position.y = element->bounds.y;
        return position;
    }
    if (list->listStyle == LS_Vertical)
    {
        position.x = element->bounds.x;
        position.y = element->bounds.y + (i * list->itemSize.y);
        return position;
    }
    return position;
}

void gf2d_element_list_draw(Element *element,Vector2D offset)
{
    ListElement *list;
    Vector2D position;
    int count,i;
    Element *e;
    if (!element)return;
    list = (ListElement*)element->data;
    if (!list)return;
    count = gf2d_list_get_count(list->list);
    for (i = 0; i < count; i++)
    {
        e = (Element *)gf2d_list_get_nth(list->list,i);
        if (!e)continue;
        position = gf2d_element_get_item_position(element,i);
        vector2d_add(position,position,offset);
        gf2d_element_draw(e, position);
    }
}

List *gf2d_element_list_update(Element *element,Vector2D offset)
{
    ListElement *list;
    Vector2D position;
    int count,i;
    Element *e;
    int retval = 0;
    if (!element)return NULL;
    list = (ListElement*)element->data;
    if (!list)return NULL;
    vector2d_add(position,offset,element->bounds);
    count = gf2d_list_get_count(list->list);
    for (i = 0; i < count; i++)
    {
        e = (Element *)gf2d_list_get_nth(list->list,i);
        if (!e)continue;
        position = gf2d_element_get_item_position(element,i);
        retval = retval || gf2d_element_update(e, position);
    }
    return NULL;//TODO make this return a list
}

void gf2d_element_list_free(Element *element)
{
    int count,i;
    Element *e;
    ListElement *list;
    if (!element)return;
    list = (ListElement*)element->data;
    if (list != NULL)
    {
        /*for each item, free it*/
        count = gf2d_list_get_count(list->list);
        for (i = 0; i < count; i++)
        {
            e = (Element *)gf2d_list_get_nth(list->list,i);
            if (!e)continue;
            gf2d_element_free(e);
        }
        gf2d_list_delete(list->list);
        free(list);
    }
}

ListElement *gf2d_element_list_new()
{
    ListElement *list;
    list = (ListElement *)malloc(sizeof(ListElement));
    if (!list)
    {
        slog("failed to allocate memory for list");
        return NULL;
    }
    memset(list,0,sizeof(ListElement));
    list->list = gf2d_list_new();
    return list;
}


ListElement *gf2d_element_list_new_full(Vector2D itemSize,ListStyle ls,int wraps,int scrolls)
{
    ListElement *list;
    list = gf2d_element_list_new();
    if (!list)
    {
        return NULL;
    }
    vector2d_copy(list->itemSize,itemSize);
    list->listStyle = ls;
    list->wraps = wraps;
    list->scrolls = scrolls;
    return list;
}

void gf2d_element_make_list(Element *e,ListElement *list)
{
    if ((!e)||(!list))return;// no op
    e->data = list;
    e->draw = gf2d_element_list_draw;
    e->update = gf2d_element_list_update;
    e->free_data = gf2d_element_list_free;
}

void gf2d_element_list_remove_item(Element *e,Element *item)
{
    ListElement *list;
    if ((!e)||(!item))return;// no op
    list = (ListElement *)e->data;
    gf2d_list_delete_data(list->list,(void*)item);
}

void gf2d_element_list_add_item(Element *e,Element *item)
{
    ListElement *list;
    if ((!e)||(!item))return;// no op
    list = (ListElement *)e->data;
    list->list = gf2d_list_append(list->list,(void*)item);
}
/*eol@eof*/
