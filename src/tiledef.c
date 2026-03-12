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

int tiledef_save_to_file(TileDef *tiledef, FILE *file)
{
    if ((!tiledef)||(!tiledef->sheet)||(!file))return 0;
    fwrite(tiledef->sheet->filepath,sizeof(GFC_TextLine),1,file);
    fwrite(&tiledef->width,sizeof(Uint32),1,file);
    fwrite(&tiledef->height,sizeof(Uint32),1,file);
    fwrite(&tiledef->fpl,sizeof(Uint32),1,file);
    return 1;
}

TileDef *tiledef_load_from_file(FILE *file)
{
    TileDef *tiledef;
    GFC_TextLine buffer;
    if (!file)return NULL;
    tiledef = tiledef_new();
    if (!tiledef)return NULL;
    fread(buffer,sizeof(GFC_TextLine),1,file);
    fread(&tiledef->width,sizeof(Uint32),1,file);
    fread(&tiledef->height,sizeof(Uint32),1,file);
    fread(&tiledef->fpl,sizeof(Uint32),1,file);
    tiledef->sheet = gf2d_sprite_load_all(
            buffer,
            tiledef->width,
            tiledef->height,
            tiledef->fpl,
            1);
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
