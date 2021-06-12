#ifndef __INSTALLATIONS_H__
#define __INSTALLATIONS_H__

#include <simple_json.h>

#include "gfc_text.h"

#include "gf2d_actor.h"

#include "empire.h"
#include "regions.h"

typedef enum
{
    IT_None,
    IT_Population,
    IT_Agriculture,
    IT_Mining,
    IT_Military,
    IT_Refinery,
    IT_Research,
    IT_Cultural,
    IT_Travel,
    IT_University,
    IT_MAX
}InstallationType;

typedef enum
{
    RS_Empty,
    RS_Plotted,         // initially plotted, but construction hasn't begun yet
    RS_Construction,    // under construction, no upkeep
    RS_Operational,     // Up and running, upkeep and production at full capacity
    RS_Starved,         // not enough resources to operate.  Will not come back online until resources are available. 
    RS_Offline,         // taken offline by the player/ will no longer produce or cost upkeep until returned to operational
    RS_Damaged,         // Needs to be repaired before it can produce again. It will still cost upkeep.  Can be repaired to return to operational / Destroyed to remove
    RS_Destroyed,       // Dead, must be removed to rebuild in this spot
    RS_MAX
}InstallationState;


typedef struct
{
    Uint32              id;             /**<this installation's id*/
    TextLine            name;           /**<name of installation*/
    Empire             *empire;         /**<who owns this*/
    InstallationType    iType;          /**<which type of installation this is*/
    InstallationState   iState;         /**<current installation state*/
    int                 startTime;      /**<gametime when this was built*/
    Actor               actor;          /**<drawing*/
    Vector2D            position;       /**<draw position*/
    Uint32              personnel;      /**<number of staff assigned to this outpost could be a factor in output/upkeep*/
    EmpireResources     upkeep;         /**<costs per update for the installation to keep running*/
    EmpireResources     production;     /**<ammount of resources produced per update, assuming upkeep went smoothly*/
    Region             *region;         /**<region where this installation was built*/
}Installation;


/**
 * @brief initialize the installation system
 */
void installations_init();

/**
 * @brief get an initialized installation
 * @return NULL on error or a zero'd installation pointer
 */
Installation *installation_new();

/**
 * @brief free up an installation
 */
void installation_free(Installation *inst);

/**
 * @brief create a new installation and configure it based on the passed in json
 */
Installation *installation_create_from_config(SJson *config,Vector2D position,Region *region,Empire *empire);

/**
 * @brief created a new installation and configures it based on the installation type
 */
Installation *installation_create_by_type(InstallationType iType,Vector2D position,Region *region,Empire *empire);

/**
 * @brief draw an installation to the screen with the given offset
 */
void installation_draw(Installation *inst,Vector2D offset);

/**
 * @brief update the given installations, running upkeep and production
 */
void installation_update(Installation *inst);


#endif
