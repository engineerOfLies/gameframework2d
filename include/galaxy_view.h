#ifndef __GALAXY_VIEW_H__
#define __GALAXY_VIEW_H__

#include "gf2d_windows.h"
#include "empire.h"
#include "galaxy.h"

Window *galaxy_view_window(Empire *emipre, Galaxy *galaxy,Window *parent);
void galaxy_view_close_child_window(Window *win);

#endif
