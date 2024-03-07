#include "simple_logger.h"

#include "gfc_types.h"

#include "rooms.h"


Uint32 room_get_index(int x,int y,int w)
{
    return x + (y * w);
}

char room_get_tile_get_char(Uint8 tile)
{
    switch (tile)
    {
        case 1:
            return '.';
        case 2:
            return '#';
    }
    return ' ';
}

Room *room_new()
{
    Room *room;
    room = gfc_allocate_array(sizeof(Room),1);
    if (!room)
    {
        slog("failed to allocate new room");
        return NULL;
    }
    //any other boilerplate code
    
    return room;
}

void room_free(Room *room)
{
    if (!room)return;
    //clean up
    if (room->tileMap)free(room->tileMap);
    free(room);
}

Room *room_generate(Room *parent, Vector2D mins, Vector2D maxs)
{
    Room * room;
    room = room_new();
    if (!room)return NULL;
    //random dimensions based on mins and maxs
    room->bounds.w = (int)(mins.x + (gfc_random() * (maxs.x - mins.x)));
    room->bounds.h = (int)(mins.y + (gfc_random() * (maxs.y - mins.y)));
    if (parent)room->parent = parent;
    return room;
}


Uint8 point_in_ellipse(Vector2D p, Vector2D e, Vector2D r)
{
    if ((((p.x - e.x)*(p.x - e.x))/(r.x*r.x))+(((p.y - e.y)*(p.y - e.y))/(r.y*r.y)) <= 1)return 1;
    return 0;
}

void room_print(Room *room)
{
    int i,j;
    if (!room)return;
    slog("Room %i:",room->id);
    for (j = 0; j < room->bounds.h;j++)
    {
        for (i = 0; i < room->bounds.w;i++)
        {
            printf("%c",room_get_tile_get_char(room->tileMap[room_get_index(i,j,room->bounds.w)]));
        }
        printf("\n");
    }
}

int room_floor_adjace(Room *room, int x, int y)
{
    if ((!room)||(!room->tileMap))return 0;
    if (x > 0)
    {
        if (room->tileMap[room_get_index(x - 1,y,room->bounds.w)] == 1)return 1;
    }
    if (x < room->bounds.w - 1)
    {
        if (room->tileMap[room_get_index(x + 1,y,room->bounds.w)] == 1)return 1;
    }
    if (y > 0)
    {
        if (room->tileMap[room_get_index(x,y - 1,room->bounds.w)] == 1)return 1;
    }
    if (y < room->bounds.h - 1)
    {
        if (room->tileMap[room_get_index(x,y + 1,room->bounds.w)] == 1)return 1;
    }
    if ((x > 0)&&(y > 0))
    {
        if (room->tileMap[room_get_index(x - 1,y - 1,room->bounds.w)] == 1)return 1;
    }
    if ((x < room->bounds.w - 1) && (y < room->bounds.h - 1))
    {
        if (room->tileMap[room_get_index(x + 1,y + 1,room->bounds.w)] == 1)return 1;
    }
    if ((x > 0)&&(y < room->bounds.h - 1))
    {
        if (room->tileMap[room_get_index(x - 1,y + 1,room->bounds.w)] == 1)return 1;
    }
    if ((x < room->bounds.w - 1)&&(y > 0))
    {
        if (room->tileMap[room_get_index(x + 1,y - 1,room->bounds.w)] == 1)return 1;
    }
    return 0;
}

void room_add_walls(Room *room)
{
    int i,j;
    if ((!room)||(!room->tileMap))return;
    for (j = 0; j < room->bounds.h;j++)
    {
        for (i = 0; i < room->bounds.w;i++)
        {
            if ((!room->tileMap[room_get_index(i,j,room->bounds.w)])&&(room_floor_adjace(room, i,j)))
            {
                room->tileMap[room_get_index(i,j,room->bounds.w)] = 2;
            }
        }
    }
}

void room_setup(Room *room)
{
    int i,j;
    Vector2D centerPoint;
    if (!room)return;
    room->size = room->bounds.w * room->bounds.h;
    if (!room->size)
    {
        slog("room %i has zero area",room->id);
        return;
    }
    centerPoint = vector2d(room->bounds.w/2, room->bounds.h/2);
    //dig the room
    room->tileMap = gfc_allocate_array(sizeof(Uint8),room->size);
    if (!room->tileMap)
    {
        slog("failed to make room %i tilemap",room->id);
        return;
    }
    switch (room->rtype)
    {
        case RT_Room:
        default:
            for (j = 1; j < (room->bounds.h - 1);j++)
            {
                for (i = 1; i < (room->bounds.w - 1);i++)
                {
                    room->tileMap[room_get_index(i,j,room->bounds.w)] = 1;//dug
                }
            }
            break;
        case RT_Hall:
            if (room->exits[ED_N].et)
            {
                //dig up
                for (i = 0; i <= centerPoint.y;i++)
                {
                    room->tileMap[room_get_index(room->exits[ED_N].position.x,i,room->bounds.w)] = 1;//dug
                }
            }
            if (room->exits[ED_S].et)
            {
                //dig down
                for (i = room->bounds.h - 1; i >= centerPoint.y;i--)
                {
                    room->tileMap[room_get_index(room->exits[ED_S].position.x,i,room->bounds.w)] = 1;//dug
                }
            }
            if (room->exits[ED_W].et)
            {
                //dig left
                for (i = 0; i <= centerPoint.x;i++)
                {
                    room->tileMap[room_get_index(i,room->exits[ED_W].position.y,room->bounds.w)] = 1;//dug
                }
            }
            if (room->exits[ED_E].et)
            {
                //dig right
                for (i = room->bounds.w - 1; i >= centerPoint.x;i--)
                {
                    room->tileMap[room_get_index(i,room->exits[ED_E].position.y,room->bounds.w)] = 1;//dug
                }
            }
            break;
        case RT_Cave:
            for (j = 1; j < (room->bounds.h - 1);j++)
            {
                for (i = 1; i < (room->bounds.w - 1);i++)
                {
                    if (point_in_ellipse(vector2d(i,j), centerPoint, centerPoint))
                    {
                        room->tileMap[i + (j * room->bounds.w)] = 1;//dug
                    }
                }
            }
            break;
    
    }
    //then dig the exits:
    for (i = 0;i < ED_MAX; i++)
    {
        if (!room->exits[i].et)continue;
        room->tileMap[room_get_index(room->exits[i].position.x,room->exits[i].position.y,room->bounds.w)] = 1;//dug
    }
    room_add_walls(room);
}


SDL_Point room_get_exit_level_position(Room *room,ExitDirection parentDir)
{
    SDL_Point out = {-1,-1};
    if (!room)return out;
    out = room_get_exit_position(room,parentDir);
    out.x += room->bounds.x;
    out.y += room->bounds.y;
    return out;
}

SDL_Point room_get_exit_position(Room *room,ExitDirection parentDir)
{
    SDL_Point out = {-1,-1};
    if (!room)return out;
    switch(parentDir)
    {
        case ED_N:
            out.y = 0;
            out.x = room->bounds.w / 2;
            break;
        case ED_S:
            out.y = room->bounds.h - 1;
            out.x = room->bounds.w / 2;
            break;
        case ED_E:
            out.x = room->bounds.w - 1;
            out.y = room->bounds.h / 2;
            break;
        case ED_W:
            out.x = 0;
            out.y = room->bounds.h / 2;
            break;
        default:
            break;
    }
    return out;
}

void room_add_child(Room *parent,ExitDirection parentDir, Room *child)
{
    SDL_Point exit;
    if ((!parent)||(!child))return;
    
    parent->exits[parentDir].room = child;
    parent->exits[parentDir].et = 1;
    exit = room_get_exit_position(parent,parentDir);
    parent->exits[parentDir].position.x = exit.x;
    parent->exits[parentDir].position.y = exit.y;
    
    switch(parentDir)
    {
        case ED_N:
//             parent->exits[ED_N].position.y = 0;
//             parent->exits[ED_N].position.x = parent->bounds.w / 2;
            child->exits[ED_S].room = parent;
            child->exits[ED_S].et = 1;
            child->exits[ED_S].position.y = child->bounds.h - 1;
            child->exits[ED_S].position.x = parent->exits[ED_N].position.x + parent->bounds.x - child->bounds.x;
            break;
        case ED_S:
//             parent->exits[ED_S].position.y = parent->bounds.h - 1;
//             parent->exits[ED_S].position.x = parent->bounds.w / 2;
            child->exits[ED_N].room = parent;
            child->exits[ED_N].et = 1;
            child->exits[ED_N].position.y = 0;
            child->exits[ED_N].position.x = parent->exits[ED_S].position.x + parent->bounds.x - child->bounds.x;
            break;
        case ED_E:
//             parent->exits[ED_E].position.x = parent->bounds.w - 1;
//             parent->exits[ED_E].position.y = parent->bounds.h / 2;
            child->exits[ED_W].room = parent;
            child->exits[ED_W].et = 1;
            child->exits[ED_W].position.x = 0;
            child->exits[ED_W].position.y = parent->exits[ED_E].position.y + parent->bounds.y - child->bounds.y;
            break;
        case ED_W:
//             parent->exits[ED_W].position.x = 0;
//             parent->exits[ED_W].position.y = parent->bounds.h / 2;
            child->exits[ED_E].room = parent;
            child->exits[ED_E].et = 1;
            child->exits[ED_E].position.x = child->bounds.w - 1;
            child->exits[ED_E].position.y = parent->exits[ED_W].position.y + parent->bounds.y - child->bounds.y;
            break;
        case ED_MAX:
            break;
    }
}

ExitDirection room_get_other_dir(ExitDirection from)
{
    switch(from)
    {
        case ED_N:
            return ED_S;
        case ED_S:
            return ED_N;
        case ED_E:
            return ED_W;
        case ED_W:
            return ED_E;
        default:
            return ED_MAX;
    }
}

ExitDirection room_get_random_dir()
{
    float angle = gfc_random() * 360.0;
    if (angle < 90)return ED_N;
    if (angle < 180)return ED_E;
    if (angle < 270)return ED_S;
    return ED_W;
    //better distribution of directions than just random()*4 due to rounding issues
}

/*eol@eof*/
