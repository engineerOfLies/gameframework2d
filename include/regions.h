#ifndef __REGIONS_H__
#define __REGIONS_H__

#include "simple_json.h"
#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_text.h"

typedef enum
{
    RS_Plotted,
    RS_Construction,
    RS_Complete,
    RS_Damaged,
    RS_Destroyed
}RegionState;

typedef enum
{
    IT_None,
    IT_Population,
    IT_Agriculture,
    IT_Military,
    IT_Mining,
    IT_Research,
    IT_Cultural,
    IT_Travel,
    IT_University
}InstallationType;

typedef struct
{
    Uint32      minerals;
    Uint32      fertility;
    Uint32      installation;       /**<which development, if any has been installed to this region*/
    Uint32      state;              /**<state of development for the installation*/
    Uint32      strength;           /**<generic score for the strength of the development, if any*/
    Uint32      commanderId;        /**<if the installation has a commander, this is their ID*/
}Region;

Region *region_new();
void    region_free(Region* region);
Region *region_generate(Uint32 id);
Region *region_load_from_json(SJson *json);
SJson  *region_save_to_json(Region *region);

List *region_list_load_from_json(SJson *json);
SJson *region_list_save_to_json(List *regionList);

void region_draw_planet_view(Region *region);
void region_draw_region_view(Region *region);

#endif
