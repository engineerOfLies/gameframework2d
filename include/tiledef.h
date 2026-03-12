#ifndef __TILEDEF_H__
#define __TILEDEF_H__

#include <stdio.h>

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

/**
 * @brief save to an open file
 * @return 0 if this doesn't do anything, 1 otherwise
 */
int tiledef_save_to_file(TileDef *tiledef, FILE *file);

TileDef *tiledef_load_from_file(FILE *file);



#endif
