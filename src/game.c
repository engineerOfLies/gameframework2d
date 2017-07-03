#include <SDL.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_list.h"
#include "gf2d_audio.h"
#include "gf2d_windows.h"
#include "simple_logger.h"
#include "simple_json.h"

void add_menu_item(Element *menu,char *text,int id)
{
    Element *e;
    Element *l;
    l = gf2d_element_new_full(
        id,
        text,
        gf2d_rect(0,0,menu->bounds.w,32),
        gf2d_color8(255,255,255,255),
        0);
    gf2d_element_make_list(l,gf2d_element_list_new_full(vector2d(64,32),LS_Horizontal,0,0));
    
    e = gf2d_element_new_full(
        0,
        "bullet",
        gf2d_rect(0,-16,64,32),
        gf2d_color8(255,255,255,255),
        0);
    gf2d_element_make_actor(e,gf2d_element_actor_new_full("actors/charge_bolt.actor"));
    gf2d_element_list_add_item(l,e);
    
    e = gf2d_element_new();
    gf2d_element_make_label(e,gf2d_element_label_new_full(text,gf2d_color8(255,255,255,255),FT_H3,0));
    gf2d_element_list_add_item(l,e);

    gf2d_element_list_add_item(menu,l);
}

void main_menu()
{
    Element *e;
    Element *l;
    SJson *json = NULL,*window = NULL;
    Window *win;
    Vector4D color;

//    json = sj_load("config/testfile.json");
//    json = sj_load("config/testwindow.min");
//     window = sj_object_get_value(json,"window");
//     if (window)
//     {
//         slog("window loaded");
//     }
//     else
//     {
//         slog("window failed to load:\n%s",sj_get_error());
//     }
    win = gf2d_window_new();
    win->color = vector4d(0,255,100,255);
    win->dimensions = gf2d_rect(200,200,800,400);
    
    l = gf2d_element_new_full(
        0,
        "list",
        gf2d_rect(10,10,win->dimensions.w-20,win->dimensions.h-20),
        gf2d_color8(255,255,255,255),
        0);
    gf2d_element_make_list(l,gf2d_element_list_new_full(vector2d(100,32),LS_Vertical,0,0));
    gf2d_window_add_element(win,l);

    e = gf2d_element_new();
    gf2d_element_make_label(e,gf2d_element_label_new_full("Window Heading",gf2d_color8(255,255,255,255),FT_H1,0));
    gf2d_element_list_add_item(l,e);

    add_menu_item(l,"New Game",0);
    add_menu_item(l,"Continue",1);
    add_menu_item(l,"Load Game",2);
    add_menu_item(l,"Quit",3);
    sj_free(json);
}

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Sprite *sprite;
    
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Vector4D mouseColor = {255,100,255,200};
    
    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_audio_init(256,16,4,1,1,1);
    gf2d_sprite_init(1024);
    gf2d_action_list_init(128);
    gf2d_text_init("config/font.cfg");
    gf2d_windows_init(128);
    SDL_ShowCursor(SDL_DISABLE);
    
    /*demo setup*/
    sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16);
    main_menu();
    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        gf2d_windows_update_all();
        
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            gf2d_sprite_draw_image(sprite,vector2d(0,0));
            
            //UI elements last
            gf2d_windows_draw_all();
            gf2d_sprite_draw(
                mouse,
                vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseColor,
                (int)mf);
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
   //     slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
