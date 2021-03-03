#include <SDL.h>
#include <stdio.h>

#include <simple_logger.h>
#include "gfc_input.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_windows.h"
#include "gf2d_entity.h"
#include "gf2d_font.h"
#include "gf2d_mouse.h"
#include "gf2d_draw.h"
#include "camera.h"
#include "gf2d_space.h"
#include "gf2d_collision.h"
#include "windows_common.h"

static int _done = 0;
static Window *_quit = NULL;

void onCancel(void *data)
{
    _quit = NULL;
}
void onExit(void *data)
{
    _done = 1;
    _quit = NULL;
}

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int i;
    int fullscreen = 0;
    Sprite *background = NULL;
    Space *space = NULL;
    Collision collision;
    CollisionFilter filter= {0};
    int mx,my;
    float mf;
    static Body body[10000];// not a pointer!
    Shape shape[4];// not a pointer!    /*parse args*/
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i],"--fullscreen") == 0)
        {
            fullscreen = 1;
        }
    }
    
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
        fullscreen);
    gf2d_graphics_set_frame_delay(16);
    gfc_audio_init(256,16,4,1,1,1);
    gf2d_sprite_init(1024);
    gf2d_action_list_init(128);
    gf2d_font_init("config/font.cfg");
    gfc_input_init("config/input.cfg");
    gf2d_windows_init(128);
    gf2d_entity_system_init(1024);
    
    camera_set_dimensions(vector2d(1200,700));
    background = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    
    SDL_ShowCursor(SDL_DISABLE);
    // game specific setup
        // init mouse, editor window
    gf2d_mouse_load("actors/mouse.actor");
        space = gf2d_space_new_full(
        3,
        gf2d_rect(0,0,1200,700),
        0.1,
        vector2d(0,0.1),
        1,
        1);
    mf = 0;

    shape[1] = gf2d_shape_circle(0,0, 10);
    shape[2] = gf2d_shape_circle(10,0, 15);
    shape[3] = gf2d_shape_rect(-32,-32,64,64);
    shape[0] = gf2d_shape_rect(-16,-16, 32,32);

    gf2d_space_add_static_shape(space,gf2d_shape_rect(200,500, 512,32));
    gf2d_space_add_static_shape(space,gf2d_shape_rect(610,50, 30,550));
    gf2d_space_add_static_shape(space,gf2d_shape_circle(300,300, 15));
    gf2d_space_add_static_shape(space,gf2d_shape_edge(100,200, 255,360));
    gf2d_space_add_static_shape(space,gf2d_shape_edge(100,400, 255,360));
    gf2d_space_add_static_shape(space,gf2d_shape_edge(100,200, 100,400));
    /* Stress test*/
        gf2d_body_set(
            &body[0],
            "body",
            1,
            0,
            0,
            0,
            vector2d(256,256),
            vector2d(2.3,4.4),
            10,
            1,
            1,  //elasticity
            &shape[0],
            NULL,
            NULL);
        gf2d_space_add_body(space,&body[0]);
    for (i = 1; i < 100;i++)
    {
        gf2d_body_set(
            &body[i],
            "body",
            1,
            0,
            0,
            0,
            vector2d(256 + 128,256 + 128),
            vector2d(2.5 * gfc_crandom(),3 * gfc_crandom()),
            10,
            1,
            1,  //elasticity
            &shape[i%2],
            NULL,
            NULL);
        gf2d_space_add_body(space,&body[i]);
    }
    /*main game loop*/
    filter.worldclip = 1;
    /*main game loop*/
    while(!_done)
    {
        gfc_input_update();
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)
        {
            mf = 0;
        }
        gf2d_windows_update_all();
                
        gf2d_entity_think_all();
        gf2d_mouse_update();
        gf2d_space_update(space);    
        
        collision = gf2d_collision_trace_space(space, vector2d(mx,my), vector2d(600,360) ,filter);
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
                gf2d_sprite_draw_image(background,vector2d(0,0));
                // DRAW WORLD
                gf2d_entity_update_all();
                // Draw entities
                gf2d_space_draw(space,vector2d(0,0));
                if (collision.collided)
                {
                    gf2d_draw_line(vector2d(mx,my),collision.pointOfContact, vector4d(255,0,0,255));            
                }
                else
                {
                    gf2d_draw_line(vector2d(mx,my),vector2d(600,360), vector4d(255,255,0,255));
                }
            //UI elements last
            
            gf2d_font_draw_line_tag("Press F4 to quit!",FT_H1,gfc_color(255,255,255,255), vector2d(0,0));
            
            gf2d_windows_draw_all();
            gf2d_mouse_draw();
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
        if ((gfc_input_command_down("exit"))&&(_quit == NULL))
        {
            _quit = window_yes_no("Exit?",onExit,onCancel,NULL,NULL);
        }
   //     slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
