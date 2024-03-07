#ifndef __ROOMS_H__
#define __ROOMS_H__

#include "gfc_shape.h"

#define RMINX 5
#define RMINY 5
#define RMAXX 10
#define RMAXY 10

typedef struct Room_S Room;

typedef enum
{
    ED_N,
    ED_E,
    ED_S,
    ED_W,
    ED_MAX
}ExitDirection;

typedef enum
{
    ET_Wall,  //its a wall!
    ET_Open,    //open the space, but no door
    ET_Door,    //spawn a door here in open state
    ET_Secret,
    ET_MAX
}ExitTypes;

typedef struct
{
    ExitTypes       et;         //see the enum
    SDL_Point       position;   //where in the room this is located (relative to room position) always on the border
    Room           *room;       //points to the room this exit leads to
}Exit;

typedef enum
{
    RT_Room,
    RT_Hall,
    RT_Cave,
    RT_MAX
}RoomType;

struct Room_S
{
    Uint32  id;                  //unique id for the level
    int     size;                   // area of the room
    RoomType rtype;             //room type
    int     depth;                  //distance from starting room
    SDL_Rect bounds;            //room bounds
    Uint8  *tileMap;             //tiles for the room.  It will be bounds.w * bounds.h large and will be bounds.w per line
                                // 0 will be WALL, 1 will be open
    Exit    exits[ED_MAX];         //exit information
    Uint8   children;           //how many children have been set
    Room   *parent;
    ExitDirection   parentDir;  //how this room was added off of the parent
};

/**
 * @brief initialize the room manager (needed to render rooms)
 */
void room_manager_init();

/**
 * @brief get a random manhattan direction to move in
 */
ExitDirection room_get_random_dir();

/**
 * @brief get the opposite direction from the one provided
 */
ExitDirection room_get_other_dir(ExitDirection from);

/**
 * @brief get the linear array index given a position and array width
 */
Uint32 room_get_index(int x,int y,int w);

/**
 * @brief for debug purpose, get the character to print for a given tile
 */
char room_get_tile_get_char(Uint8 tile);

/**
 * @brief get the position in room space where an exit should be
 */
SDL_Point room_get_exit_position(Room *room,ExitDirection parentDir);

/**
 * @brief get the position in level space where an exit should be for a room
 */
SDL_Point room_get_exit_level_position(Room *room,ExitDirection parentDir);


/**
 * @brief allocate a new blank room
 * @return the room, or NULL if bad
 */
Room *room_new();

/**
 * @brief free a previously allocated room
 */
void room_free(Room *room);

/**
 * @brief generate a new room and give it random width and height based on k
 */
Room *room_generate(Room *parent,Vector2D mins, Vector2D maxs);

/**
 * @brief add a room as a child to another room
 */
void room_add_child(Room *parent,ExitDirection parentDir, Room *child);

/**
 * @brief "dig" the room for its tileMap
 */
void room_setup(Room *room);

/**
 * @brief print the room to console
 */
void room_print(Room *room);

#endif
