#ifndef __KEEP_H__
#define __KEEP_H__

#include "gf2d_entity.h"

typedef enum
{
    KD_South,
    KD_SouthWest,
    KD_West,
    KD_NorthWest,
    KD_North,
    KD_NorthEast,
    KD_East,
    KD_SouthEast,
    KD_MAX
}KeepDirection;

typedef enum
{
    KS_Base,
    KS_Start,
    KS_Half,
    KS_Nearly,
    KS_Finished,
    KS_Damaged,
    KS_MAX
}KeepStage;

Entity *keep_segment_new(Vector2D position,const char *segment,KeepStage stage, KeepDirection direction);

char *keep_get_direction_name(KeepDirection direction);
char *keep_get_state_name(KeepStage stage);

#endif
