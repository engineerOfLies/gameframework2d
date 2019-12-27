#ifndef __GF2D_MENUS_H__
#define __GF2D_MENUS_H__

#include <simple_json.h>
#include "gfc_list.h"

#include "gf2d_shape.h"
#include "gf2d_sprite.h"

typedef struct Element_S Element;

typedef enum
{
    BE_TL,
    BE_Top,
    BE_TR,
    BE_Left,
    BE_Right,
    BE_BL,
    BE_Bottom,
    BE_BR
}BorderElements;

typedef struct Window_S
{
    int _inuse;             /**<do not touch*/
    int no_draw_generic;    /**<if true, do not use the generic window draw style*/
    List *elements;         /**<all the components of the window*/
    Sprite *background;     /**<background image*/
    Sprite *border;         /**<border sprites*/
    Rect dimensions;        /**<where on the screen*/
    Rect canvas;            /**<Where within the window we draw things*/
    Vector4D color;         /**<color to draw the window with*/
    int blocks_input;       /**<if true, windows below will not be checked for input updates, but will update*/
    int (*update)(struct Window_S *win,List *updateList);
    int (*draw)(struct Window_S *win);
    int (*free_data)(struct Window_S *win);
    void *data;             /**<custom data*/
}Window;

/**
 * @brief initialize the window system
 * @param max_windows the limit of active windows
 */
void gf2d_windows_init(int max_windows);

/**
 * @brief draw all active windows
 */
void gf2d_windows_draw_all();

/**
 * @brief update all active windows
 */
void gf2d_windows_update_all();

/**
 * @brief get a new initialized window
 * @return NULL on error or a window pointer
 */
Window *gf2d_window_new();

/**
 * @brief load a window config from file and return a handle to it
 * @param the filename of the config file
 * @return NULL on error or a window pointer on success
 */
Window *gf2d_window_load(char *filename);

/**
 * @brief free a window no longer in use
 * @param win the window to free
 */
void gf2d_window_free(Window *win);

/**
 * @brief add a gui element to the window
 * @param win the window to add an element to
 * @param w the element to add
 */
void gf2d_window_add_element(Window *win,Element *e);

/**
 * @brief update a window and all of its elements
 * @param win the window to update
 */
void gf2d_window_update(Window *win);

/**
 * @brief draw a window to the screen.  
 * @note: This is done automatically for windows without a custom draw function or if that function returns 0
 * @param win the window to draw
 */
void gf2d_window_draw(Window *win);

/**
 * @brief draw a window given the border sprites and background image
 * @param border the sprite containing the border elements
 * @param bg the sprite to use for the background image (it will be stretch to match the rect
 * @param rect the dimensions of the window to draw
 * @param color the color to draw the window with
 */
void gf2d_draw_window_border(Sprite *border,Sprite *bg,Rect rect,Vector4D color);

/**
 * @brief draw a generic window using the common border assets
 * @param rect the dimensions of the window to draw
 * @param color the color to draw the window with
 */
void gf2d_draw_window_border_generic(Rect rect,Vector4D color);

/**
 * @brief get the element from the window with the matching id
 * @param win the window to query
 * @param id the index to search for
 * @returns NULL on error or not found, a pointo to the element otherwise
 */
Element *gf2d_window_get_element_by_id(Window *win,int id);

#endif
