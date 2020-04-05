#ifndef __HUD_H__
#define __HUD_H__

#include "gf2d_windows.h"
#include "scene.h"
#include "player.h"

/**
 * @brief launch the hud
 * @param player the player the hud should reflect
 * @return NULL on error or the HUD otherwise
 */
Window *hud_open(Entity *player);

/**
 * @brief get the hud
 * @return NULL if not set, the active HUD otherwise
 */
Window *hud_get();

/**
 * @brief check if the mouse is over the HUD
 * @return 0 if no HUD or the mouse is not over it, 1 otherwise
 */
int hud_mouse_check();

#endif
