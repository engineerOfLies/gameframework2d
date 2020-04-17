#include <SDL.h>

#include "gf2d_actor.h"
#include "gf2d_mouse.h"

#include "scene.h"


typedef struct
{
    Uint32 buttons;     /**<buttons mask*/
    Vector2D position;  /**<position of mouse*/
}MouseState;

typedef struct
{
    MouseState mouse[2]; /**<mouse state for the current and last frames*/
    Actor actor;         /**<mouse actor*/
    MouseFunction mf;    /**<current mouse state*/
    Bool    itemSet;     /**<if set, allow cycling through itemActor*/
    Bool    spellSet;     /**<if set, allow cycling through spellActor*/
    Actor itemActor;    /**<use this actor when drawing an item selection*/
    Actor spellActor;   /**<use this actor when drawing a spell selection*/
}Mouse;

static Mouse _mouse = {0};

void gf2d_mouse_set_item_actor(char *actor)
{
    if (!actor)return;
    if (_mouse.itemActor.al)
    {
        // already an actor loaded
        if (gfc_line_cmp(actor,_mouse.itemActor.al->filename)==0)return;//already the correct actor
        gf2d_actor_free(&_mouse.itemActor);
    }
    gf2d_actor_load(&_mouse.itemActor,actor);
}

void gf2d_mouse_set_action(char *action)
{
    gf2d_actor_set_action(&_mouse.actor,action);
}

void gf2d_mouse_set_spell_actor(char *actor)
{
    if (!actor)return;
    if (_mouse.spellActor.al)
    {
        // already an actor loaded
        if (gfc_line_cmp(actor,_mouse.spellActor.al->filename)==0)return;//already the correct actor
        gf2d_actor_free(&_mouse.spellActor);
    }
    gf2d_actor_load(&_mouse.spellActor,actor);
}

void gf2d_mouse_load(char *actorFile)
{
    gf2d_actor_free(&_mouse.actor);
    gf2d_actor_load(&_mouse.actor,actorFile);
}

void gf2d_mouse_set_item_action(char *action)
{
    if ((!action)||(strlen(action)))
    {
        gf2d_mouse_set_function(MF_Walk);
        _mouse.itemSet = 0;
    }
    _mouse.itemSet = 1;
    gf2d_actor_set_action(&_mouse.itemActor,action);
    gf2d_mouse_set_function(MF_Item);
}

void gf2d_mouse_set_spell_action(char *action)
{
    if (!action)
    {
        gf2d_mouse_set_function(MF_Walk);
        _mouse.spellSet = 0;
    }
    _mouse.spellSet = 1;
    gf2d_actor_set_action(&_mouse.spellActor,action);
    gf2d_mouse_set_function(MF_Spell);
}

void gf2d_mouse_set_function(MouseFunction mf)
{
    if (mf == MF_Max)
    {
        mf = MF_Pointer;
    }
    _mouse.mf = mf;
    switch (mf)
    {
        case MF_Max:
        case MF_Pointer:
            gf2d_mouse_set_action("pointer");
            break;
        case MF_Walk:
            gf2d_mouse_set_action("walk");
            break;
        case MF_Look:
            gf2d_mouse_set_action("look");
            break;
        case MF_Interact:
            gf2d_mouse_set_action("interact");
            break;
        case MF_Talk:
            gf2d_mouse_set_action("talk");
            break;
        case MF_Item:
            gf2d_mouse_set_action("item");
            break;
        case MF_Spell:
            gf2d_mouse_set_action("spell");
            break;
    }
}

void gf2d_mouse_scene_update()
{
    if((gf2d_mouse_button_released(2))&&(_mouse.mf != MF_Pointer))
    {
        _mouse.mf++;
        if ((_mouse.mf == MF_Item)&&(!_mouse.itemSet))
        {
            _mouse.mf++;
        }
        if ((_mouse.mf == MF_Spell)&&(!_mouse.spellSet))
        {
            _mouse.mf++;
        }
        if (_mouse.mf >= MF_Max)
        {
            _mouse.mf = MF_Walk;
        }
        gf2d_mouse_set_function(_mouse.mf);
        scene_set_mouse_function(scene_get_active(),_mouse.mf);
    }
}

void gf2d_mouse_update()
{
    int x,y;
    gf2d_actor_next_frame(&_mouse.actor);
    memcpy(&_mouse.mouse[1],&_mouse.mouse[0],sizeof(MouseState));
    _mouse.mouse[0].buttons = SDL_GetMouseState(&x,&y);
    vector2d_set(_mouse.mouse[0].position,x,y);
}

void gf2d_mouse_draw()
{
    switch (_mouse.mf)
    {
        case MF_Spell:
            gf2d_actor_draw(
            &_mouse.spellActor,
            _mouse.mouse[0].position,
            NULL,
            NULL,
            NULL,
            NULL);
            break;
        case MF_Item:
            gf2d_actor_draw(
            &_mouse.itemActor,
            _mouse.mouse[0].position,
            NULL,
            NULL,
            NULL,
            NULL);
            break;
        default:
            gf2d_actor_draw(
            &_mouse.actor,
            _mouse.mouse[0].position,
            NULL,
            NULL,
            NULL,
            NULL);
    }
}

int gf2d_mouse_moved()
{
    if ((_mouse.mouse[0].position.x != _mouse.mouse[1].position.x) ||
        (_mouse.mouse[0].position.y != _mouse.mouse[1].position.y) ||
        (_mouse.mouse[0].buttons != _mouse.mouse[1].buttons))
    {
        return 1;
    }
    return 0;
}

int gf2d_mouse_button_pressed(int button)
{
    int mask;
    mask = 1 << button;
    if ((_mouse.mouse[0].buttons & mask) &&
        !(_mouse.mouse[1].buttons & mask))
    {
        return 1;
    }
    return 0;
}

int gf2d_mouse_button_held(int button)
{
    int mask;
    mask = 1 << button;
    if ((_mouse.mouse[0].buttons & mask) &&
        (_mouse.mouse[1].buttons & mask))
    {
        return 1;
    }
    return 0;
}

int gf2d_mouse_button_released(int button)
{
    int mask;
    mask = 1 << button;
    if (!(_mouse.mouse[0].buttons & mask) &&
        (_mouse.mouse[1].buttons & mask))
    {
        return 1;
    }
    return 0;
}

int gf2d_mouse_button_state(int button)
{
    int mask;
    mask = 1 << button;
    return (_mouse.mouse[0].buttons & mask);
}

Vector2D gf2d_mouse_get_position()
{
    return _mouse.mouse[0].position;
}

Vector2D gf2d_mouse_get_movement()
{
    Vector2D dif;
    vector2d_sub(dif,_mouse.mouse[0].position,_mouse.mouse[1].position);
    return dif;
}

int gf2d_mouse_in_rect(Rect r)
{
    return gf2d_point_in_rect(_mouse.mouse[0].position,r);
}

MouseFunction gf2d_mouse_get_function()
{
    return _mouse.mf;
}
/*eol@eof*/
