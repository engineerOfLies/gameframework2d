#include "gf2d_list.h"
#include "simple_logger.h"

void gf2d_list_delete(List *list)
{
    if (!list)return;
    if (list->elements)
    {
        free(list->elements);
    }
    free(list);
}

List *gf2d_list_new()
{
    return gf2d_list_new_size(16);
}

List *gf2d_list_new_size(Uint32 count)
{
    List *l;
    if (!count)
    {
        slog("cannot make a list of size zero");
        return NULL;
    }
    l = (List *)malloc(sizeof(List));
    if (!l)
    {
        slog("failed to allocate space for the list");
        return NULL;
    }
    memset(l,0,sizeof(List));
    l->size = count;
    l->elements = (ListElementData*)malloc(sizeof(ListElementData)*count);
    if (!l->elements)
    {
        slog("failed to allocate space for list elements");
        free(l);
        return NULL;
    }
    memset(l->elements,0,sizeof(ListElementData)*count);
    return l;
}

void *gf2d_list_get_nth(List *list,Uint32 n)
{
    if (!list)
    {
        slog("no list provided");
        return NULL;
    }
    if ((n >= list->count)||(n >= list->size))return NULL;
    return list->elements[n].data;
}

List *gf2d_list_expand(List *list)
{
    List *l;
    if (!list)
    {
        slog("no list provided");
        return NULL;
    }
    if (!list->size)list->size = 8;
    l = gf2d_list_new_size(list->size * 2);
    if (!l)
    {
        return list;
    }
    if (list->count > 0)
    {
        memcpy(l->elements,list->elements,sizeof(ListElementData)*list->count);
    }
    l->count = list->count;
    gf2d_list_delete(list);
    return l;
}

List *gf2d_list_append(List *list,void *data)
{
    if (!list)
    {
        slog("no list provided");
        return NULL;
    }
    if (list->count >= list->size)
    {
        list = gf2d_list_expand(list);
        if (!list)
        {
            slog("append failed due to lack of memory");
            return NULL;
        }
    }
    list->elements[list->count].data = data;
    list->count++;
    return list;
}

List *gf2d_list_concat(List *a,List *b)
{
    int i,count;
    void *data;
    if ((!a) || (!b))
    {
        slog("missing list data");
        return NULL;
    }
    count = gf2d_list_get_count(b);
    for (i = 0; i < count;i++)
    {
        data = gf2d_list_get_nth(b,i);
        a = gf2d_list_append(a,data);
        if (a == NULL)return NULL;
    }
    return a;
}

List *gf2d_list_concat_free(List *a,List *b)
{
    a = gf2d_list_concat(a,b);
    if (a == NULL)return NULL;
    gf2d_list_delete(b);
    return a;
}

List *gf2d_list_prepend(List *list,void *data)
{
    return gf2d_list_insert(list,data,0);
}

List *gf2d_list_insert(List *list,void *data,Uint32 n)
{
    if (!list)
    {
        slog("no list provided");
        return NULL;
    }
    if (n > list->size + 1)
    {
        slog("attempting to insert element beyond length of list");
        return list;
    }
    if (list->count >= list->size)
    {
        list = gf2d_list_expand(list);
        if (!list)return NULL;
    }
    memmove(&list->elements[n+1],&list->elements[n],sizeof(ListElementData)*(list->count - n));//copy all elements after n
    list->elements[n].data = data;
    list->count++;
    return list;
}


int gf2d_list_delete_first(List *list)
{
    return gf2d_list_delete_nth(list,0);
}

int gf2d_list_delete_last(List *list)
{
    if (!list)
    {
        slog("no list provided");
        return -1;
    }
    return gf2d_list_delete_nth(list,list->count-1);
}

int gf2d_list_delete_data(List *list,void *data)
{
    int i;
    if (!list)
    {
        slog("no list provided");
        return -1;
    }
    if (!data)return 0;
    for (i = 0; i < list->count;i++)
    {
        if (list->elements[i].data == data)
        {
            // found it, now delete it
            gf2d_list_delete_nth(list,i);
            return 0;
        }
    }
    slog("data not found");
    return -1;
}

int gf2d_list_delete_nth(List *list,Uint32 n)
{
    if (!list)
    {
        slog("no list provided");
        return -1;
    }
    if (n >= list->count)
    {
        slog("attempting to delete beyond the length of the list");
        return -1;
    }
    if (n == (list->count - 1))
    {
        list->count--;// last element in the array, this is easy
        return 0;
    }
    memmove(&list->elements[n],&list->elements[n+1],sizeof(ListElementData)*(list->count - n));//copy all elements after n
    list->count--;
    return 0;
}

Uint32 gf2d_list_get_count(List *list)
{
    if (!list)return 0;
    return list->count;
}

void gf2d_list_foreach(List *list,void (*function)(void *data,void *context),void *contextData)
{
    int i;
    if (!list)
    {
        slog("no list provided");
        return;
    }
    if (!function)
    {
        slog("no function provided");
    }
    for (i = 0;i < list->count;i++)
    {
        function(list->elements[i].data,contextData);
    }
}

/*eol@eof*/
