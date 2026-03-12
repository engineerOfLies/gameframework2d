#ifndef __PARALLAX_H__
#define __PARALLAX_H__

#include "gf2d_sprite.h"

typedef struct
{
    Sprite         *baseLayer;
    GFC_List       *layers;
    GFC_Vector2D    baseSize;
}Parallax;

Parallax *parallax_load(const char *filename);

void parallax_free(Parallax *parallax);

void parallax_draw(Parallax *parallax);

#endif
