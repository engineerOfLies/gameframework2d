#ifndef __COMBAT_H__
#define __COMBAT_H__

#include "skills.h"
#include "gf2d_actor.h"
#include "gf2d_text.h"
#include "gf2d_windows.h"

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

typedef enum
{
    CS_Idle = 0,        // able to perform any action from this state
    CS_Retreating,      // only able to perform a retreating action from this state Transition to far
    CS_Advancing,       // only able to perform an advancing action from this state Transition to close
    CS_Pain,            // cannot act, will automatically return to recover
    CS_Stun,            // cannot act, different animation from Pain, will automatically return to pain
    CS_Windup,          // begining of an action.  Committed and vulnerable in this state
    CS_Action,          // active frames of an action  returns to recover from here
    CS_Recover,         // Vulnerable, but automatically returns to Idle
    CS_DodgingLeft,     // Avoids linear attacks and swinging attacks that go right
    CS_DodgingRight,    // Avoids linear attacks and swinging attacks that go left
    CS_Blocking,        // blocks incoming attacks
    CS_Parrying,        // puts melee attacks into parried state
    CS_Parried,         // vulnerable state after an attack was parried
    CS_Dying            // triggers death animation
}CombatState;

typedef struct Combatant_S
{
    CombatState state;
    Actor       actor;
    TextLine    stateString;
    float       cooldown;           /**<how long before the player state changes*/
    float       health,healthMax;
    float       mana,manaMax;
    float       stamina,staminaMax;
    float       weaponDamage;
    Skill     * actions[CA_MAX];
    Color       color;
    Window      *statusMenu;
    Skill     * currentAction;
    const char* holdInput;          /**<input to check for held status*/
    Vector2D    nearPosition,farPosition;
    void (*stateUpdate)(struct Combatant_S *com);
    List      * decisions;          /**<for AI*/
    struct Combatant_S  *other;     /**<opponent*/
    TextLine    lastChoice;
}Combatant;

void combat_init();
void combat_start_action(Combatant *com,Skill *action,const char *hold);

#endif
