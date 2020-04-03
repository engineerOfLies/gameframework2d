#include "simple_logger.h"
#include "gfc_types.h"

#include "gf2d_draw.h"
#include "gf2d_config.h"

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
    
    vector2d_add(position,previous->position,nextpoint->position);
    vector2d_scale(position,position,0.5);
    
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

SJson *pointdata_to_json(PointData *point)
{
    SJson *json;
    if (!point)return NULL;
    json = sj_object_new();
    if (!json)return NULL;
    sj_object_insert(json,"position",sj_vector2d_new(point->position));
    sj_object_insert(json,"scale",sj_vector2d_new(point->scale));
    sj_object_insert(json,"layer",sj_new_int(point->layer));
    sj_object_insert(json,"nextPoint",sj_new_int(point->nextPoint));
    return json;
}

SJson *walkmask_to_json(Walkmask *mask)
{
    SJson *json,*array,*item;
    PointData *point;
    int i,c;
    if (!mask)
    {
        slog("no mask provided to convert to json");
        return NULL;
    }
    
    json = sj_object_new();
    if (!json)
    {
        slog("failed to make json object for mask");
        return NULL;
    }
    sj_object_insert(json,"exterior",sj_new_bool(mask->exterior));
    array = sj_array_new();
    sj_object_insert(json,"points",array);
    
    c = gfc_list_get_count(mask->points);
    for (i = 0;i < c;i++)
    {
        point = gfc_list_get_nth(mask->points,i);
        if (!point)continue;
        item = pointdata_to_json(point);
        if (!item)continue;
        sj_array_append(array,item);
    }
    return json;
}

PointData *pointdata_load_from_json(SJson *json)
{
    PointData *point;
    int temp;
    if (!json)return NULL;
    point = pointdata_new();
    if (!point)return NULL;
    
    sj_value_as_vector2d(sj_object_get_value(json,"position"),&point->position);
    sj_value_as_vector2d(sj_object_get_value(json,"scale"),&point->scale);
    if (sj_get_integer_value(sj_object_get_value(json,"layer"),&temp))
    {
        point->layer = temp;
    }
    if (sj_get_integer_value(sj_object_get_value(json,"nextPoint"),&temp))
    {
        point->nextPoint = temp;
    }
    
    return point;
}

Walkmask *walkmask_load_from_json(SJson *json)
{
    PointData *pdata = NULL;
    Walkmask *mask = NULL;
    SJson *points = NULL, *point = NULL;
    int i,c;
    if (!json)return NULL;

    mask = walkmask_new();
    if (!mask)return NULL;
    
    sj_get_bool_value(sj_object_get_value(json,"exterior"),(short int *)&mask->exterior);
    points = sj_object_get_value(json,"points");
    c = sj_array_get_count(points);
    for (i = 0; i < c; i++)
    {
        point = sj_array_get_nth(points,i);
        if (!point)continue;
        pdata = pointdata_load_from_json(point);
        if (!pdata)continue;
        mask->points = gfc_list_append(mask->points,pdata);
    }

    return mask;

}

int walkmask_edge_clip(Walkmask *mask,Vector2D start, Vector2D end,Vector2D *contact)
{
    PointData *p1,*p2;
    Vector2D best,poc;
    float bestDistance = -1,distance;
    int i,c;
    int crossCount = 0;
    if (!mask)return 0;

    // for each point pair, check if the line described by the queryPoint crosses the edge.  Odd number means inside, even number means outside
    c = gfc_list_get_count(mask->points);
    for (i = 0; i < c; i++)
    {
        p1 = gfc_list_get_nth(mask->points,i);
        p2 = gfc_list_get_nth(mask->points,p1->nextPoint);
        if (gf2d_edge_intersect_poc(
            gf2d_edge_from_vectors(start,end),
            gf2d_edge_from_vectors(p1->position,p2->position),
            &poc,
            NULL))
        {
            crossCount++;
            distance = vector2d_magnitude_between(start,poc);
            if ((bestDistance == -1) || (distance < bestDistance))
            {
                vector2d_copy(best,poc);
                bestDistance = distance;
            }
        }
    }
    if (crossCount)
    {
        if (contact)
            *contact = best;
        return 1;
    }
    return 0;
}


/*eol@eof*/
