#include "simple_logger.h"

#include "level.h"


void level_print(Level *level)
{
    int i,j;
    if ((!level)||(!level->tileMap))return;
    slog("Level:");
    for (j = 0; j < level->bounds.h;j++)
    {
        for (i = 0; i < level->bounds.w;i++)
        {
            printf("%c",room_get_tile_get_char(level->tileMap[room_get_index(i,j,level->bounds.w)]));

        }
        printf("\n");
    }
}

int level_check_room_overlap(Level *level, Room *room)
{
    Room *check;
    SDL_Rect checkR,overlap;
    int i,c;
    if ((!level)||(!room))return 0;
    checkR.x = room->bounds.x + 1;
    checkR.y = room->bounds.y + 1;
    checkR.w = room->bounds.w - 2;
    checkR.h = room->bounds.h - 2;
    
    c = gfc_list_get_count(level->rooms);
    for (i = 0; i < c; i++)
    {
        check = gfc_list_get_nth(level->rooms,i);
        if ((!check)||(check == room))continue;
        while (SDL_IntersectRect(&checkR,&check->bounds,&overlap))
        {
            if (overlap.w < overlap.h)
            {
                checkR.w--;
                if (room->parentDir == ED_W)checkR.x++;
            }
            else
            {
                checkR.h--;
                if (room->parentDir == ED_N)checkR.y++;
            }
            if ((checkR.w+2 < RMINX) || (checkR.h+2 < RMINY))return 0;//failed to fit
        }
    }
    room->bounds.x = checkR.x - 1;
    room->bounds.y = checkR.y - 1;
    room->bounds.w = checkR.w + 2;
    room->bounds.h = checkR.h + 2;
    return 1;
}

Room *level_generate_room(Level *level,Room *parent, ExitDirection parentDir)
{
    int dir;
    Room *child;
    int childDir = room_get_other_dir(parentDir);
    int childAttempts[ED_MAX] = {0};
    Room *room;
    if (!level)return NULL;
    room = room_generate(parent,vector2d(RMINX,RMINY), vector2d(RMAXX,RMAXY));
    if (!room)return NULL;
    if (!parent)
    {
        room->bounds.x = (gfc_random() * (level->bounds.w - room->bounds.w));
        room->bounds.y = (gfc_random() * (level->bounds.h - room->bounds.h));
    }
    else
    {
        room->depth = parent->depth + 1;
        room->parentId = parent->id;
        room->parentDir = parentDir;

        switch (parentDir)
        {
            case ED_N:
                room->bounds.y = parent->bounds.y - room->bounds.h + 1;
                //centered for now, revisit later if I dont like it
                room->bounds.x = parent->bounds.x + (parent->bounds.w /2) - (room->bounds.w/2);
                break;
            case ED_S:
                room->bounds.y = parent->bounds.y + parent->bounds.h - 1;
                room->bounds.x = parent->bounds.x + (parent->bounds.w /2) - (room->bounds.w/2);
                break;
            case ED_E:
                room->bounds.x = parent->bounds.x + parent->bounds.w - 1;
                room->bounds.y = parent->bounds.y + (parent->bounds.h /2) - (room->bounds.h/2);
                break;
            case ED_W:
                room->bounds.x = parent->bounds.x - room->bounds.w + 1;
                room->bounds.y = parent->bounds.y + (parent->bounds.h /2) - (room->bounds.h/2);
                break;
            default:
                break;
        }
        if ((room->bounds.x + room->bounds.w > level->bounds.w)||
            (room->bounds.y + room->bounds.h > level->bounds.h)||
            (room->bounds.x < 0)||
            (room->bounds.y < 0))
        {
            //we have exceeded the bounds, stop
            room_free(room);
            return NULL;
        }
        //check for any overlap and try to fix it
        if (!level_check_room_overlap(level, room))
        {
            //we couldn't fix it, reject the room
            room_free(room);
            return NULL;
        }
        childAttempts[childDir] = 1;//skip where we came from
    }
    room->id = ++level->roomIdPool;// no room should be id 0
    gfc_list_append(level->rooms,room);//add us to the list
//    if (room->depth == 1)return room;
    while(childAttempts[0]+childAttempts[1]+childAttempts[2]+childAttempts[3] < 4)
    {
        dir = room_get_random_dir();
        while (childAttempts[dir])dir = (dir+1)%4;//select a random direction, but then move clockwise until we find an unused dir
        childAttempts[dir] = 1;//set it
        //and check it
        child = level_generate_room(level,room, dir);
        if (child)
        {
            room->children++;
            room_add_child(room,dir, child);
        }
    }
    if ((room->children == 0)||(room->depth == 0))
    {
        room->rtype = RT_Room;
    }
    else
    {
        room->rtype = (int)(gfc_random() * 2.0);
    }
    return room;
}

void level_dig_room(Level *level,Room *room)
{
    int i,j;
    int lx,ly;
    if ((!level)||(!room))return;
    if ((!level->tileMap)||(!room->tileMap))return;
    for (j = 0; j < room->bounds.h;j++)
    {
        for (i = 0; i < room->bounds.w;i++)
        {
            if (room->tileMap[room_get_index(i,j,room->bounds.w)])
            {
                
                lx = i + room->bounds.x;//in level space
                ly = j + room->bounds.y;
                level->tileMap[room_get_index(lx,ly,level->bounds.w)] = room->tileMap[room_get_index(i,j,room->bounds.w)];
            }
        }
    }
}

void level_setup(Level *level)
{
    int i,c;
    Room *room;
    if (!level)return;
    c = gfc_list_get_count(level->rooms);
    for (i = 0; i < c; i++)
    {
        room = gfc_list_get_nth(level->rooms,i);
        if (!room)continue;
        room_setup(room);
        level_dig_room(level,room);
        //room_print(room);
    }
}

Level *level_generate(Uint32 w, Uint32 h)
{
    Level *level;
    if ((w < RMINX)||(h < RMINY))
    {
        slog("cannot generate such a small dungeon");
        return NULL;
    }
    level = level_new();
    if (!level)return NULL;
    
    level->bounds.w = w;
    level->bounds.h = h;
    
    level->tileMap = gfc_allocate_array(sizeof(Uint8),w*h);

    level_generate_room(level,NULL, 0);
    
    level_setup(level);
    level_print(level);//remove this at some point, or make an ASCII GAME!
    
    return level;
}

Level *level_new()
{
    Level *level;
    level = gfc_allocate_array(sizeof(Level),1);
    if (!level)return NULL;
    
    level->rooms = gfc_list_new();
    if (!level->rooms)
    {
        slog("failed to make room list for level");
        free(level);
        return NULL;
    }
    
    return level;
}

void level_free(Level *level)
{
    if (!level)return;
    if (level->rooms)
    {
        gfc_list_foreach(level->rooms,(gfc_work_func*)room_free);
        gfc_list_delete(level->rooms);
    }
    
    if (level->tileMap)free(level->tileMap);
    
    free(level);
}



/*eol@eof*/
