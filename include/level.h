#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "simple_json.h"

#include "gfc_audio.h"

#include "gf2d_space.h"
#include "gf2d_sprite.h"
#include "gf2d_entity.h"
#include "gf2d_particles.h"


/**
 * @Purpose game specific level code
 * This game is going to be a tile based side scrolling 2d platformer
 */

/**
 * @brief The level structure holds data for a given game level.
 */
typedef struct
{
    TextLine    filename;
    List       *backgroundImages;   /**<filenames for the background images*/
    TextLine    backgroundMusic;    /**<background music for this level*/
    TextLine    tileSet;            /**<tiles to use for this level*/
    Vector2D    tileMapSize;        /**<how many tiles there are in width & height*/
    Vector2D    levelBounds;        /**<screen size of the level*/
    int        *tileMap;            /**<tile data*/
    Uint32      idPool;             /**<used to make sure all spawns have a unique ID*/
    SJson      *spawnList;          /**<list of spawn candidates for the level*/
    SJson      *args;               /**<level specific arguments*/
    Uint8       dark;               /**<if true, the level is drawn with darkness mask*/
    Color       darkLevel;
    Uint32      waterLevel;         /**<if not zero, this will be which water sprite to use*/
}LevelInfo;

/**
 * @brief clears all assets loaded for the live level
 * Called at program exit and before loading the next level
 */
void level_clear();

/**
 * @brief draw the current level
 */
void level_draw();

/**
 * @brief just render the background parallax images
 */
void level_draw_background();


/**
 * @brief just render the tile layer
 */
void level_draw_tiles(Vector2D offset);

/**
 * @brief get the sprite image holding the tile layer
 * @return NULL if not set, or the layer sprite
 */
Sprite *level_get_tile_layer();

/**
 * @brief get the size of a tile for the level
 * @return (0,0) if something is wrong, of the dimension
 */
Vector2D level_get_tile_size();

/**
 * @brief change the size of the given level and re-initialize it
 * @param linfo the level to resize
 * @param newSize the new size
 */
void level_resize(LevelInfo *linfo,Vector2D newSize);

/**
 * @brief snap to the top left corner of tile at the position
 * @param position the position to snap
 * @return -1,-1 on error or the tile position
 */
Vector2D level_snape_to_tile(Vector2D position);


/**
 * @brief check if the given position is in the level water
 * @param position the point to check
 * @return 0 if not, 1 if it is
 */
int level_water_check(Vector2D position);


/**
 * @brief draw all the spawn candidates for the level info
 * @param linfo the level info to draw from
 * @note: honors camera
 */
void level_draw_spawns(LevelInfo *linfo);

/**
 * @brief draw an outline of the given tile
 * @note should be called AFTER level_draw()
 * @param linfo the level info to draw based on
 * @param tile the tile to draw (in tile space)
 * @param color what color to draw the highlight in
 */
void level_draw_outline_tile(LevelInfo *linfo,Vector2D tile,Color color);

/**
 * @brief update the current level
 */
void level_update();

/**
 * @brief create the live level based on the provided level info
 * @param linfo the level info to create the level for
 * @param space if true, create the physical space for the game to run
 * @param editMode if true, do not actually launch the game
 */
void level_init(LevelInfo *linfo,Uint8 space,Uint8 editMode);

/**
 * @brief get the tile that the mouse is currently over
 * @note takes into consideration the camera position
 * @return a (-1,-1) vector if there is an error
 */
Vector2D level_mouse_to_tile();

/**
 * @brief set a solid tile boarder for a level that already has its tile size set
 * @param linfo the level info to set the tile data for.
 * @note also builds the tile drawing layer
 */
void level_add_border(LevelInfo *linfo);

/**
 * @brief allocate and initialize a blank level
 * @return NULL on error or an empty level otherwise
 */
LevelInfo *level_info_new();

/**
 * @brief free level info
 * @param linfo the level info to free
 */
void level_info_free(LevelInfo *linfo);

/**
 * @brief load level information from file
 * @param filename the level file to load
 * @return NULL on non found or other error.  Usable level info otherwise
 */
LevelInfo *level_info_load(char *filename);

/**
 * @brief add a background to the level info
 */
void level_info_add_background(LevelInfo *linfo,const char *background);

/**
 * @brief change which actor file is used for the tile set
 */
void level_set_tileset(const char *tileset);

/**
 * @brief add a background image to the loaded game level
 */
void level_add_background(Sprite *background);

/**
 * @brief delete a background from the game level
 * @param layer the layer index to delete
 */
void level_delete_background(Uint32 layer);

/**
 * @brief delete a background layer from the level info
 * @param linfo the level info to delete from
 * @param layer the layer to delete
 */
void level_info_delete_background(LevelInfo *linfo,Uint32 layer);

/**
 * @brief raise/lower the background layer for the given level info
 * @param linfo the level info to change
 * @param layer the layer to move
 */
void level_info_raise_layer(LevelInfo *linfo,Uint32 layer);
void level_info_lower_layer(LevelInfo *linfo,Uint32 layer);

/**
 * @brief raise/ lower a background layer for the loaded game level
 */
void level_lower_layer(Uint32 layer);
void level_raise_layer(Uint32 layer);

/**
 * @brief allocation for a 2d integer array
 * @param w width
 * @param h height
 * @return NULL on error or an array that needs to be freed otherwise
 */
int *level_alloc_tilemap(int w,int h);

/**
 * @brief create a new level info and populate based on parameters
 * @param backgroundImage the image file to use as the background
 * @param backgroundMusic the music the play for this level
 * @param tileSet the actor to use for tiles on this level
 * @param tileMapSize how many tiles (width & height) to use fo this level
 */
LevelInfo *level_info_create(
    const char *backgroundImage,
    const char *backgroundMusic,
    const char *tileSet,
    Vector2D    tileMapSize
);

/**
 * @brief update the tile information in the levelInfo struct provided
 * @note: does not auto update the internal structures for drawing / collision
 * @param linfo the pointer to the data to update
 * @param position the place of the new tile to update If position outside of the range of the map, this will be a no-op and log an error
 * @param tile what tile to set the data to.  Note that 0 is tile clear
 */
void level_update_tile(LevelInfo *linfo,Vector2D position,Uint32 tile);

/**
 * @brief get the tile index at the location
 * @param linfo the level info to query
 * @param position the place in the tilemap to check for
 * @return -1 on error, or the tile index otherwise
 */
int level_info_get_tile_index_at(LevelInfo *linfo,Vector2D position);

/**
 * @brief convert a screen position to a tile
 * @param linfo pointer to the level info to base the calculation on
 * @param position the position in world space (camera is not considered)
 * @returns (-1,-1) on error or the tile position (in integer space) otherwise
 */
Vector2D level_position_to_tile(LevelInfo *linfo, Vector2D position);

/**
 * @brief make or remake tile layer for rendering based on level info;
 */
void level_make_tile_layer(LevelInfo *linfo);

/**
 * @brief adds an entity to the level physics space
 * @note entity must have a Body defined
 * @param ent the entity to add to the space
 */
void level_add_entity(Entity *ent);

/**
 * @brief removes an entity from the level physics space
 * @note this should not be done during a space update (ie: collision callbacks)
 * @param ent the entity to be removed
 */
void level_remove_entity(Entity *ent);

/**
 * @brief build the collision space 
 * @note unneeded in editor mode
 */
void level_make_space();

/**
 * @brief add the tiles from the level info to the collision space
 * @param linfo the level info to setup
 */
void level_build_tile_space(LevelInfo *linfo);

/**
 * @brief convert file data to json and save it to file
 * @param linfo the level data to save
 * @param filename the file to save it as.
 * @note it will overwrite
 */
void level_save_to_file(LevelInfo *linfo,const char *filename);

/**
 * @brief get the name of the current level
 * @return NULL on error or the name of the level otherwise
 */
const char *level_get_name();

/**
 * @brief set the background music file for the current level
 */
void level_set_music(const char *music);


/**
 * @brief get the pointer to the collision space of the current level
 */
Space *level_get_space();

/**
 * @brief load up a new game level and move the player into it
 * @param
 */
void level_transition(char *filename, const char *playerTarget, Uint32 targetId);

/**
 * @brief change the tileset for the level info and the current level
 * @param linfo the level info to change
 * @param tileSet the name of the actor file to use
 */
void level_info_set_tileset(LevelInfo *linfo,const char *tileSet);

/**
 * @brief get the name of the tileset for the given level info
 * @param linfo the level info to query
 * @returns NULL on error or not set, the address of the character array otherwise
 */
const char* level_info_get_tileset(LevelInfo *linfo);

/**
 * @brief get the particle emitter for the level
 * @return NULL if not available, or a pointer to the particle emitter
 */
ParticleEmitter *level_get_particle_emitter();

/**
 * @brief get the spawn json for an entity based on a given position
 * @param linfo the level to query
 * @param position the position to check for.  returns the first entity that clips this position
 * @return NULL on no entities found or the json of the entity found
 */
SJson *level_spawn_get_by_position(LevelInfo *linfo,Vector2D position);

/**
 * @brief given the json for a level spawn, get its position as a vector
 * @param spawn the level spawn to check
 * @return an zero vector if not found, or the extracted otherwise
 */
Vector2D level_spawn_position(SJson *spawn);

/**
 * @brief get the name of a level spawn
 * @param spawn the level spawn to query
 * @return NULL on error or a pointer to the name of the spawn
 */
const char *level_spawn_name(SJson *spawn);

/**
 * @brief get the id of the level spawn
 * @param spawn the level spawn to query
 * @return -1 on error, or the id
 */
int level_spawn_get_id(SJson *spawn);

/**
 * @brief call before setting light values for the frame
 */
void level_reset_darkness();

/**
 * @brief add a light for this frame to the level
 * @param position the position for the light
 * @param color color mods (255,255,255) will be no change
 * @param size scale value for the light
 */
void level_set_light_at(Vector2D position,Color color,float size);

/**
 * @brief change the spawn position for a level spawn
 * @param spawn the spawn to change
 * @param position the new position
 */
void level_spawn_change_position(SJson *spawn,Vector2D position);

/**
 * @brief add a new entity spawn point to the level
 * @param linfo the level to add to
 * @param name the name of the spawn to add
 * @param position the location in the map to add it to
 */
void level_add_spawn_entity(LevelInfo *linfo, const char *name,Vector2D position);

#endif
