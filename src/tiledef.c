#include "simple_logger.h"

#include "tiledef.h"

TileDef *tiledef_new()
{
    TileDef *tiledef;
    tiledef = gfc_allocate_array(sizeof(TileDef),1);
    if (!tiledef)return NULL;
    //defaul init stuff
    return tiledef;
}


void tiledef_free(TileDef *tiledef)
{
    if (!tiledef)return;
    
    //do other freeing
    free(tiledef);
}

TileDef *tiledef_parse(SJson *config)
{
    const char *str;
    TileDef *tiledef;
    if (!config)return NULL;
    tiledef = tiledef_new();

    sj_object_get_uint32(config,"width",&tiledef->width);
    sj_object_get_uint32(config,"height",&tiledef->height);
    sj_object_get_uint32(config,"fpl",&tiledef->fpl);
    str = sj_object_get_string(config,"sheet");
    if (str)
    {
        tiledef->sheet = gf2d_sprite_load_all(
            str,
            tiledef->width,
            tiledef->height,
            tiledef->fpl,
            1);
    }
    slog("we loaded the tile sheet %s",str);
    return tiledef;
}

/*eol@eof*/
