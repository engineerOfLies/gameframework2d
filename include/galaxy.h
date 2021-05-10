#ifndef __GALAXY_H__
#define __GALAXY_H__

#include "simple_json.h"
#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_text.h"

typedef struct
{
    Uint32  seed;
    Uint32  idPool;
    List *systemList;
}Galaxy;

Galaxy *galaxy_new();
Galaxy *galaxy_generate(Uint32 seed);
void    galaxy_free(Galaxy *galaxy);
Galaxy *galaxy_load_from_json(SJson *json);
SJson  *galaxy_save_to_json(Galaxy *galaxy);
void    galaxy_draw(Galaxy *galaxy);

#endif
