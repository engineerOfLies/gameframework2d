#ifndef __COMBAT_H__
#define __COMBAT_H__

typedef enum
{
    CA_Advancing = 0,
    CA_Retreating,
    CA_DodgingLeft,
    CA_DodgingRight,
    CA_LeftWeak,
    CA_LeftStrong,
    CA_DodgingLeftAction,
    CA_RetreatingLeft,
    CA_AdvancingLeft,
    CA_RightWeak,
    CA_RightStrong,
    CA_DodgingRightAction,
    CA_RetreatingRight,
    CA_AdvancingRight,
    CA_MAX
}CombatActions;

void combat_init();

#endif
