#ifndef __GALAXY_H__
#define __GALAXY_H__

#include "simple_json.h"
#include "gfc_types.h"
#include "gfc_list.h"
#include "gfc_text.h"
#include "gfc_vector.h"
#include "systems.h"

typedef struct
{
    Uint32  seed;
    Uint32  idPool;
    List *systemList;
}Galaxy;

void galaxy_init();

Galaxy *galaxy_new();

/**
 * @brief generate a galaxy based on the provided seed
 * @param seed the seed to use to generate the galaxy
 * @param count the number of star systems to generate
 * @return NULL on error or a pointer to the generated galaxy
 */
Galaxy *galaxy_generate(Uint32 seed, Uint32 count);
void    galaxy_free(Galaxy *galaxy);
Galaxy *galaxy_load_from_json(SJson *json);
SJson  *galaxy_save_to_json(Galaxy *galaxy);

/**
 * @brief draw the galaxy
 * @param galaxy the galaxy to draw
 * @param offset with this offset (camera stuff)
 * @param scale with this scale
 */
void galaxy_draw(Galaxy *galaxy,Vector2D offset,float scale);

/**
 * @brief perform an iterative search through the systems for one within range of the provided system
 * @param galaxy the galaxy to search
 * @param from search from this point forward.  Provide a NULL to start a fresh search
 * @param ignore skip this system
 * @param position relative to this position
 * @param radius within this range to the position
 * @return NULL when no more systems qualify, or a system pointer when we have a match.  Feed the result back into this function to keep searching
 */
System *galaxy_get_next_system_in_range(Galaxy *galaxy, System *from,System *ignore,Vector2D position,float radius);
System *galaxy_get_nearest_system(Galaxy *galaxy,System *ignore,Vector2D position,float radius);

/**
 * @brief generate a star system with the id and provided seed
 */
System *system_generate(Galaxy *galaxy, Uint32 id,Uint32 seed);

// convert to and from screen / galaxy position space
Vector2D galaxy_position_to_screen_position(Vector2D position);
Vector2D galaxy_position_from_screen_position(Vector2D position);

#endif
