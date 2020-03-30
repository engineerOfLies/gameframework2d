#include "simple_logger.h"
#include "gfc_types.h"

#include "gf2d_draw.h"

#include "camera.h"
#include "walkmask.h"


void pointdata_free(PointData *point)
{
    if (!point)return;
    free(point);
}

PointData *pointdata_new()
{
    PointData *point;
    point = gfc_allocate_array(sizeof(PointData),1);
    if (!point)return NULL;
    // if I need other data it will go here
    return point;
}

void walkmask_free(Walkmask *mask)
{
    int i,count;
    PointData *point;
    if (!mask)return;
    count = gfc_list_get_count(mask->points);
    for (i = 0; i < count; i++)
    {
        point = gfc_list_get_nth(mask->points,i);
        if (!point)continue;
        pointdata_free(point);
    }
    gfc_list_delete(mask->points);
}

Walkmask *walkmask_new()
{
    Walkmask *mask;
    mask = (Walkmask *)gfc_allocate_array(sizeof(Walkmask),1);
    if (!mask)return NULL;
    mask->points = gfc_list_new();
    return mask;
}

Walkmask *walkmask_new_by_rect(Rect r)
{
    Vector2D position;
    PointData *point;
    Walkmask *mask;
    
    mask = walkmask_new();
    if (!mask)return NULL;
    
    position = vector2d(r.x,r.y);
    point = walkmask_insert_point(mask,position,NULL);
    if (!point)
    {
        walkmask_free(mask);
        return NULL;
    }
    
    position = vector2d(r.x + r.w,r.y);
    point = walkmask_insert_point(mask,position,point);
    if (!point)
    {
        walkmask_free(mask);
        return NULL;
    }
    
    position = vector2d(r.x + r.w,r.y + r.h);
    point = walkmask_insert_point(mask,position,point);
    if (!point)
    {
        walkmask_free(mask);
        return NULL;
    }

    position = vector2d(r.x,r.y + r.h);
    point = walkmask_insert_point(mask,position,point);
    if (!point)
    {
        walkmask_free(mask);
        return NULL;
    }
    
    return mask;
}

PointData *walkmask_get_nearest_point(Walkmask *mask, Vector2D refPoint)
{
    int i,c;
    PointData *best = NULL;
    PointData *point = NULL;
    float bestDist = -1,dist = -1;
    if (!mask)return NULL;
    c = gfc_list_get_count(mask->points);
    for (i = 0;i < c; i++)
    {
        point = (PointData*)gfc_list_get_nth(mask->points,i);
        if (!point)continue;
        if (bestDist == -1)
        {
            best = point;
            bestDist = vector2d_magnitude_squared(vector2d(point->position.x - refPoint.x,point->position.y - refPoint.y));
            continue;
        }
        dist = vector2d_magnitude_squared(vector2d(point->position.x - refPoint.x,point->position.y - refPoint.y));
        if (dist < bestDist)
        {
            best = point;
            bestDist = dist;
        }
    }
    return best;
}

PointData *walkmask_subdivide_point(Walkmask *mask,PointData *previous)
{
    PointData *nextpoint = NULL;
    Vector2D position;
    if ((!mask)||(!previous))return NULL;
    
    nextpoint = gfc_list_get_nth(mask->points,previous->nextPoint);
    if (!nextpoint)
    {
        return NULL;
    }
    
    vector2d_sub(position,previous->position,nextpoint->position);
    
    return walkmask_insert_point(mask,position,previous);
}


PointData *walkmask_insert_point(Walkmask *mask,Vector2D position,PointData *previous)
{
    int i;
    PointData *newone;
    if (!mask)return NULL;
    newone = pointdata_new();
    if (!newone)return NULL;
    vector2d_copy(newone->position,position);
    mask->points = gfc_list_append(mask->points,newone);
    i = gfc_list_get_item_index(mask->points,newone);
    if (previous)
    {
        if (i != -1)
        {
            newone->nextPoint = previous->nextPoint;
            previous->nextPoint = i;
        }
    }
    return newone;
}

void walkmask_move(Walkmask *mask,Vector2D offset)
{
    PointData *point;
    int i = 0,c;
    if (!mask)return;    
    c = gfc_list_get_count(mask->points);
    for (i = 0;i < c; i++)
    {
        point = (PointData*)gfc_list_get_nth(mask->points,i);
        if (!point)continue;
        vector2d_add(point->position,point->position,offset);
    }
}

void walkmask_draw(Walkmask *mask,Color color,Vector2D drawOffset)
{
    PointData *nextpoint = NULL;
    PointData *point;
    PointData *firstpoint = NULL;
    Vector2D p1,p2,offset;
    int i = 0,c;
    if (!mask)return;
    c = gfc_list_get_count(mask->points);
    firstpoint = point = gfc_list_get_nth(mask->points,0);
    if (!point)return;
    offset = camera_get_offset();
    vector2d_add(offset,offset,drawOffset);
    do
    {
        vector2d_add(p1,point->position,offset);
        gf2d_draw_circle(p1, 5, gfc_color_to_vector4(color));
        nextpoint = gfc_list_get_nth(mask->points,point->nextPoint);
        if (!nextpoint)
        {
            slog("no next point for current mask point");
            return;
        }
        vector2d_add(p1,point->position,offset);
        vector2d_add(p2,nextpoint->position,offset);
        
        gf2d_draw_line(p1,p2, gfc_color_to_vector4(color));
        
        point = nextpoint;
    }
    while ((point != firstpoint)&&(++i < c));
}

Rect walkmask_get_bounds(Walkmask *mask)
{
    PointData *p = NULL;
    int i,c;
    Rect r = {0};
    if (!mask)return r;
    c = gfc_list_get_count(mask->points);
    p = gfc_list_get_nth(mask->points,0);
    if (!p)return r;
    r.x = r.w = p->position.x;
    r.y = r.h = p->position.y;
    for (i = 1;i < c;i++)
    {
        p = (PointData *)gfc_list_get_nth(mask->points,i);
        if (!p)continue;
        if (p->position.x < r.x)r.x = p->position.x;
        if (p->position.y < r.y)r.x = p->position.y;
        if (p->position.x > r.w)r.w = p->position.x;
        if (p->position.y > r.h)r.h = p->position.y;
    }
    r.w = r.w - r.x;
    r.h = r.h - r.y;
    return r;
}

int walkmask_point_in_check(Walkmask *mask, Vector2D queryPoint)
{
    PointData *p1,*p2;
    Vector2D refPoint;
    int i,c;
    int crossCount = 0;
    if (!mask)return 0;
    refPoint.x = queryPoint.x + 100000;
    refPoint.y = queryPoint.y;
    // for each point pair, check if the line described by the queryPoint crosses the edge.  Odd number means inside, even number means outside
    c = gfc_list_get_count(mask->points);
    for (i = 0; i < c; i++)
    {
        p1 = gfc_list_get_nth(mask->points,i);
        p2 = gfc_list_get_nth(mask->points,p1->nextPoint);
        crossCount += gf2d_edge_intersect(
            gf2d_edge_from_vectors(queryPoint,refPoint),
            gf2d_edge_from_vectors(p1->position,p2->position));
    }
    return crossCount % 2;
}

/*eol@eof*/
