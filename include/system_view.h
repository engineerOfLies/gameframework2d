#ifndef __SYSTEM_VIEW_H__
#define __SYSTEM_VIEW_H__

#include "gf2d_windows.h"
#include "empire.h"
#include "systems.h"

Window *system_view_window(Empire *empire, System *system,Window *parent);
void system_view_close_child_window(Window *win);

#endif
