#ifndef __WINDOWS_COMMON_H__
#define __WINDOWS_COMMON_H__

#include "gf2d_windows.h"

Window *window_alert(const char *title, const char *text, void(*onOK)(void *),void *okData);
Window *window_dialog(const char *title, const char *text, void(*onOK)(void *),void *okData);
Window *window_yes_no(const char *text, void(*onYes)(void *),void(*onNo)(void *),void *yesData,void *noData);
Window *window_text_entry(const char *question,char *defaultText, void *callbackData, size_t length, void(*onOk)(void *),void(*onCancel)(void *));


/**
 * @brief open an options window with a number of items to choose from
 * @param position where on the screen to place it.  Window convention applies.  negatives will be FROM the right/bottom.  Values under 1 will be used as percentages of screen resolution.
 * @param question the question/title of the options list
 * @param n the number of options
 * @param optionText an array of n strings for each option
 * @param onOption an array of n callback functions to call on the event of the option in question being selected
 * @param onCancel if the user cancels (ESC by default) the options, call this function instead
 * @param data the data to be provided to the callbacks
 * @param returnValue if not NULL the integer provided will be populated with the selected option if a choice is made (not touched on cancel)
 * @return a pointer to the window created
 */
Window *window_list_options(Vector2D position,const char *question, int n, const char*optionText[], void(*onOption[])(void *),void(*onCancel)(void *),void *data,int *returnValue);


/**
 * @brief call the cancel action for a list_options menu
 * @param win the list options window to cancel
 */
void window_list_options_cancel(Window *win);


#endif
