#ifndef __GF2D_ACTOR_H__
#define __GF2D_ACTOR_H__

#include <SDL.h>

#include "gfc_text.h"
#include "gfc_vector.h"

#include "gf2d_sprite.h"


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
    TextLine    name;
    int         startFrame;
    int         endFrame;
    float       frameRate;
    ActionType  type;
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
    Vector2D    drawOffset;
}ActionList;

typedef struct
{
    int         _inuse;       /**<set if the actor is in use*/
    Sprite     *sprite;       /**<which sprite to draw this entity with*/
    Vector4D    color;        /**<color to shift sprite too*/
    float       frame;        /**<current frame for the sprite*/
    Vector2D    size;         /**<size of the actor (sprite size times scale*/
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
 * @returns true on success or false on failure
 */
int gf2d_actor_load(Actor *actor,char *file);

/**
 * @brief free all assets loaded for the actor and set its data to 0
 * @param actor a pointer to the actor to free
 */
void gf2d_actor_free(Actor *actor);

/**
 * @brief draw an actor to screen using its current frame
 * @param actor the actor to draw
 * @param position where to draw it
 * @param scale (optional) if provided this factor is applies
 * @param scaleCenter (optional) if provided this center is used for scaling
 * @param rotation (option) if provided the actor is rotated
 * @param flip (optional) if provided the actor is flipped horizontally or vertically
 */
void gf2d_actor_draw(
    Actor *actor,
    Vector2D position,
    Vector2D * scale,
    Vector2D * scaleCenter,
    Vector3D * rotation,
    Vector2D * flip
);

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

/**
 * @brief change the action to the next action in the list
 * @note loops
 * @param actor the actor to change
 */
void gf2d_actor_next_action(Actor *actor);

/**
 * @brief change the action to the previous action in the list
 * @note loops
 * @param actor the actor to change
 */
void gf2d_actor_prev_action(Actor *actor);

/**
 * @brief get the name of the current action for the actor
 * @param actor the actor to get the current action for
 * @return NULL on error or not set, a pointer to the name otherwise
 */
const char *gf2d_actor_get_action_name(Actor *actor);

/**
 * @brief get the current action for the given actor
 * @param actor the actor to get the action for
 * @return NULL on error or not set, a pointer to the action data otherwise
 */
Action *gf2d_actor_get_current_action(Actor *actor);

/**
 * @brief return the number of animation frames (not sprite frames) until the action completes
 */
int gf2d_actor_get_frames_remaining(Actor *actor);
#endif
