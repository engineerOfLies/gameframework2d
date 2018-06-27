#include "gf2d_draw.h"
#include "gf2d_windows.h"
#include "gf2d_actor.h"
#include "gf2d_element_button.h"
#include "gf2d_graphics.h"
#include "gf2d_input.h"
#include "simple_logger.h"

#include "combat.h"
#include "skills.h"
#include "opponent.h"
#include "decision.h"

typedef struct
{
    Window *combatMenu;
    float   position;     /**<close or far*/
    Combatant combatant[2];
}Combat;

static Combat combat_info = {0};

const char *combat_state_to_string(CombatState state);

void combat_draw_combatant(Combatant *com)
{
    SDL_Rect rect = {0,0,128,256};
    Vector2D position;
    if (!com)return;
    gf2d_line_cpy(com->stateString,combat_state_to_string(com->state));
    position.x = (com->nearPosition.x * (1.0 - combat_info.position)) + (com->farPosition.x * combat_info.position);
    position.y = (com->nearPosition.y * (1.0 - combat_info.position)) + (com->farPosition.y * combat_info.position);

    rect.x += position.x;
    rect.y += position.y;
    if (com->actor.sprite)
    {
        position.x -= 205;
        position.y -= 255;
        gf2d_actor_draw(
            &com->actor,
            position,
            NULL,
            NULL,
            NULL,
            NULL);
        gf2d_actor_next_frame(&com->actor);
    }
    else
    {
        gf2d_draw_solid_rect(rect,gf2d_color_to_vector4(com->color));
        gf2d_text_draw_line(com->stateString,FT_H2,gf2d_color8(255,255,255,255), position);
    }
}


void idle_state(Combatant *com)
{
    if (!com)return;
    com->stamina += 0.5;
    if (com->stamina > com->staminaMax)com->stamina = com->staminaMax;
    com->mana += 0.1;
    if (com->mana > com->manaMax)com->mana = com->manaMax;
}

void recover_state(Combatant *com)
{
    slog("recovering...");
    if (!com)return;
    if (com->cooldown != 0)return;
    com->state = CS_Idle;
    com->stateUpdate = idle_state;
    com->currentAction = NULL;
    gf2d_actor_set_action(&com->actor,"idle");
    com->cooldown = 40 + (gf2d_random() *30);
}

void pain_state(Combatant *com)
{
    slog("owww...");
    if (!com)return;
    if (com->cooldown != 0)return;
    com->state = CS_Idle;
    com->stateUpdate = idle_state;
    com->currentAction = NULL;
    gf2d_actor_set_action(&com->actor,"idle");
    com->cooldown = 40 + (gf2d_random() *30);
}

void pain(Combatant *com)
{
    if (!com)return;
    com->state = CS_Pain;
    com->stateUpdate = pain_state;
    if (gf2d_crandom() > 0)
    {
        com->cooldown = 20;
        gf2d_actor_set_action(&com->actor,"pain1");
    }
    else
    {
        com->cooldown = 25;
        gf2d_actor_set_action(&com->actor,"pain2");
    }
}

void recover(Combatant *com)
{
    if (!com)return;
    com->state = CS_Recover;
    com->stateUpdate = recover_state;
    if (com->currentAction == NULL)
    {
        com->cooldown = 10;
    }
    else
    {
        com->cooldown = com->currentAction->recover;
    }
    gf2d_actor_set_action(&com->actor,(char *)gf2d_list_get_nth(com->currentAction->animation,2));
}


void advancing_state(Combatant *com)
{
    slog("advancing...");
    if (!com)return;
    if (combat_info.position > 0)
    {
        combat_info.position -= 1.0/com->currentAction->actionTime;
        if (combat_info.position < 0)combat_info.position = 0;
    }
    if (com->cooldown != 0)return;
    recover(com);
}

void retreating_state(Combatant *com)
{
    slog("retreating...");
    if (!com)return;
    if (combat_info.position < 1)
    {
        combat_info.position += 1.0/com->currentAction->actionTime;
        if (combat_info.position > 1)combat_info.position = 1;
    }
    if (com->cooldown != 0)return;
    recover(com);
}

void dodging_state(Combatant *com)
{
    //TODO: update relative postions in the dodging direction
    if (!com)return;
    if (com->state == CS_DodgingLeft)
    {
        slog("dodgin left...");
    }
    else
    {
        slog("dodging right...");
    }
    if (com->cooldown != 0)return;
    //evaluate success for AI
    if (com->decisions != NULL)
    {
        decision_list_update_choice(com->decisions, com->currentAction->name, 0.5);
    }
    recover(com);
}

void deal_damage(Combatant *com,float damage)
{
    com->health -= damage;
    if (com->currentAction)
    {
        decision_list_update_choice(com->decisions, com->currentAction->name, 0);
    }
    pain(com);
}

void action_state(Combatant *com)
{
    if (!com)return;
    if (skill_check_tag(com->currentAction, "held"))
    {
        if (gf2d_input_command_held(com->holdInput))
        {
            slog("Holding...");
            return;
        }
    }
    if (skill_check_tag(com->currentAction, "advances"))
    {
        advancing_state(com);
        return;
    }
    if (skill_check_tag(com->currentAction, "retreats"))
    {
        retreating_state(com);
        return;
    }
    if (com->cooldown != 0)return;
    if (skill_check_tag(com->currentAction, "melee"))
    {
        if (combat_info.position > 0.5)
        {
            slog("miss");
            decision_list_update_choice(com->decisions, com->currentAction->name, 0);
        }
        else switch (com->other->state)
        {
            case CS_Idle:
            case CS_Dying:
            case CS_Pain:
            case CS_Stun:
            case CS_Recover:
                slog("HIT");
                deal_damage(com->other,com->currentAction->damageMod * com->weaponDamage);
                decision_list_update_choice(com->decisions, com->currentAction->name, 0.9);
                break;
            case CS_Advancing:
            case CS_Retreating:
            case CS_Windup:
            case CS_Action:
            case CS_Parried:
                slog("Counter HIT");
                decision_list_update_choice(com->decisions, com->currentAction->name, 1.1);
                break;
            case CS_DodgingLeft:
            case CS_DodgingRight:
                slog("Dodged!");
                decision_list_update_choice(com->decisions, com->currentAction->name, 0.1);
                decision_list_update_choice(com->other->decisions, com->other->currentAction->name, 1);
                break;
            case CS_Blocking:
                slog("blocked");
                decision_list_update_choice(com->decisions, com->currentAction->name, 0.1);
                decision_list_update_choice(com->other->decisions, com->other->currentAction->name, 1);
                break;
            case CS_Parrying:
                slog("Parried");
                decision_list_update_choice(com->decisions, com->currentAction->name, 0.1);
                decision_list_update_choice(com->other->decisions, com->other->currentAction->name, 1);
                break;
        }
    }
    recover(com);
}

void windup_state(Combatant *com)
{
    slog("winding up...");
    if (!com)return;
    if (com->cooldown != 0)return;
    if (com->currentAction == NULL)return;
    com->cooldown = com->currentAction->actionTime;
    if (skill_check_tag(com->currentAction, "advances"))
    {
        com->state = CS_Advancing;
        com->stateUpdate = advancing_state;
        return;
    }
    if (skill_check_tag(com->currentAction, "retreats"))
    {
        com->state = CS_Retreating;
        com->stateUpdate = retreating_state;
        return;
    }
    if (skill_check_tag(com->currentAction, "dodges_left"))
    {
        com->state = CS_DodgingLeft;
        com->stateUpdate = dodging_state;
        return;
    }
    if (skill_check_tag(com->currentAction, "dodges_right"))
    {
        com->state = CS_DodgingRight;
        com->stateUpdate = dodging_state;
        return;
    }
    gf2d_actor_set_action(&com->actor,(char *)gf2d_list_get_nth(com->currentAction->animation,1));
    if (skill_check_tag(com->currentAction, "blocks"))
    {
        com->state = CS_Blocking;
        com->stateUpdate = action_state;
        return;
    }
    if (skill_check_tag(com->currentAction, "parries"))
    {
        com->state = CS_Parrying;
        com->stateUpdate = action_state;
        return;
    }
    com->state = CS_Action;
    com->stateUpdate = action_state;
}

void combatant_update(Combatant *com)
{
    float fps;
    if (!com)return;
    if (com->cooldown > 0)
    {
        fps = gf2d_graphics_get_frames_per_second();
        if (fps == 0)
        {
            com->cooldown -= 0.01;
        }
        else com->cooldown -= 60/gf2d_graphics_get_frames_per_second();
        if (com->cooldown < 0.5)com->cooldown = 0;
        if (com->decisions)slog("AI Cooling down %f",com->cooldown);
    }
    if (com->stateUpdate != NULL)
    {
        com->stateUpdate(com);
    }
    if (com->decisions != NULL)
    {
        opponent_think(com->decisions,com,combat_info.position);
    }
}

void combat_update()
{
    combatant_update(&combat_info.combatant[0]);
    combatant_update(&combat_info.combatant[1]);
}

void combat_start_action(Combatant *com,Skill *action,const char *hold)
{
    if ((!com)||(!action))return;
    slog("action: %s",action->name);
    if ((action->staminaCost > 0)&&(com->stamina <= 0))return;
    if ((action->manaCost > 0)&&(com->mana <= 0))return;
    if ((action->healthCost > 0)&&(com->health <= 0))return;
    com->currentAction = action;
    com->cooldown = action->windup;
    com->state = CS_Windup;
    com->stateUpdate = windup_state;
    com->holdInput = hold;
    com->stamina -= action->staminaCost;
    com->mana -= action->manaCost;
    com->health -= action->healthCost;
    gf2d_actor_set_action(&com->actor,(char *)gf2d_list_get_nth(action->animation,0));
}

int combat_menu_update(Window *win,List *updateList)
{
    Combatant *player;
    int i,count;
    Element *e;
    if (!win)return 0;
    if (!updateList)return 0;
    player = &combat_info.combatant[0];
    combat_update();
    count = gf2d_list_get_count(updateList);
    if (player->state == CS_Idle)
    {
        for (i = 0; i < count; i++)
        {
            e = gf2d_list_get_nth(updateList,i);
            if (!e)continue;
            switch(e->index)
            {
                case 1001:
                    slog("Left Weak");
                    combat_start_action(player,player->actions[CA_LeftWeak],gf2d_element_button_get_input(e));
                    break;
                case 1002:
                    slog("Left Dodge");
                    combat_start_action(player,player->actions[CA_DodgingLeft],gf2d_element_button_get_input(e));
                    break;
                case 1003:
                    slog("Left Strong");
                    combat_start_action(player,player->actions[CA_LeftStrong],gf2d_element_button_get_input(e));
                    break;
                case 2001:
                    slog("Advance");
                    combat_start_action(player,player->actions[CA_Advancing],gf2d_element_button_get_input(e));
                    break;
                case 2003:
                    slog("Retreat");
                    combat_start_action(player,player->actions[CA_Retreating],gf2d_element_button_get_input(e));
                    break;
                case 3001:
                    slog("Right Weak");
                    combat_start_action(player,player->actions[CA_RightWeak],gf2d_element_button_get_input(e));
                    break;
                case 3002:
                    slog("Right Dodge");
                    combat_start_action(player,player->actions[CA_DodgingRight],gf2d_element_button_get_input(e));
                    break;
                case 3003:
                    slog("Right Strong");
                    combat_start_action(player,player->actions[CA_RightStrong],gf2d_element_button_get_input(e));
                    break;
            }
        }
    }
    else if (player->state == CS_Advancing)
    {
        for (i = 0; i < count; i++)
        {
            e = gf2d_list_get_nth(updateList,i);
            if (!e)continue;
            switch(e->index)
            {
                case 3001:
                case 3003:
                    slog("Advancing Right Action");
                    combat_start_action(player,player->actions[CA_AdvancingRight],gf2d_element_button_get_input(e));
                    break;
                case 1001:
                case 1003:
                    slog("Advancing Left Action");
                    combat_start_action(player,player->actions[CA_AdvancingLeft],gf2d_element_button_get_input(e));
                    break;
            }        
        }
    }
    else if (player->state == CS_Retreating)
    {
        for (i = 0; i < count; i++)
        {
            e = gf2d_list_get_nth(updateList,i);
            if (!e)continue;
            switch(e->index)
            {
                case 3001:
                case 3003:
                    slog("Retreating Right Action");
                    combat_start_action(player,player->actions[CA_RetreatingRight],gf2d_element_button_get_input(e));
                    break;
                case 1001:
                case 1003:
                    slog("Retreating Left Action");
                    combat_start_action(player,player->actions[CA_RetreatingLeft],gf2d_element_button_get_input(e));
                    break;
            }        
        }
    }
    else if (player->state == CS_DodgingLeft)
    {
        for (i = 0; i < count; i++)
        {
            e = gf2d_list_get_nth(updateList,i);
            if (!e)continue;
            switch(e->index)
            {
                case 3001:
                case 3003:
                    slog("Dodging Left Action");
                    combat_start_action(player,player->actions[CA_DodgingLeftAction],gf2d_element_button_get_input(e));
                    break;
            }        
        }
    }
    else if (player->state == CS_DodgingRight)
    {
        for (i = 0; i < count; i++)
        {
            e = gf2d_list_get_nth(updateList,i);
            if (!e)continue;
            switch(e->index)
            {
                case 1001:
                case 1003:
                    slog("Dodging Right Action");
                    combat_start_action(player,player->actions[CA_DodgingRightAction],gf2d_element_button_get_input(e));
                    break;
            }        
        }
    }
    return 0;
}

int combat_menu_draw(Window *win)
{
    combat_draw_combatant(&combat_info.combatant[1]);
    combat_draw_combatant(&combat_info.combatant[0]);
    return 0;
}

Window * combat_menu()
{
    Window *win;
    win = gf2d_window_load_from_file("config/combatoptions.cfg");
    if (win)
    {
        win->update = combat_menu_update;
        win->draw = combat_menu_draw;
    }
    return win;
}

int status_menu_draw(Window *win)
{
    Combatant *com;
    SDL_Rect rect = {0,0,220,24};
    SDL_Rect level = {0,0,220,24};
    if (!win)return 0;
    com = (Combatant*)win->data;
    if (!com)return 0;
    // health
    level.x = rect.x = win->dimensions.x + 5;
    level.y = rect.y = win->dimensions.y + 25;
    gf2d_draw_solid_rect(rect, vector4d(128,0,0,255));
    level.w = rect.w * (MAX(com->health,0) / com->healthMax);
    gf2d_draw_solid_rect(level, vector4d(255,0,0,255));    
    gf2d_draw_rect(rect, vector4d(185,121,73,255));
    
    // stamina
    rect.y += 25;
    level.y += 25;
    gf2d_draw_solid_rect(rect, vector4d(0,128,0,255));
    level.w = rect.w * (MAX(com->stamina,0) / com->staminaMax);
    gf2d_draw_solid_rect(level, vector4d(0,255,0,255));
    gf2d_draw_rect(rect, vector4d(185,121,73,255));

    // mana
    rect.y += 25;
    level.y += 25;
    gf2d_draw_solid_rect(rect, vector4d(0,0,128,255));
    level.w = rect.w * (MAX(com->mana,0) / com->manaMax);
    gf2d_draw_solid_rect(level, vector4d(0,0,255,255));
    gf2d_draw_rect(rect, vector4d(185,121,73,255));
    
    //cooldown
    level.x = rect.x = win->dimensions.x + 227;
    level.y = rect.y = win->dimensions.y + 25;
    level.w = rect.w = 20;
    rect.h = 75;
    gf2d_draw_solid_rect(rect, vector4d(100,0,100,255));
    level.h = rect.h * (MIN(75,(com->cooldown * 10))/rect.h);
    level.y = rect.y + rect.h - level.h;
    gf2d_draw_solid_rect(level, vector4d(200,0,200,255));
    gf2d_draw_rect(rect, vector4d(185,121,73,255));
    
    return 0;
}

Window * statusMenu(Vector2D offset,Combatant *com)
{
    Window *win;
    win = gf2d_window_load_from_file("config/statusmenu.cfg");
    if (win)
    {
        win->draw = status_menu_draw;
        win->data = (void*)com;
        gf2d_window_set_position(win,offset);
    }
    return win;
}

void combatant_setup_skills(Combatant *com)
{
    if (!com)return;
    com->actions[CA_Advancing] = skill_get_by_name("advance");
    com->actions[CA_Retreating] = skill_get_by_name("retreat");
    com->actions[CA_DodgingLeft] = skill_get_by_name("dodge_left");
    com->actions[CA_DodgingRight] = skill_get_by_name("dodge_right");
    com->actions[CA_LeftWeak] = skill_get_by_name("shield_block");
    com->actions[CA_LeftStrong] = skill_get_by_name("shield_parry");
    com->actions[CA_DodgingRightAction] = skill_get_by_name("shield_deflect");
    com->actions[CA_RetreatingLeft] = skill_get_by_name("shield_deflect");
    com->actions[CA_AdvancingLeft] = skill_get_by_name("shield_deflect");;
    com->actions[CA_RightWeak] = skill_get_by_name("dagger_thrust");
    com->actions[CA_RightStrong] = skill_get_by_name("dagger_slash");
    com->actions[CA_DodgingLeftAction] = skill_get_by_name("dagger_strike");
    com->actions[CA_RetreatingRight] = skill_get_by_name("dagger_parry");
    com->actions[CA_AdvancingRight] = skill_get_by_name("dagger_lunge");
}

void combat_init()
{
    Element *e;
    int i;
    combat_info.combatMenu = combat_menu();
    combat_info.combatant[0].color = gf2d_color8(0,0,255,255);
    combat_info.combatant[0].statusMenu = statusMenu(vector2d(20,20),&combat_info.combatant[0]);
    combatant_setup_skills(&combat_info.combatant[0]);
    vector2d_set(combat_info.combatant[0].nearPosition,375,355);
    vector2d_set(combat_info.combatant[0].farPosition,250,480);
    e = gf2d_window_get_element_by_name(combat_info.combatant[0].statusMenu,"title");
    if (e)
    {
        gf2d_element_label_set_text(e,"Human Player");
    }
    gf2d_actor_load(&combat_info.combatant[0].actor,"actors/player.actor");
    gf2d_actor_set_action(&combat_info.combatant[0].actor,"idle");
    combat_info.combatant[0].other = &combat_info.combatant[1];

    combat_info.combatant[1].other = &combat_info.combatant[0];
    combat_info.combatant[1].color = gf2d_color8(255,0,0,255);
    combat_info.combatant[1].statusMenu = statusMenu(vector2d(930,20),&combat_info.combatant[1]);
    vector2d_set(combat_info.combatant[1].nearPosition,525,225);
    vector2d_set(combat_info.combatant[1].farPosition,630,140);
    gf2d_actor_load(&combat_info.combatant[1].actor,"actors/opponent.actor");
    gf2d_actor_set_action(&combat_info.combatant[1].actor,"idle");
    e = gf2d_window_get_element_by_name(combat_info.combatant[1].statusMenu,"title");
    if (e)
    {
        gf2d_element_label_set_text(e,"NPC Player");
    }
    combat_info.combatant[1].decisions = opponent_setup_choices();

    for (i = 0;i < 2;i++)
    {
        combat_info.combatant[i].health = combat_info.combatant[i].healthMax = 100;
        combat_info.combatant[i].stamina = combat_info.combatant[i].staminaMax = 100;
        combat_info.combatant[i].mana = combat_info.combatant[i].manaMax = 100;
        combat_info.combatant[i].cooldown = 100;
        combat_info.combatant[i].stateUpdate = idle_state;
        combat_info.combatant[i].weaponDamage = 5;
    }
}

const char *combat_state_to_string(CombatState state)
{
    switch (state)
    {
        case CS_Idle:
            return "Idle";
            break;
        case CS_Retreating:   
            return "Retreating";
            break;
        case CS_Advancing:
            return "Advancing";
            break;
        case CS_Pain:   
            return "In Pain";
            break;
        case CS_Stun:
            return "Stunned";
            break;
        case CS_Windup:
            return "Winding Up";
            break;
        case CS_Action:
            return "Acting";
            break;
        case CS_Recover:
            return "Recovering";
            break;
        case CS_DodgingLeft:
            return "Dodging Left";
            break;
        case CS_DodgingRight:
            return "Dodging Right";
            break;
        case CS_Blocking:
            return "Blocking";
            break;
        case CS_Parrying:
            return "Parrying";
            break;
        case CS_Parried:
            return "Parried";
            break;
        case CS_Dying:
            return "Dying";
            break;
    }
    return "";
}
/*eol@eof*/
