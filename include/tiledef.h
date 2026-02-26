#ifndef __TILEDEF_H__
#define __TILEDEF_H__

#include "simple_json.h"

#include "gf2d_sprite.h"

typedef struct
{
    Sprite *sheet;
    Uint32 width,height,fpl;
}TileDef;


void tiledef_free(TileDef *tiledef);

/**
 * @brief parse json into a tiledef
 * 
 */
TileDef *tiledef_parse(SJson *config);

#endif
