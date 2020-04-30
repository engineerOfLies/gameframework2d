#ifndef __CONDITIONS_H__
#define __CONDITIONS_H__

#include "simple_json.h"

#include "gf2d_entity.h"

/**
 * @purpose evaluate conditions present in config files
 */

/**
 * @brief evaluate a condition from config
 * @param json the config condition to evaluate
 * @param player the player to evaluate the conditions of
 * @return 0 on false, 1 on true
 */
int condition_evaluate(SJson *json,Entity *player);

#endif

