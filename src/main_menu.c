#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gf2d_graphics.h"
#include "gf2d_windows.h"
#include "gf2d_elements.h"
#include "gf2d_element_label.h"
#include "gf2d_draw.h"
#include "gf2d_shape.h"
#include "gf2d_mouse.h"

#include "camera.h"
#include "windows_common.h"
#include "galaxy.h"
#include "empire.h"
#include "empire_hud.h"
#include "main_menu.h"

extern void exitGame();
extern void exitCheck();
extern void beginGame();


typedef struct
{
    Sprite *background;
    TextLine filename;
    Window *win;
}MainMenuData;


void onFileLoadCancel(void *Data)
{
    MainMenuData* data;
    if (!Data)return;
    data = Data;
    gfc_line_cpy(data->filename,"saves/");
    return;
}

void onFileLoadOk(void *Data)
{
    MainMenuData* data;
    if (!Data)return;
    data = Data;

    gf2d_window_free(data->win);
    return;
}


void main_menu_start_new_game()
{
    Galaxy *galaxy;
    Empire *empire;
    galaxy = galaxy_generate(2,10);    
    srand(SDL_GetTicks());
    empire = empire_new();
    empire_hud_window(empire,galaxy);
}

int main_menu_free(Window *win)
{
    MainMenuData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    gf2d_sprite_free(data->background);
    free(data);

    return 0;
}

int main_menu_draw(Window *win)
{
    MainMenuData *data;
    if (!win)return 0;
    if (!win->data)return 0;
    data = win->data;
    gf2d_sprite_draw_image(data->background,vector2d(0,0));
    return 0;
}

int main_menu_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    MainMenuData* data;
    if (!win)return 0;
    if (!updateList)return 0;
    data = (MainMenuData*)win->data;
    if (!data)return 0;
    
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 51:
                main_menu_start_new_game();
                gf2d_window_free(win);
                return 1;
            case 71:
                window_text_entry("Enter Save Game to Load", data->filename, win->data, GFCLINELEN, onFileLoadOk,onFileLoadCancel);
                return 1;
            case 81:
                gf2d_window_free(win);
                exitGame();
                return 1;
        }
    }
    return 0;
}


Window *main_menu()
{
    Window *win;
    MainMenuData* data;
    win = gf2d_window_load("menus/main_menu.json");
    if (!win)
    {
        slog("failed to main menu");
        return NULL;
    }
    win->update = main_menu_update;
    win->free_data = main_menu_free;
    win->draw = main_menu_draw;
    data = (MainMenuData*)gfc_allocate_array(sizeof(MainMenuData),1);
    data->background = gf2d_sprite_load_image("images/backgrounds/nebulablue.jpg");
    gfc_line_cpy(data->filename,"saves/");
    win->data = data;
    data->win = win;
    return win;
}


/*eol@eof*/
