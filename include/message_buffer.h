#ifndef __MESSAGE_BUFFER_H__
#define __MESSAGE_BUFFER_H__

#include "gfc_color.h"

/**
 * @brief intialize the message buffer window
 * @param count how many messages will be displayed at once
 */
void message_buffer_init(int count,Uint32 timeout, Color defaultColor);

void message_new(const char *newMessage);
void message_buffer_bubble();


#endif
