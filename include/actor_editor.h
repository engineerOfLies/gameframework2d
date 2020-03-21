#ifndef __ACTOR_EDITOR_H__
#define __ACTOR_EDITOR_H__

#include "gf2d_actor.h"
#include "gf2d_windows.h"

Window *actor_editor_menu(char *actorFile,char *action,void *data, void(*onOk)(void *),void(*onCancel)(void *));

#endif
