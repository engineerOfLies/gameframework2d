#include "decision.h"
#include "opponent.h"
#include "simple_logger.h"

List *opponent_setup_choices()
{
    List *decisions = NULL;
    decisions = decision_list_new();
    if (!decisions)return NULL;
    decision_list_insert_choice(decisions,"dagger_thrust");
    decision_list_insert_choice(decisions,"dagger_thrust");
    decision_list_insert_choice(decisions,"shield_block");
    decision_list_insert_choice(decisions,"dagger_slash");
    decision_list_insert_choice(decisions,"dagger_slash");
    decision_list_insert_choice(decisions,"dodge_left");
    decision_list_insert_choice(decisions,"dodge_right");
    decision_list_insert_choice(decisions,"shield_parry");
    return decisions;
}

void opponent_think(List *decisions,Combatant *com,float position)
{
    const char *choice;
    Skill *skill;
    if (com->state == CS_Idle)
    {
        if (com->cooldown > 0)return;//think delay
        slog("AI THINK");
        if (com->stamina <= 5)
        {
            com->cooldown = 30;
            slog("AI: Resting");
            return;
        }
        choice = decision_list_get_best_choice(decisions);
        if (gf2d_line_cmp(com->lastChoice,choice)==0)
        {
            decision_list_update_choice(decisions, choice, 0.2);
        }
        skill = skill_get_by_name((char *)choice);
        if (skill_check_tag(skill, "melee") && (position == 1))
        {
            combat_start_action(com,skill_get_by_name("advance"),NULL);
            return;
        }
        if (skill_check_tag(skill, "ranged") && (position == 0))
        {
            combat_start_action(com,skill_get_by_name("retreat"),NULL);
            return;
        }
        combat_start_action(com,skill,NULL);
    }
}

/*eol@eof*/
