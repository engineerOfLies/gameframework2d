#ifndef __EMPIRE_H__
#define __EMPIRE_H__

#include "gfc_list.h"
#include "gfc_text.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "systems.h"
#include "planet.h"

typedef struct
{
    Uint32      id;             /**<unique id for the empire*/
    TextLine    leaderName;     /**<leader of the empire's name*/
    TextLine    empireName;     /**<name of the player's empire*/
    System     *homeSystem;     /**<this empire's home system*/
    Planet     *homeWorld;      /**<seat of power for this empire*/
    List       *systems;        /**<list of systems controlled by this empire*/
    List       *planets;        /**<list of planets developed by this empire*/
    List       *armada;         /**<list of aramda controlled by this empire*/
    List       *ships;          /**<list of ships controlled by this empire*/
    List       *technologies;   /**<list of technologies developed by this empire*/
    Uint32      minerals;       /**<how many minerals are at the empire's disposal*/
    Uint32      population;     /**<how many workers are at the empire's disposal*/
    Uint32      agriculture;    /**<how much food has been produced by the empire*/
}Empire;

/**
 * @brief initialized internal management for the empire system
 */
void empire_init();

Empire *empire_new();
void empire_free(Empire *empire);

void empire_give_system(Empire *empire,System *system);
void empire_give_planet(Empire *empire,Planet *planet);
void empire_set_home_system(Empire *empire,System *system);
void empire_set_home_planet(Empire *empire,Planet *planet);

Empire *empire_load();

#endif