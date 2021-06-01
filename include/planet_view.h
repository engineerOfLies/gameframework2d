#ifndef __PLANET_VIEW_H__
#define __PLANET_VIEW_H__

#include "gf2d_windows.h"

#include "empire.h"
#include "planet.h"

Window *planet_view_window(Empire *empire,Planet *planet,Window *parent);
void planet_view_close_child_window(Window *win);

#endif
