#ifndef __GF2D_LIST_H__
#define __GF2D_LIST_H__

#include <SDL.h>

typedef struct
{
    void *data;
}ListElement;

/**
 * @brief the GF2D List is an automatically expanding general purpose list
 * the list stores data in contiguous memory
 */
typedef struct
{
    ListElement *elements;
    Uint32 size;
    Uint32 count;
}List;

/**
 * @brief allocated a new empty list
 * @return NULL on memory error or a new empty list
 */
List *gf2d_list_new();

/**
 * @brief allocate a new empty list of size 'count'
 * @param count how many elements you wish to support in this list.
 * @return NULL on memory error or a new empty list
 */
List *gf2d_list_new_size(Uint32 count);

/**
 * @brief deletes a list that has been previously allocated
 * @param list the list to delete;
 */
void gf2d_list_delete(List *list);

/**
 * @brief get the data stored at the nth element
 * @param list the list to pull data from
 * @param n which element to look out
 * @return NULL on error (such as if n > the element count) or the address of the data otherwise
 */
void *gf2d_list_get_nth(List *list,Uint32 n);

/**
 * @brief add an element to the end of the list
 * @param list the list to add to
 * @param data the data to assign to the new element
 * @return the address of the list.  In the event that the list needed to expand this will be different from the list provided
 */
List *gf2d_list_append(List *list,void *data);

/**
 * @brief instert a new element at the position provided
 * @param list the list to insert into
 * @param data the data to assin to the new element
 * @return the address of the list.  In the event that the list needed to expand this will be different from the list provided
 */
List *gf2d_list_insert(List *list,void *data,Uint32 n);

/**
 * @brief delete the element at the nth position in the array
 * @param list the list to delete out of
 * @param n the element to delete.  This is no-op if the nth element is beyond the scope of the list (event is logged)
 * @return the list provided
 */
List *gf2d_list_delete_nth(List *list,Uint32 n);

/**
 * @brief delete the first element in the list pointing to the address of data
 * @param list the list to delete the element from
 * @param data used to match against which element to delete
 * @return 0 on complete, error otherwise
 */
int gf2d_list_delete_data(List *list,void *data);

/**
 * @brief get the number of tracked elements in the list
 * @param list the list the check
 * @return the count in the list.  Will be zero if list was NULL
 */
Uint32 gf2d_list_get_count(List *list);

/**
 * @brief iterate over each element in the array and call the function provided
 * @param list the list to iterate over
 * @param function a pointer to a function that will be called.  Data will be set to the list data element, context will be the contextData provided
 * @param contextData the data that will also be provided to the function pointer for each element
 */
void gf2d_list_foreach(List *list,void (*function)(void *data,void *context),void *contextData);

#endif
