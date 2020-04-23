#ifndef __INVENTORY_MENU_H__
#define __INVENTORY_MENU_H__

#include "gf2d_windows.h"
#include "inventory.h"

Window *inventory_menu(char *title, Inventory *inven,char *actor,char *action,void (*onSelect)(void *data),void *callbackData);

#endif
