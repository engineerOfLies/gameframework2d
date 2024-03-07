#include "simple_logger.h"

#include "gf2d_graphics.h"
#include "gf2d_draw.h"

#include "camera.h"

#include "level.h"

typedef struct
{
    Level *activeLevel;
    Sprite *tileSet;
}LevelManager;

static LevelManager level_manager = {0};

int level_get_tile_index(Level *level, int x,int y)
{
    if ((!level)||(!level->tileMap))return 0;
    return level->tileMap[room_get_index(x,y,level->bounds.w)];
}

SDL_Point level_get_tile_from_position(Level *level, Vector2D position)
{
    SDL_Point out = {-1,-1};
    if (!level)return out;
    if ((!level->bounds.w)||(!level->bounds.h))return out;
    out.x = (int)(position.x / level->bounds.w);
    out.y = (int)(position.y / level->bounds.h);
    return out;
}

SDL_Point level_get_tile_from_mouse_position(Level *level)
{
    Vector2D position = {0};
    int mx,my;
    SDL_Point out = {-1,-1};
    if (!level)return out;
    if ((!level->bounds.w)||(!level->bounds.h))return out;
    SDL_GetMouseState(&mx,&my);
    position = camera_get_position();
    position.x += mx;
    position.y += my;
    return level_get_tile_from_position(level, position);
}


void level_system_close()
{
    gf2d_sprite_free(level_manager.tileSet);
    memset(&level_manager,0,sizeof(LevelManager));
}

void level_system_init()
{
    level_manager.tileSet = gf2d_sprite_load_all("images/backgrounds/tileSet.png",32,32, 1,1);
}


void level_print(Level *level)
{
    int i,j;
    if ((!level)||(!level->tileMap))return;
    slog("Level w:%i, h:%i",level->bounds.w,level->bounds.h);
    slog("start: %i,%i",level->start.x,level->start.y);
    slog("end  : %i,%i",level->end.x,level->end.y);
    for (j = 0; j < level->bounds.h;j++)
    {
        for (i = 0; i < level->bounds.w;i++)
        {
            if ((i == level->start.x)&&(j == level->start.y))printf("S");
            else if ((i == level->end.x)&&(j == level->end.y))printf("E");
            else printf("%c",room_get_tile_get_char(level->tileMap[room_get_index(i,j,level->bounds.w)]));

        }
        printf("\n");
    }
}

int level_check_room_overlap(Level *level, Room *room)
{
    Room *check;
    SDL_Point parentDoor;
    SDL_Rect checkR,overlap;
    int i,c;
    if ((!level)||(!room)||(!room->parent))return 0;
    checkR.x = room->bounds.x + 1;
    checkR.y = room->bounds.y + 1;
    checkR.w = room->bounds.w - 2;
    checkR.h = room->bounds.h - 2;
    
    parentDoor = room_get_exit_level_position(room->parent,room->parentDir);
    
    c = gfc_list_get_count(level->rooms);
    for (i = 0; i < c; i++)
    {
        check = gfc_list_get_nth(level->rooms,i);
        if ((!check)||(check == room)||(check == room->parent))continue;
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
            if (((checkR.x - 1) > parentDoor.x)||((checkR.x + checkR.w +1) < parentDoor.x)||
                ((checkR.y - 1) > parentDoor.y)||((checkR.y + checkR.h +1) < parentDoor.y))
            {
                return 0;// lost our door
            }
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
        level->start.x = room->bounds.x + (room->bounds.w/2);
        level->start.y = room->bounds.y + (room->bounds.h/2);
    }
    else
    {
        room->depth = parent->depth + 1;
        room->parent= parent;
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
    int bestDepth = 0;
    Room *room;
    if (!level)return;
    c = gfc_list_get_count(level->rooms);
    for (i = 0; i < c; i++)
    {
        room = gfc_list_get_nth(level->rooms,i);
        if (!room)continue;
        room_setup(room);
        level_dig_room(level,room);
        if (room->depth > bestDepth)
        {
            bestDepth = room->depth;
            level->end.x = room->bounds.x + (room->bounds.w/2);
            level->end.y = room->bounds.y + (room->bounds.h/2);
        }
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

void level_generate_tile_layer(Level *level)
{
    int i,j;
    int w,h;
    int tileIndex;
    if ((!level)||(!level->tileMap))return;
    if (!level_manager.tileSet)return;// can't do this without tiles to draw
    if (level->tileLayer)
    {
        gf2d_sprite_free(level->tileLayer);
        //throw out the old one
        level->tileLayer = NULL;
    }
    level->tileLayer = gf2d_sprite_new();//get a blank sprite to work with
    if (!level->tileLayer)
    {
        slog("failed to generate a tile layer sprite");
        return;
    }
        /*number of tiles across*/      /*how large a tile is*/
    w = level->bounds.w * level_manager.tileSet->frame_w;
    h = level->bounds.h * level_manager.tileSet->frame_h;
    level->tileLayer->surface = gf2d_graphics_create_surface(w,h);
    //draw all the tiles to the layer
    for (j = 0; j < level->bounds.h; j++)
    {
        for (i = 0; i < level->bounds.w; i++)
        {
            tileIndex = level->tileMap[room_get_index(i,j,level->bounds.w)];
            if (!tileIndex)continue; // skip zero, thats an empty tile
            tileIndex--;//but the tiles index from 0, so...
            gf2d_sprite_draw_to_surface(
                level_manager.tileSet,
                vector2d(i*level_manager.tileSet->frame_w,j*level_manager.tileSet->frame_h),
                NULL,
                NULL,
                tileIndex,
                level->tileLayer->surface);
        }
    }
    level->tileLayer->texture = SDL_CreateTextureFromSurface(gf2d_graphics_get_renderer(),level->tileLayer->surface);
    if (!level->tileLayer->texture)
    {
        slog("failed to convert tileLayer to texture");
        gf2d_sprite_free(level->tileLayer);
        level->tileLayer = NULL;
        return;
    }
    SDL_SetTextureBlendMode(level->tileLayer->texture,SDL_BLENDMODE_BLEND);        
    SDL_UpdateTexture(level->tileLayer->texture,
                    NULL,
                    level->tileLayer->surface->pixels,
                    level->tileLayer->surface->pitch);
    level->tileLayer->frame_w = level->tileLayer->surface->w;
    level->tileLayer->frame_h = level->tileLayer->surface->h;
}

void level_draw(Level * level, Vector2D offset)
{
    Rect rect = {0,0,32,32};
    if ((!level)||(!level->tileLayer))return;
    gf2d_sprite_draw_image(level->tileLayer,offset);
    rect.x = (level->start.x * 32) + offset.x;
    rect.y = (level->start.y * 32) + offset.y;
    gf2d_draw_rect_filled(rect,GFC_COLOR_GREEN);
    rect.x = (level->end.x * 32) + offset.x;
    rect.y = (level->end.y * 32) + offset.y;
    gf2d_draw_rect_filled(rect,GFC_COLOR_RED);
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
