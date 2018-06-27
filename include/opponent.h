#ifndef __OPPONENT_H__
#define __OPPONENT_H__

#include "combat.h"

List *opponent_setup_choices();
void opponent_think(List *decisions,Combatant *com,float position);

#endif
