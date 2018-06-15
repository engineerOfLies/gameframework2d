#ifndef __EQUIPMENT_H__
#define __EQUIPMENT_H__

#include "gf2d_text.h"
#include "combat.h"

typedef struct
{
    TextLine    name;
    TextLine    combatActions[CA_MAX];
    TextLine    icon;
    TextLine    model;
    float       staminaFactor;
    float       manaFactor;
    float       speedFactor;
    int         handedness;
    float       weight;
}Equipment;

void equipment_list_load(const char *filename);
Equipment *equipment_get_by_name(const char *name);

#endif
