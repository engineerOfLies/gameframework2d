#ifndef __EDITOR_H__
#define __EDITOR_H__
#include "gf2d_windows.h"
#include "scene.h"


Window *editor_window(Scene *scene);
Window *editor_menu();
void editor_deselect_layer(Window *win);


#endif
