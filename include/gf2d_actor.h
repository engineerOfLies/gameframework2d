#ifndef __GF2D_ACTOR_H__
#define __GF2D_ACTOR_H__

#include "gf2d_sprite.h"
#include "gf2d_text.h"
#include "gf2d_vector.h"

#include <SDL.h>

typedef enum
{
    AT_NONE,
    AT_LOOP,
    AT_PASS
}ActionType;

typedef enum
{
    ART_ERROR,
    ART_NORMAL,
    ART_END,
    ART_START
}ActionReturnType;

/**
 * @brief this structure describes an individual action
 */
typedef struct Action_S
{
    TextLine name;
    int startFrame;
    int endFrame;
    float frameRate;
    ActionType type;
}Action;

/**
 * @brief this structure is of an action list, common to any actor using this list
 */
typedef struct
{
    Uint32      ref_count;
    TextLine    filename;    
    Action     *actions;
    int         numActions;
    TextLine    sprite;
    int         frameWidth;
    int         frameHeight;
    int         framesPerLine;
    Vector2D    scale;        /**<scale to draw at*/
    Vector4D    color;
    Vector4D    colorSpecial;
}ActionList;

typedef struct
{
    Sprite     *sprite;       /**<which sprite to draw this entity with*/
    Vector4D    color;        /**<color to shift sprite too*/
    float       frame;        /**<current frame for the sprite*/
    ActionList *al;           /**<action list for managing sprite animations*/
    TextLine    action;       /**<the current action*/
    ActionReturnType at;      /**<set automatically each frame*/
}Actor;

/**
 * @brief initialize the action list system
 */
void gf2d_action_list_init(Uint32 max);

/**
 * @brief load an action list from file or get a reference to one already in memory
 * @param filename the name of the action list to load
 * @return NULL on error or a pointer to the action list
 */
ActionList *gf2d_action_list_load(
    char *filename
);

/**
 * @brief let the system know you are done with the action list
 * @param actionList the list to free
 */
void gf2d_action_list_free(ActionList *actionList);

/**
 * @brief given an action, set the next frame for an actor
 * @param al the action list to use to determine the next frame
 * @param frame the current frame, will be changed to the next frame
 * @param name the name of the action to to by.  If its a changed action, this will set the frame to the starting frame
 * @return the Action Return Type of this action.  Lets you know how to react to the update
 */
ActionReturnType gf2d_action_list_get_next_frame(
    ActionList *al,
    float * frame,
    char *name);

/**
 * @brief get the starting frame of an action
 * @param al the action list to set the frame by
 * @param name the action to set the frame to
 * @return the frame
 */
float gf2d_action_set(ActionList *al,char *name);

/**
 * @brief load actor information from file into the actor provided
 * @param actor a pointer to the actor to populate
 * @param file the file path to load
 */
void gf2d_actor_load(Actor *actor,char *file);

/**
 * @brief free all assets loaded for the actor and set its data to 0
 * @param actor a pointer to the actor to free
 */
void gf2d_actor_free(Actor *actor);


/**
 * @brief set the action and starting frame for an actor
 * @param actor the actor to set
 * @param action the action to set it to
 */
void gf2d_actor_set_action(Actor *actor,char *action);

/**
 * @brief based on the actor's current action set the next frame and return type
 */
void gf2d_actor_next_frame(Actor *actor);
#endif
