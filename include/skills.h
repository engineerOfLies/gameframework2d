#ifndef __SKILLS_H__
#define __SKILLS_H__

#include "gf2d_text.h"
#include "gf2d_list.h"

typedef struct
{
    TextLine    name;
    float       windup;         /**<frames to be in windup state*/
    float       actionTime;     /**<frames to be in action state*/
    float       recover;        /**<frames to be in recovery state before idle again*/
    List    *   animation;      /**<which actor animations to show in the windup, act, and recover*/
    float       staminaCost;    /**<cost of stamina*/
    float       manaCost;
    float       healthCost;     
    float       damageMod;      /**<damage scaling factor based on equipped weapon / spell*/
    List    *   tags;
}Skill;

/**
 * @brief load a list of skills from a json file
 */
void skill_list_load(char *filename);

/**
 * @brief search for a skill by name
 * @param name the skill to search for
 * @returns NULL if not found or the first accurance found
 */
Skill *skill_get_by_name(char *name);

/**
 * @brief check if a skill contains the tag
 * @param skill the skill to check
 * @param check the name of the tag to check for
 * @returns true or false
 */
Uint8 skill_check_tag(Skill *skill, char *check);

#endif
