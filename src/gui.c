#include "gui.h"
#include "gf2d_shape.h"
#include "gf2d_draw.h"
#include "gf2d_sprite.h"

typedef struct
{
    Sprite *hud;
    float   healthPercent;
    float   thrustPercent;
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

void gui_draw_percent_bar_horizontal(Rect rect,float percent,Vector4D fgColor,Vector4D bgColor,int left)
{
    SDL_Rect r;
    r = gf2d_rect_to_sdl_rect(rect);
    gf2d_draw_solid_rect(r,bgColor);
    if (left)
    {
        r = gf2d_rect_to_sdl_rect(gf2d_rect(rect.x,rect.y,(float)rect.w*percent,rect.h));
    }
    else
    {
        r = gf2d_rect_to_sdl_rect(gf2d_rect(rect.x+(1-percent)*rect.w,rect.y,(float)rect.w*percent,rect.h));
    }
    gf2d_draw_solid_rect(r,fgColor);
}

void gui_draw_percent_bar_vertical(Rect rect,float percent,Vector4D fgColor,Vector4D bgColor,int top)
{
    SDL_Rect r;
    r = gf2d_rect_to_sdl_rect(rect);
    gf2d_draw_solid_rect(r,bgColor);
    if (top)
    {
        r = gf2d_rect_to_sdl_rect(gf2d_rect(rect.x,rect.y+(1-percent)*rect.h,rect.w,rect.h*percent));
    }
    else
    {
        r = gf2d_rect_to_sdl_rect(gf2d_rect(rect.x,rect.y,rect.w,rect.h*percent));
    }
    gf2d_draw_solid_rect(r,fgColor);
}

void gui_draw_hud()
{
    gf2d_sprite_draw_image(gui.hud,vector2d(0,100));
    gui_draw_percent_bar_horizontal(gf2d_rect(495,680,145,10),gui.healthPercent,vector4d((1-gui.healthPercent) * 255, gui.healthPercent*255, 0, 255), vector4d(128, 0, 0, 128),0);
    gui_draw_percent_bar_horizontal(gf2d_rect(640,680,145,10),gui.shieldPercent,vector4d(0, 0, 255, 255), vector4d(128, 0, 0, 128),1);
    gui_draw_percent_bar_vertical(gf2d_rect(475,680,15,30),gui.thrustPercent,vector4d(0, 255, 255, 255), vector4d(128, 0, 0, 128),1);
    gui_draw_percent_bar_vertical(gf2d_rect(790,680,15,30),gui.chargePercent,vector4d(128+(127*gui.chargePercent), 128 - (127*gui.chargePercent), 128 - (127*gui.chargePercent), 255), vector4d(128, 0, 0, 128),1);
    gf2d_text_draw_line("Health",FT_Small,gf2d_color8(0,255,0,255), vector2d(495,665));
    gf2d_text_draw_line("Shields",FT_Small,gf2d_color8(0,0,255,255), vector2d(672,665));
}

void gui_set_health(float health)
{
    gui.healthPercent = health;
}

void gui_set_energy(float energy)
{
    gui.energyPercent = energy;
}

void gui_set_charge(float charge)
{
    gui.chargePercent = charge;
}

void gui_set_thrust(float thrust)
{
    gui.thrustPercent = thrust;
}


/*eol@eof*/
