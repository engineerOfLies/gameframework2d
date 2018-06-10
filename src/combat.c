#include "gf2d_draw.h"
#include "gf2d_windows.h"
#include "simple_logger.h"

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
    CS_Parried,         // vulnerable state after an attack was parried
    CS_Dying            // triggers death animation
}CombatState;

typedef enum
{
    CA_LeftWeak = 0,
    CA_LeftStrong,
    CA_DodgingLeft,
    CA_RetreatingLeft,
    CA_AdvancingLeft,
    CA_RightWeak,
    CA_RightStrong,
    CA_DodgingRight,
    CA_RetreatingRight,
    CA_AdvancingRight,
    CA_MAX
}CombatActions;

typedef struct
{
    CombatState state;
    Uint32      cooldown;           /**<how long before the player can act again*/
    int         health,healthMax;
    int         mana,manaMax;
    int         stamina,staminaMax;
    TextLine    actions[CA_MAX];
    Color       color;
}Combatant;

typedef struct
{
    Window *win;
    Uint8 position;     /**<close or far*/
    Combatant combatant[2];
}Combat;

static Combat combat_info = {0};


void combat_draw_combatant(Combatant *com,int position)
{
    SDL_Rect rect = {600,80,128,256};
    if (!com)return;
    rect.x -= (1 - position) * 200;
    rect.y += (1 - position) * 200;
    gf2d_draw_rect(rect,gf2d_color_to_vector4(com->color));
}


int combat_menu_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    if (!win)return 0;
    if (!updateList)return 0;
    count = gf2d_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gf2d_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 1001:
                slog("Left Weak");
                break;
            case 1002:
                slog("Left Dodge");
                break;
            case 1003:
                slog("Left Strong");
                break;
            case 2001:
                slog("Advance");
                break;
            case 2003:
                slog("Retreat");
                break;
            case 3001:
                slog("Right Weak");
                break;
            case 3002:
                slog("Right Dodge");
                break;
            case 3003:
                slog("Right Strong");
                break;
        }
    }
    return 0;
}

int combat_menu_draw(Window *win)
{
    combat_draw_combatant(&combat_info.combatant[0],0);
    combat_draw_combatant(&combat_info.combatant[1],1);
    return 0;
}

Window * combat_menu()
{
    Window *win;
    SJson *json = NULL;
    json = sj_load("config/testwindow.cfg");
    win = gf2f_window_load_from_json(json);
    if (win)
    {
        win->update = combat_menu_update;
        win->draw = combat_menu_draw;
    }
    sj_free(json);
    return win;
}

void combat_init()
{
    combat_info.win = combat_menu();
    combat_info.combatant[0].color = gf2d_color8(0,0,255,255);
    combat_info.combatant[1].color = gf2d_color8(255,0,0,255);
}

/*eol@eof*/
