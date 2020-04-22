#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_callbacks.h"
#include "gf2d_graphics.h"
#include "gf2d_windows.h"
#include "gf2d_elements.h"
#include "gf2d_element_button.h"
#include "gf2d_element_list.h"
#include "gf2d_element_label.h"
#include "gf2d_element_actor.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"
#include "gf2d_shape.h"
#include "gf2d_mouse.h"

#include "camera.h"
#include "windows_common.h"
#include "player.h"
#include "items.h"
#include "player_status_menu.h"

typedef struct
{
    Entity *player;
}PlayerStatsData;

int player_status_menu_free(Window *win)
{
    PlayerStatsData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    free(data);
    return 0;
}


int player_status_menu_draw(Window *win)
{
    return 0;
}

int player_status_menu_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    PlayerStatsData* data;
    if (!win)return 0;
    if (!updateList)return 0;
    data = (PlayerStatsData*)win->data;
    if (!data)return 0;
    
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 51:
                gf2d_window_free(win);
                return 1;
        }
    }
    return 1;
}

void player_status_menu_set_attributes(Window *win, Entity *player)
{
    char *name;
    int         health, stamina, mana;
    int         healthMax,staminaMax,manaMax;
    Inventory *skills;
    int i,c;
    TextLine buffer;
    Element *e,*list;
    InventoryItem *attribute;
    if ((!win)||(!player))return;
    
    name = player_get_name(player);
    if (name)
    {
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1),name);
    }
    
    attribute = player_get_attribute(player, "Strength");
    if (attribute)
    {
        gfc_line_sprintf(buffer,"Strength: %i",attribute->skill);
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1000),buffer);
    }
    attribute = player_get_attribute(player, "Endurance");
    if (attribute)
    {
        gfc_line_sprintf(buffer,"Endurance: %i",attribute->skill);
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1001),buffer);
    }
    attribute = player_get_attribute(player, "Dexterity");
    if (attribute)
    {
        gfc_line_sprintf(buffer,"Dexterity: %i",attribute->skill);
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1002),buffer);
    }
    attribute = player_get_attribute(player, "Agility");
    if (attribute)
    {
        gfc_line_sprintf(buffer,"Agility: %i",attribute->skill);
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1003),buffer);
    }
    attribute = player_get_attribute(player, "Focus");
    if (attribute)
    {
        gfc_line_sprintf(buffer,"Focus: %i",attribute->skill);
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1004),buffer);
    }
    attribute = player_get_attribute(player, "Resolve");
    if (attribute)
    {
        gfc_line_sprintf(buffer,"Resolve: %i",attribute->skill);
        gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,1005),buffer);
    }

    player_get_core_attributes(player,&health,&healthMax,&stamina, &staminaMax,&mana, &manaMax);

    gfc_line_sprintf(buffer,"Health: %i / %i",health,healthMax);
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,2000),buffer);
    gfc_line_sprintf(buffer,"Stamina: %i / %i",stamina,staminaMax);
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,2001),buffer);
    gfc_line_sprintf(buffer,"mana: %i / %i",mana,manaMax);
    gf2d_element_label_set_text(gf2d_window_get_element_by_id(win,2002),buffer);
    
    skills = player_get_skill_inventory(player);
    
    c = inventory_get_count(skills);
    list = gf2d_window_get_element_by_id(win,3000);
    if (!list)return;
    for (i = 0;i < c; i++)
    {
        attribute = inventory_get_nth(skills,i);
        if (!attribute)continue;
        gfc_line_sprintf(buffer,"%s: %i",attribute->name,attribute->skill);
        e = gf2d_element_new_full(
            list,
            0,
            "skill",
            gf2d_rect(0,0,1,1),
            gfc_color(1,1,1,1),
            0,
            gfc_color(1,1,1,1),
            0);
        gf2d_element_make_label(e,gf2d_element_label_new_full(buffer,gfc_color(1,1,1,1),FT_H3,LJ_Left,LA_Middle,0));
        gf2d_element_list_add_item(list,e);
    }


}


Window *player_status_menu(Entity *player)
{
    Window *win;
    PlayerStatsData* data;
    win = gf2d_window_load("menus/player_status_menu.json");
    if (!win)
    {
        slog("failed to load player status menu menu");
        return NULL;
    }
    win->update = player_status_menu_update;
    win->free_data = player_status_menu_free;
    win->draw = player_status_menu_draw;
    data = (PlayerStatsData*)gfc_allocate_array(sizeof(PlayerStatsData),1);
     player_status_menu_set_attributes(win, player);
    win->data = data;
    data->player = player;
    return win;
}


/*eol@eof*/
