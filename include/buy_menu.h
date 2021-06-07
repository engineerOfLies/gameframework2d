#ifndef __OPTIONS_MENU_H__
#define __OPTIONS_MENU_H__

#include "gf2d_entity.h"
#include "gf2d_windows.h"

#include "empire.h"

/**
 * @brief open a buy menu
 * @param empire this empire's resources will be used to determine if the player can afford the items
 * @param shopConfig the json file used to configure the shop
 * @param buyChoice the choice made will be populated here unless the cancel option was selected
 * @param onBuy the callback function to invoke when the player makes a purchase
 * @param onCancel the callback function to invoke when the player cancels the operation
 * @param cData the data to be passed to the callback
 */
Window *buy_menu(Empire *empire,const char *shopConfig,int *choice,void(*onBuy)(void *),void(*onCancel)(void *),void *cData);

#endif
