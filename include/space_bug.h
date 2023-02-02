#ifndef __SPACE_BUG_H__
#define __SPACE_BUG_H__

#include "gfc_vector.h"
#include "entity.h"

/**
 * @brief spawn a new space bug at the given location
 * @param position where to spawn the bug at
 * @return NULL on failure, or a pointer to the new bug
 */
Entity *space_bug_new(Vector2D position);


#endif
