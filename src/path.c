#include "simple_logger.h"

#include "gf2d_draw.h"

#include "rooms.h"
#include "path.h"

typedef struct PathNode_S
{
    Vector2D position;          /**<my position in the world*/
    struct PathNode_S *parent;  /**<my parent*/
    int      depth;             /**<how many steps it takes to get to me*/
    float    distance;          /**<how many steps I estimate it will take to get to the goal*/
}PathNode;

typedef struct
{
    List *todo;
    List *done;
    Uint8 *tilemap;
    Vector2D start;
    Vector2D end;
    Uint32 w;
    Uint32 h;
}PathInfo;

PathNode *path_node_new()
{
    PathNode *node;
    node = gfc_allocate_array(sizeof(PathNode),1);
    return node;
}

void path_node_free(PathNode *node)
{
    if (!node)return;
    free(node);
}

PathNode *path_node_get_by_position(List *nodes,Vector2D position)
{
    int i,c;
    PathNode *node;
    c = gfc_list_get_count(nodes);
    for (i = 0; i < c; i++)
    {
        node = gfc_list_get_nth(nodes,i);
        if (!node)continue;
        if (vector2d_compare(node->position,position))
        {
            return node;
        }
    }
    //not found
    return NULL;
}

int path_node_goal_check(PathNode *node,Vector2D end)
{
    if (!node)return 0;
    if (vector2d_compare(node->position, end))return 1;
    return 0;
}

void path_node_estimate_distance(PathNode *node, Vector2D end)
{
    if (!node)return;
    node->distance = vector2d_magnitude_between(node->position,end);
}


// note this approach looks for the best guess as to which node is going to be a better candidate
// if we wanted breadth first search, we should always take the FIRST node in this list
// if we wanted depth first search, we should always the the LAST node in this list
// if we ignore estimated distance, we get Djikstra's algorithm
// here I am using the A* approach
PathNode *path_node_list_get_next(List *nodes)
{
    PathNode *bestNode = NULL;
    PathNode *node;
    float bestCost = -1.0;
    int i,c;
    c = gfc_list_get_count(nodes);
    for (i = 0; i < c; i++)
    {
        node = gfc_list_get_nth(nodes,i);
        if (!node)continue;
        if ((node->depth + node->distance < bestCost)||(bestCost < 0))
        {
            bestCost = node->depth + node->distance;
            bestNode = node;
        }
    }
    return bestNode;
}

PathInfo *path_info_new()
{
    PathInfo *pathInfo;
    pathInfo = gfc_allocate_array(sizeof(PathInfo),1);
    if (!pathInfo)return NULL;
    pathInfo->todo = gfc_list_new();
    pathInfo->done = gfc_list_new();
    return pathInfo;
}

void path_info_free(PathInfo *pathInfo)
{
    if (!pathInfo)return;
    gfc_list_foreach(pathInfo->todo,(gfc_work_func*)path_node_free);
    gfc_list_delete(pathInfo->todo);
    gfc_list_foreach(pathInfo->done,(gfc_work_func*)path_node_free);
    gfc_list_delete(pathInfo->done);
    free(pathInfo);
}

void path_info_add_todo(PathInfo *pathInfo,PathNode *parent, Vector2D newPosition)
{
    PathNode *node;
    if ((!pathInfo)||(!pathInfo->todo)||(!pathInfo->done))return;
    //lets check if its already here:
    node = path_node_get_by_position(pathInfo->todo,newPosition);//is it on the todo?
    if (!node)node = path_node_get_by_position(pathInfo->done,newPosition);//is it already checked?
    if (node)
    {
        //slog("node (%f,%f) already considered",newPosition.x,newPosition.y);
        //already here.  We can choose to update if the path to get there is better than previously, or let it stand
        //right now I am going to let it stand
        return;
    }
    //hasn't been done yet, lets make a new one
    node = path_node_new();
    if (!node)
    {
        slog("failed to allocate memory for a new path search node");
        return;
    }
    vector2d_copy(node->position,newPosition);
    path_node_estimate_distance(node, pathInfo->end);
    if (parent)
    {
        node->parent = parent;
        node->depth = parent->depth + 1;
    }
    gfc_list_append(pathInfo->todo,node);
}

int path_info_get_tile_index(PathInfo *pathInfo,int x, int y)
{
    if (!pathInfo)return 0;
    if ((x < 0)||(x >= pathInfo->w)||
        (y < 0)||(y >= pathInfo->h))return 0;
    return pathInfo->tilemap[room_get_index(x,y,pathInfo->w)];
}

int path_info_find_end(PathInfo *pathInfo)
{
    PathNode *node;
    Vector2D newPosition;
    while (gfc_list_get_count(pathInfo->todo))
    {
        node = path_node_list_get_next(pathInfo->todo);
        if (!node)break;// this shouldn't happen
        gfc_list_append(pathInfo->done,node);//add us to the done pile
        gfc_list_delete_data(pathInfo->todo,node);//remove us from the todo pile
//        slog("checking node (%f,%f)",node->position.x,node->position.y);
        if (path_node_goal_check(node,pathInfo->end))
        {
            slog("end node found!");
            return 1;//we reached out goal
        }
        //now add my kids to the todo pile
        //this is how you handle it where each node is a tile, and only connects to adjacent nodes that are
        //walkable ie: index is 1.  with 0 being empty space, and 2 is a wall.  this would need to be adjusted
        //for other setups
        vector2d_set(newPosition,node->position.x - 1,node->position.y);
        if (path_info_get_tile_index(pathInfo,newPosition.x,newPosition.y) == 1)
        {
            path_info_add_todo(pathInfo,node, newPosition);
        }
        vector2d_set(newPosition,node->position.x + 1,node->position.y);
        if (path_info_get_tile_index(pathInfo,newPosition.x,newPosition.y) == 1)
        {
            path_info_add_todo(pathInfo,node, newPosition);
        }
        vector2d_set(newPosition,node->position.x,node->position.y - 1);
        if (path_info_get_tile_index(pathInfo,newPosition.x,newPosition.y) == 1)
        {
            path_info_add_todo(pathInfo,node, newPosition);
        }
        vector2d_set(newPosition,node->position.x,node->position.y + 1);
        if (path_info_get_tile_index(pathInfo,newPosition.x,newPosition.y) == 1)
        {
            path_info_add_todo(pathInfo,node, newPosition);
        }
    }
    //we have nothing left to search and we still haven't found it
    slog("failed to find the end node");
    return 0;
}

void path_free(List *path)
{
    if (!path)return;
    gfc_list_foreach(path,(gfc_work_func*)free);
    gfc_list_delete(path);
}

List *path_build(PathInfo *pathInfo)
{
    Vector2D *position;
    PathNode *node;
    int c;
    List *path = NULL;
    if ((!pathInfo)||(!pathInfo->done))return NULL;
    c = gfc_list_get_count(pathInfo->done);
    if (!c)return NULL;
    path = gfc_list_new();
    if (!path)return NULL;
    node = gfc_list_get_nth(pathInfo->done,c - 1);
    //the final node in the list should be the end position
    while (node)
    {
        position = vector2d_new();
        if (!position)
        {
            //this is very bad
            slog("failed to add a node to the final path");
            path_free(path);
            return NULL;
        }
        vector2d_copy((*position),node->position);
        gfc_list_prepend(path,position);//by pre-pending we make the final list in order from start at 0 and end at the end of the list
        node = node->parent;//traverse backwards from how we got here
        //when we reach the start, we won't have a parent, so the next node will be NULL and the while condition will trigger
    }
    return path;
}

List *path_find(Uint8 *tilemap,Uint32 w,Uint32 h,Vector2D start, Vector2D end)
{
    List *path = NULL;
    PathInfo *pathInfo;
    if ((!tilemap)||(!w)||(!h))return NULL;
    if ((start.x < 0)||(end.x < 0)||(start.x >= w)||(end.x >= w)||
        (start.y < 0)||(end.y < 0)||(start.y >= h)||(end.y >= h))return NULL;// out of bounds
    if (vector2d_compare(start,end))
    {
        //our goal is our starting point, nothing to do!
        return NULL;
    }
    pathInfo = path_info_new();
    if (!pathInfo)return NULL;
    pathInfo->tilemap = tilemap;
    pathInfo->w = w;
    pathInfo->h = h;
    vector2d_copy(pathInfo->start,start);
    vector2d_copy(pathInfo->end,end);
    //add our starting point
    path_info_add_todo(pathInfo,NULL, start);
    if (path_info_find_end(pathInfo))
    {
        //we found the end position, now lets build the list of tiles that reach out goal in sequence
        path = path_build(pathInfo);
    }
    path_info_free(pathInfo);
    return path;
}

void path_draw(List *path,Vector2D offset)
{
    Vector2D *position,*last;
    int i,c;
    if (!path)return;
    c = gfc_list_get_count(path);
    if (c < 2)return; //need at least two points to draw a line
    last = gfc_list_get_nth(path,0);
    if (!last)return;
    for (i = 1;i < c;i++)
    {
        position = gfc_list_get_nth(path,i);
        if (!position)continue;
        gf2d_draw_line(vector2d((last->x * 32) + offset.x + 16,(last->y * 32) + offset.y + 16),
                       vector2d((position->x *32) + offset.x + 16,(position->y *32) + offset.y + 16), GFC_COLOR_YELLOW);
        last = position;
    }
}

/*eol@eof*/
