#ifndef __PARTICLE_EFFECTS_H__
#define __PARTICLE_EFFECTS_H__

#include "gf2d_vector.h"
#include "gf2d_color.h"

/**
 * @brief make a particle spray that follows gravity
 * @param position starting from this point
 * @param direction moving in this direction
 * @param count making this many
 */
void particle_spray(Vector2D position, Vector2D direction,Color color, Uint32 count);

/**
 * @brief make a trail from a shape
 */
void particle_trail(Vector2D start, Vector2D end,Color color);

#endif
