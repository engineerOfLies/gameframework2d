#ifndef __PATH_H__
#define __PATH_H__

#include "gfc_list.h"

/**
 * @brief given a tilemap where a value of 1 is a traversable tile, get a path from start to end
 * @param tilemap the map of tiles to search
 * @param w the width of the tile map
 * @param h the height of the tile map
 * @note if w and h are innacruate, we are GOING to crash
 * @param start the start point of the search
 * @param end the goal of the search
 * @return NULL on error or if the end point is not reachable from the start point, a list of Vector2Ds otherwise, where start should be represented in the 0 spot
 */
List *path_find(Uint8 *tilemap,Uint32 w,Uint32 h,Vector2D start, Vector2D end);

/**
 * @brief visualize a path in 2D
 * @param path the list of Vector2Ds
 */
void path_draw(List *path,Vector2D offset);

/**
 * @brief free a previously created path
 * @param path the path to free
 */
void path_free(List *path);

#endif
