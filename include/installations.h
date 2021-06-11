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
    RS_Plotted,
    RS_Construction,
    RS_Complete,
    RS_Damaged,
    RS_Destroyed,
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


#endif
