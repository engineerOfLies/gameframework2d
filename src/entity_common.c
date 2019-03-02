#include "gf2d_entity.h"
#include "camera.h"
#include "simple_logger.h"
#include "particle_effects.h"
#include "entity_common.h"
#include "level.h"
#include "player.h"

Collision entity_scan_hit(Entity *self,Vector2D start,Vector2D end, CollisionFilter filter)
{
    Collision c = {0};
    if (!self)return c;
    filter.ignore = &self->body;
    c = gf2d_collision_trace_space(level_get_space(), start, end ,filter);
    gf2d_shape_draw(gf2d_shape_edge(start.x,start.y,end.x,end.y),gf2d_color(255,255,0,255),camera_get_offset());
    return c;
}

void entity_damage(Entity *target,Entity *killer,int damage,float kick)
{
    if ((!target)||(!killer))
    {
        slog("missing entity data");
        return;
    }
    if (target->damage != NULL)
    {
        target->damage(target,damage,killer);
        entity_push(killer,target,kick);
    }
}

void entity_push(Entity *self,Entity *other,float amount)
{
    Vector2D push;
    if ((!self)||(!other))
    {
        slog("missing an entity");
        return;
    }
    vector2d_sub(push,other->position,self->position);
    vector2d_set_magnitude(&push,amount);
    vector2d_add(other->velocity,other->velocity,push);
    vector2d_add(other->body.velocity,other->body.velocity,push);
}

int entity_camera_view(Entity *self)
{
    Rect r,c;
    if (!self)return 0;
    c = camera_get_dimensions();
    r = gf2d_shape_get_bounds(self->shape);
    vector2d_add(r,r,self->position);
    return gf2d_rect_overlap(r,c);
}

void entity_apply_gravity(Entity *self)
{
    self->velocity.y += 0.58;
    if (entity_wall_check(self,vector2d(0,2)))
    {
        if (self->velocity.y > 0)self->velocity.y = 0;
        self->grounded = 1;
    }
    else
    {
        self->grounded = 0;
    }
}

int entity_wall_check(Entity *self, Vector2D dir)
{
    Shape s;
    int i,count;
    Collision *c;
    List *collisionList;
    CollisionFilter filter = {
        1,
        WORLD_LAYER,
        0,
        0,
        &self->body
    };

    if (!self)return 0;
    s = gf2d_body_to_shape(&self->body);
    gf2d_shape_move(&s,dir);

    collisionList = gf2d_collision_check_space_shape(level_get_space(), s,filter);
    if (collisionList != NULL)
    {
        count = gf2d_list_get_count(collisionList);
        for (i = 0;i < count;i++)
        {
            c = (Collision*)gf2d_list_get_nth(collisionList,i);
            if (!c)continue;
            if (!c->shape)continue;
            gf2d_shape_draw(*c->shape,gf2d_color(255,255,0,255),camera_get_offset());
        }
        gf2d_collision_list_free(collisionList);
        return 1;
    }
    return 0;
}

List *entity_get_clipped_entities(Entity *self,Shape s, Uint32 layers, Uint32 team)
{
    CollisionFilter filter = {0};
    filter.ignore = &self->body;
    filter.cliplayer = layers;
    filter.team = team;
    return gf2d_collision_check_space_shape(level_get_space(), s,filter);
}

void entity_activate(Entity *self)
{
    int i,count;
    Entity *other;
    Collision *c;
    List *collisionList = NULL;
    collisionList = entity_get_clipped_entities(self,gf2d_body_to_shape(&self->body), OBJECT_LAYER, 0);
    count = gf2d_list_get_count(collisionList);
    for (i = 0; i < count;i++)
    {
        c = (Collision*)gf2d_list_get_nth(collisionList,i);
        if (!c)continue;
        if (!c->body)continue;
        if (!c->body->data)continue;
        other = c->body->data;
        if (other->activate)other->activate(other,self);
    }
    gf2d_collision_list_free(collisionList);
}

void entity_world_snap(Entity *self)
{
    if (entity_wall_check(self, vector2d(0,0.1)))
    {
        self->position.y -= 0.1;
    }
    if (entity_wall_check(self, vector2d(0,-0.1)))
    {
        self->position.y += 0.1;
    }
    if (entity_wall_check(self, vector2d(0.1,0)))
    {
        self->position.x -= 0.1;
    }
    if (entity_wall_check(self, vector2d(-0.1,0)))
    {
        self->position.x += 0.1;
    }    
}

void entity_clear_all_but_player()
{
    Entity *ent = NULL;
    for (ent = gf2d_entity_iterate(NULL);ent != NULL;ent = gf2d_entity_iterate(ent))
    {
        if (ent == player_get())continue;
        gf2d_entity_free(ent);
    }
}

Entity *entity_get_from_body(Body *body)
{
    if (!body) return NULL;
    if (!gf2d_entity_validate_entity_pointer(body->data))return NULL;
    return (Entity*)body->data;
}



Entity *entity_get_touching_player(Entity *self)
{
    Entity *player;
    player = player_get();
    if (!player)return NULL;
    if (gf2d_body_body_collide(&self->body,&player->body))
    {
        return player;
    }
    return NULL;
}
/*eol@eof*/
