#ifndef __EMPIRE_HUD_H__
#define __EMPIRE_HUD_H__

#include "gf2d_windows.h"

#include "galaxy.h"
#include "empire.h"

Window *empire_hud_window(Empire *empire,Galaxy *galaxy);

/**
 * @brief bring the hud to the top of the window stack.
 */
void empire_hud_bubble();


#endif
