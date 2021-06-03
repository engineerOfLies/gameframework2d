#ifndef __WINDOWS_COMMON_H__
#define __WINDOWS_COMMON_H__

#include "gf2d_windows.h"

Window *window_alert(const char *title, const char *text, void(*onOK)(void *),void *okData);
Window *window_dialog(const char *title, const char *text, void(*onOK)(void *),void *okData);
Window *window_yes_no(const char *text, void(*onYes)(void *),void(*onNo)(void *),void *yesData,void *noData);
Window *window_text_entry(const char *question,char *defaultText, void *callbackData, size_t length, void(*onOk)(void *),void(*onCancel)(void *));
Window *window_list_options(const char *question, int n, const char*optionText[], void(*onOption[])(void *),void(*onCancel)(void *),void *data);


#endif
