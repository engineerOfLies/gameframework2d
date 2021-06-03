#ifndef __EMPIRE_H__
#define __EMPIRE_H__

#include "simple_json.h"

#include "gfc_list.h"
#include "gfc_text.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "regions.h"
#include "systems.h"
#include "planet.h"
#include "galaxy.h"

typedef struct
{
    Uint32      id;             /**<unique id for the empire*/
    Uint64      gameTime;       /**<time for the empire*/
    TextLine    leaderName;     /**<leader of the empire's name*/
    TextLine    empireName;     /**<name of the player's empire*/
    Color       empireColor;    /**<highlight color for this empire*/
    Galaxy     *galaxy;         /**<the galaxy to rule*/
    System     *homeSystem;     /**<this empire's home system*/
    Planet     *homeWorld;      /**<seat of power for this empire*/
    List       *systems;        /**<list of systems controlled by this empire*/
    List       *planets;        /**<list of planets developed by this empire*/
    List       *armada;         /**<list of aramda controlled by this empire*/
    List       *ships;          /**<list of ships controlled by this empire*/
    List       *technologies;   /**<list of technologies developed by this empire*/
    Sint32      approval;       /**<how well liked you are by your people*/
    Sint32      credits;        /**<how many credits are at the empire's disposal*/
    Sint32      minerals;       /**<how many minerals are at the empire's disposal*/
    Uint32      population;     /**<how many workers are at the empire's disposal*/
    Sint32      agriculture;    /**<how much food has been produced by the empire*/
    SJson      *progress;       /**<keeps track of updates throughout the galaxy*/
}Empire;

/**
 * @brief initialized internal management for the empire system
 */
void empire_init();

/**
 * @brief allocate a blank empire
 * @return NULL on erorr or an empire handle
 */
Empire *empire_new();

/**
 * @brief setup an empire for the first time, when starting a new game
 * @param empire the empire to setup
 */
void empire_setup(Empire *empire);

/**
 * @brief get an empire by its id
 * @param id the id to search for
 * @return NULL on not found, or the empire in question
 */
Empire *empire_get_by_id(Uint32 id);

/**
 * @brief perform all upkeep needed for an empire
 * @param empire the one in question
 */
void empire_update(Empire *empire);
void empire_free(Empire *empire);

void empire_give_system(Empire *empire,System *system);
void empire_give_planet(Empire *empire,Planet *planet);
void empire_set_home_system(Empire *empire,System *system);
void empire_set_home_planet(Empire *empire,Planet *planet);

Empire *empire_load();


SurveyState empire_region_get_survey_state(Empire *empire,Uint32 regionId,SurveyType surveyType);
int empire_survery_region(Empire *empire,Uint32 regionId,SurveyType surveyType);

/**
 * @brief get the empire's current credits
 * @param empire the empire
 * @return 0 if error (but that could be valid too) or the amount of credits (or debt) for the empire
 */
int empire_get_credits(Empire *empire);

/**
 * @brief change the empire's credits by the amount provided
 * @param empire the empire;
 * @param credits the amount to change it by (yes even negative)
 */
int empire_change_credits(Empire *empire,int credits);

#endif
