#include "gui.h"
#include "gf2d_shape.h"
#include "gf2d_draw.h"
#include "gf2d_sprite.h"

typedef struct
{
    Sprite *hud;
    float   healthPercent;
    float   shieldPercent;
    float   chargePercent;
    float   energyPercent;
    float   capacitors;
    int     score;
}GUI;

static GUI gui = {0};

void gui_close_hud()
{
    gf2d_sprite_free(gui.hud);
}

void gui_setup_hud()
{
    memset(&gui,0,sizeof(GUI));
    gui.hud = gf2d_sprite_load_image("images/hud.png");
    atexit(gui_close_hud);
}

void gui_draw_percent_bar(Rect rect,float percent,Vector4D fgColor,Vector4D bgColor)
{
    SDL_Rect r;
    r = gf2d_rect_to_sdl_rect(rect);
    gf2d_draw_solid_rect(r,bgColor);
    r = gf2d_rect_to_sdl_rect(gf2d_rect(rect.x,rect.y,(float)rect.w*percent,rect.h));
    gf2d_draw_solid_rect(r,fgColor);
}

void gui_draw_hud()
{
    gf2d_sprite_draw_image(gui.hud,vector2d(0,120));
    gui_draw_percent_bar(gf2d_rect(465,685,350,30),gui.healthPercent,vector4d((1-gui.healthPercent)*255,gui.healthPercent*255,0,255),vector4d(128,0,0,128));
}

void gui_set_health(float health)
{
    gui.healthPercent = health;
}

/*eol@eof*/
