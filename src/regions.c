#include <simple_logger.h>
#include "regions.h"

Region *region_new()
{
    Region *region;
    region = gfc_allocate_array(sizeof(Region),1);
    if (!region)
    {
        slog("failed to allocate memory for a new region");
        return NULL;
    }
    return region;
}


void region_free(Region* region)
{
    if (!region)return;
    free(region);
}

Region *region_generate(Uint32 id);
Region *region_load_from_json(SJson *json);
SJson  *region_save_to_json(Region *region);

List *region_list_load_from_json(SJson *json);
SJson *region_list_save_to_json(List *regionList);

void region_draw_planet_view(Region *region);
void region_draw_region_view(Region *region);

/*END OF THE FILE*/
