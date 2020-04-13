#ifndef __GF2D_MOUSE_H__
#define __GF2D_MOUSE_H__

#include "gfc_vector.h"

#include "gf2d_actor.h"
#include "gf2d_shape.h"


typedef enum
{
    MF_Pointer,
    MF_Walk,
    MF_Look,
    MF_Interact,
    MF_Talk,
    MF_Item,
    MF_Spell,
    MF_Max
}MouseFunction;

/**
 * @brief load a mouse graphic from an actor
 * @param actorFile the file to use to load the mouse information
 * @note it automatically frees previously loaded actor information
 */
void gf2d_mouse_load(char *actorFile);

/**
 * @brief call once per frame to update the internal mouse information
 */
void gf2d_mouse_update();

/**
 * @brief Draw the mouse to the screen.  Should probably be the last thing you do each frame
 */
void gf2d_mouse_draw();

/**
 * @brief check if the mouse has moved since last frame
 * gf2d_mouse_update() must have been called or this just returns 0
 * @return 1 if the mouse has changed state at all
 */
int gf2d_mouse_moved();

/**
 * @brief check if the mouse button is down at the moment
 * @param button the button number to check (left mouse button is 0, right mouse button is 1, etc)
 * @return 1 if it is down, 0 otherwise
 */
int gf2d_mouse_button_state(int button);

/**
 * @brief check if the mouse button was pressed this frame
 * @param button the button number to check (left mouse button is 0, right mouse button is 1, etc)
 * @return 1 if it has, 0 otherwise
 */
int gf2d_mouse_button_pressed(int button);

/**
 * @brief check if the mouse button was held for this frame
 * @param button the button number to check (left mouse button is 0, right mouse button is 1, etc)
 * @return 1 if it has, 0 otherwise
 */
int gf2d_mouse_button_held(int button);

/**
 * @brief check if the mouse button was released this frame
 * @param button the button number to check (left mouse button is 0, right mouse button is 1, etc)
 * @return 1 if it has, 0 otherwise
 */
int gf2d_mouse_button_released(int button);

/**
 * @brief check the state of the mouse button
 * @param button the button number to check (left mouse button is 0, right mouse button is 1, etc)
 * @return 1 if it is down, 0 otherwise
 */
int gf2d_mouse_button_state(int button);

/**
 * @brief get the current mouse position
 * @return a vector containing the mouse position information.  Position will be -1,-1 if never updated
 */
Vector2D gf2d_mouse_get_position();

/**
 * @brief get the change in position from the last frame
 * @return a vector containing the change in mouse position.
 */
Vector2D gf2d_mouse_get_movement();

/**
 * @brief check if the mouse is in the given rect
 * @param r the rectangle to check against
 * @return 1 if it is within the rect, 0 otherwise
 */
int gf2d_mouse_in_rect(Rect r);

/**
 * @brief set the action for the mouse icon
 * @param action the name of the action to set it to
 */
void gf2d_mouse_set_action(char *action);

/**
 * @brief set the action for the mouse icon
 * @param mf the enum of the function to set it to
 */
void gf2d_mouse_set_function(MouseFunction mf);

/**
 * @brief get the current function selected for the mouse
 * @return a MouseFunction code
 */
MouseFunction gf2d_mouse_get_function();

/**
 * @brief set the action to use for the spell actor
 * @param action if NULL clears the spell associated with the spell, otherwise will use the actor specified
 */
void gf2d_mouse_set_spell_action(char *action);

/**
 * @brief set the actor to use for the item
 * @param action     if NULL clears the item associated with the item action, otherwise will use the actor specified
 */
void gf2d_mouse_set_item_action(char *action);

/**
 * @brief update the logic for mouse context switching
 */
void gf2d_mouse_scene_update();


#endif
