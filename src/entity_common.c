#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_config.h"

#include "gf2d_camera.h"
#include "gf2d_draw.h"
#include "gf2d_actor.h"
#include "gf2d_entity.h"

#include "message_buffer.h"
#include "particle_effects.h"
#include "player.h"
#include "level.h"
#include "monsters.h"
#include "entity_common.h"

extern int __DebugMode;

int entity_id_from_args(Entity *self)
{
    int i = -1;
    if (!self)return -1;
    if (!self->args)return -1;
    sj_get_integer_value(sj_object_get_value(self->args,"id"),&i);
    return i;
}

Entity *entity_scan_hit(Entity *self,Vector2D start,Vector2D end, CollisionFilter filter,Vector2D *endpoint)
{
    Collision c = {0};
    if (!self)return NULL;
    filter.ignore = &self->body;
    c = gf2d_collision_trace_space(level_get_space(), start, end ,filter);
        
    if (__DebugMode)
    {
        gf2d_draw_shape(
            gfc_shape_edge(start.x,start.y,c.pointOfContact.x,c.pointOfContact.y),
            gfc_color(255,255,0,255),
            gf2d_camera_get_offset());
    }
    if (endpoint)
    {
        endpoint->x = c.pointOfContact.x;
        endpoint->y = c.pointOfContact.y;
    }
    if (c.body == NULL)return NULL;
    return entity_get_from_body(c.body);
}


void entity_push(Entity *self,Entity *other,float amount)
{
    Vector2D push;
    if ((!self)||(!other))
    {
        slog("missing an entity");
        return;
    }
    vector2d_sub(push,other->body.position,self->body.position);
    vector2d_set_magnitude(&push,amount);
    vector2d_add(other->body.velocity,other->body.velocity,push);
    vector2d_add(other->body.velocity,other->body.velocity,push);
}

int entity_camera_view(Entity *self)
{
    Rect r,c;
    if (!self)return 0;
    c = gf2d_camera_get_dimensions();
    r = gfc_shape_get_bounds(self->shape);
    vector2d_add(r,r,self->body.position);
    return gfc_rect_overlap(r,c);
}

void entity_apply_gravity(Entity *self)
{
    if ((!self)||(!self->body.gravity))return;
    if (self->inWater)
    {
        self->body.velocity.y += 0.18 * self->body.gravity;
    }
    else
    {
        self->body.velocity.y += 0.58 * self->body.gravity;
    }
    
    if (entity_wall_check(self,vector2d(0,2)))
    {
        if (self->body.velocity.y > 0)
        {
            self->body.velocity.y = 0;
        }
        self->grounded = 1;
        if (self->body.ignore)self->body.ignore = NULL;// reset once we touch ground again
    }
    else
    {
        self->grounded = 0;
    }
}

int entity_platform_end_check(Entity *self)
{
    Shape s;
    Rect r;
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
    r = gfc_shape_get_bounds(s);
    gfc_shape_move(&s,vector2d(r.w * self->facing.x,3));

    collisionList = gf2d_collision_check_space_shape(level_get_space(), s,filter);
    if (collisionList != NULL)
    {
        gf2d_collision_list_free(collisionList);
        return 1;
    }    
    return 0;
}

void entity_face_from_direction(Entity *self, Vector2D direction)
{
    if (!self)return;
    if (direction.x < 0)
    {
        self->facing.x = -1;
        self->flip.x = 0;
    }
    else if (direction.x > 0)
    {
        self->flip.x = 1;
        self->facing.x = 1;
    }
    if (direction.y < 0)
    {
        self->facing.y = -1;
        self->flip.y = 0;
    }
    else if (direction.y > 0)
    {
        self->flip.y = 1;
        self->facing.y = 1;
    }
}

Entity *entity_get_platform(Entity *self,Vector2D dir)
{
    Entity *target = NULL;
    Shape s;
    int i,count;
    Collision *c;
    List *collisionList;
    CollisionFilter filter = {
        0,
        PLATFORM_LAYER,
        0,
        0,
        &self->body
    };

    if (!self)return NULL;
    s = gf2d_body_to_shape(&self->body);
    if (s.type == ST_RECT)
    {
        if (dir.y < 0)
        {
            s.s.r.y += dir.y;
            s.s.r.h = fabs(dir.y);            
        }
        else if (dir.y > 0)
        {
            s.s.r.y += s.s.r.h;
            s.s.r.h = dir.y;
        }
    }

    collisionList = gf2d_collision_check_space_shape(level_get_space(), s,filter);
    if (collisionList != NULL)
    {
        count = gfc_list_get_count(collisionList);
        for (i = 0;i < count;i++)
        {
            c = (Collision*)gfc_list_get_nth(collisionList,i);
            if (!c)continue;
            if ((!c->body)||(!c->body->data))continue;
            target = (Entity*)c->body->data;
            break;
        }
        gf2d_collision_list_free(collisionList);
    }
    return target;
}

int entity_wall_check(Entity *self, Vector2D dir)
{
    Shape s;
    int i,count;
    int hit = 0;
    Collision *c;
    List *collisionList;
    CollisionFilter filter = {
        self->body.worldclip,
        self->body.cliplayer,
        0,
        0,
        &self->body
    };

    if (!self)return 0;
    s = gf2d_body_to_shape(&self->body);
    if (s.type == ST_RECT)
    {
        if (dir.x < 0)
        {
            s.s.r.w = -dir.x + 1;
        }
        else if (dir.x > 0)
        {
            s.s.r.x += (s.s.r.w + dir.x) - 1;
            s.s.r.w = dir.x + 1;
        }
        if (dir.y < 0)
        {
            s.s.r.h = -dir.y + 1;
        }
        else if (dir.y > 0)
        {
            s.s.r.y += (s.s.r.h - dir.y) - 1;
            s.s.r.h = dir.y + 1;
        }
    }
    gfc_shape_move(&s,dir);

    collisionList = gf2d_collision_check_space_shape(level_get_space(), s,filter);
    if (collisionList != NULL)
    {
        count = gfc_list_get_count(collisionList);
        for (i = 0;i < count;i++)
        {
            c = (Collision*)gfc_list_get_nth(collisionList,i);
            if (!c)continue;
            if ((c->body) && (c->body == self->body.ignore))continue;
            if ((dir.y <= 0)&&(c->body)&&(c->body->cliplayer & PLATFORM_LAYER))
            {
                continue;
            }
            hit = 1;
            if(__DebugMode)
            {
                gf2d_draw_shape(c->shape,gfc_color(255,255,0,255),gf2d_camera_get_offset());
            }
        }
        gf2d_collision_list_free(collisionList);
        return hit;
    }
    return 0;
}

List *entity_get_clipped_world(Entity *self,Shape s)
{
    CollisionFilter filter = {0};
    filter.ignore = &self->body;
    filter.worldclip = 1;
    return gf2d_collision_check_space_shape(level_get_space(), s,filter);
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
    count = gfc_list_get_count(collisionList);
    for (i = 0; i < count;i++)
    {
        c = (Collision*)gfc_list_get_nth(collisionList,i);
        if (!c)continue;
        if (!c->body)continue;
        if (!c->body->data)continue;
        other = c->body->data;
        if (other->activate)
        {
            if (other->activate(other,self))break;//only activate one entity per action
        }
    }
    gf2d_collision_list_free(collisionList);
}

void entity_world_snap(Entity *self)
{
    if (entity_wall_check(self, vector2d(0,1)))
    {
        self->body.position.y -= 1;
    }
    if (entity_wall_check(self, vector2d(0,-1)))
    {
        self->body.position.y += 1;
    }
    if (entity_wall_check(self, vector2d(1,0)))
    {
        self->body.position.x -= 1;
        if (self->body.velocity.x > 0)self->body.velocity.x = 0;
    }
    if (entity_wall_check(self, vector2d(-1,0)))
    {
        self->body.position.x += 1;
        if (self->body.velocity.x < 0)self->body.velocity.x = 0;
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

Shape entity_config_parse_shape(SJson *file)
{
    Vector3D    circle = {0};
    Vector4D    rect = {0};
    Shape shape = {0};
    if (sj_object_get_value(file,"rect"))
    {
        sj_value_as_vector4d(sj_object_get_value(file,"rect"),&rect);
        shape = gfc_shape_rect(rect.x, rect.y,rect.z, rect.w);
    }
    else if (sj_object_get_value(file,"circle"))
    {
        sj_value_as_vector3d(sj_object_get_value(file,"circle"),&circle);
        shape = gfc_shape_circle(circle.x, circle.y,circle.z);        
    }
    return shape;
}

Uint32 entity_config_parse_layers(SJson *layers)
{
    int i,c;
    const char *text;
    SJson *layer;
    Uint32 cliplayer = 0;
    c = sj_array_get_count(layers);
    for (i = 0; i < c; i++)
    {
        layer = sj_array_get_nth(layers,i);
        if (!layer)continue;
        text = sj_get_string_value(layer);
        if (!text)continue;
        if (gfc_stricmp(text,"object") == 0)cliplayer |= OBJECT_LAYER;
        else if (gfc_stricmp(text,"monster") == 0)cliplayer |= MONSTER_LAYER;
        else if (gfc_stricmp(text,"world") == 0)cliplayer |= WORLD_LAYER;
        else if (gfc_stricmp(text,"player") == 0)cliplayer |= PLAYER_LAYER;
        else if (gfc_stricmp(text,"pickup") == 0)cliplayer |= PICKUP_LAYER;
        else if (gfc_stricmp(text,"platform") == 0)cliplayer |= PLATFORM_LAYER;
    }
    return cliplayer;
}

int entity_config_parse_physics(Body *body,Shape *shape,Entity *self, Vector2D position, const char *name,SJson *file)
{
    int         physics = 0;
    float       gravity = 0;
    int         worldclip = 0;
    Uint32      cliplayer = 0;
    Uint32      touchlayer = 0;
    int         team = 0;
    SJson      *value;
    if (!file)return 0;
    if ((!body)||(!shape))return 0;
    if (!sj_is_object(file))return 0;
    
    gravity = body->gravity;
    worldclip = body->worldclip;
    cliplayer = body->cliplayer;
    touchlayer = body->touchlayer;
    team = body->team;//only overwrite new values
    
    value = sj_object_get_value(file,"clip_layers");
    if (value)
    {
        cliplayer |= entity_config_parse_layers(value);
        physics = 1;
    }
    value = sj_object_get_value(file,"touch_layers");
    if (value)
    {
        touchlayer |= entity_config_parse_layers(value);
        physics = 1;
    }
    if (sj_get_integer_value(sj_object_get_value(file,"team"),&team))physics = 1;
    if (sj_get_integer_value(sj_object_get_value(file,"worldclip"),&worldclip))physics = 1;
    if (sj_get_float_value(sj_object_get_value(file,"gravity"),&gravity))physics = 1;
    *shape = entity_config_parse_shape(file);
    
    if (!physics)return 0;// if there is nothing to do, don't do it

    gf2d_body_set(
        body,
        name,
        worldclip,//world clip
        cliplayer,//clip layers
        touchlayer,//touch layers
        team,//team
        position,
        vector2d(0,0),
        10,
        gravity,
        0,
        shape,
        self,
        NULL);

    return 1;
}

void entity_config_from_def(Entity *self,Vector2D position, SJson *file)
{
    MonsterAttack *attack;
    EntityLink *link;
    TextLine linkname;
    int i,c;
    int j,d;
    Uint32 index;
    Vector2D    vector;
    Vector4D    color = {0};
    SJson *sounds,*colorJS,*item,*links,*frames,*frame,*value;
    const char *text;
    if ((!self)||(!file))return;
    
    text = sj_get_string_value(sj_object_get_value(file,"name"));
    if (text)
    {
        gfc_line_cpy(self->name,text);
    }
    //graphics
    if (sj_object_get_value(file,"color"))
    {
        self->color = sj_value_as_color(sj_object_get_value(file,"color"));
    }

    text = sj_get_string_value(sj_object_get_value(file,"state"));
    if (text)
    {
        self->state = gf2d_entity_state_from_name(text);
    }

    text = sj_get_string_value(sj_object_get_value(file,"figure"));
    if (text)
    {
        if (self->figureInstance.figure != NULL)
        {
            gf2d_figure_free(self->figureInstance.figure);
            self->figureInstance.figure = NULL;
        }
        if (self->actor != NULL)
        {
            gf2d_actor_free(self->actor);
            self->actor = NULL;
        }
        self->figureInstance.figure = gf2d_figure_load(text);
        if (self->figureInstance.figure)
        {
            if (self->figureInstance.figure->armature != NULL)
            {
                gf2d_figure_instance_link(&self->figureInstance);
            }
            text = sj_get_string_value(sj_object_get_value(file,"action"));
            if (text)
            {
                gf2d_entity_set_action(self, text);
            }
        }
    }
    else
    {
        text = sj_get_string_value(sj_object_get_value(file,"actor"));
        if (text)
        {
            if (self->figureInstance.figure != NULL)
            {
                gf2d_figure_free(self->figureInstance.figure);
                self->figureInstance.figure = NULL;
            }
            if (self->actor != NULL)
            {
                gf2d_actor_free(self->actor);
                self->actor = NULL;
            }
            self->actor = gf2d_actor_load(text);
            if (self->actor)
            {
                vector2d_copy(self->scale,self->actor->scale);
                vector2d_copy(self->center,self->actor->center);
                text = sj_get_string_value(sj_object_get_value(file,"action"));
                gf2d_entity_set_action(self,text);
            }
        }
    }
    if (sj_value_as_vector2d(sj_object_get_value(file,"scale"),&vector))
    {
        vector2d_copy(self->scale,vector);
    }
        
//physics
    value = sj_object_get_value(file,"physics");
    if (value)
    {
        entity_config_parse_physics(&self->body,&self->shape,self, position, self->name,value);
    }

//attacks
    links = sj_object_get_value(file,"attacks");
    if (links)
    {
        self->attacks = gfc_list_new();
        c = sj_array_get_count(links);
        for (i =0;i < c;i++)
        {
            item = sj_array_get_nth(links,i);
            if (!item)continue;
            attack = gfc_allocate_array(sizeof(MonsterAttack),1);
            text = sj_get_string_value(sj_object_get_value(item,"action"));
            if (text)gfc_line_cpy(attack->action,text);
            text = sj_get_string_value(sj_object_get_value(item,"type"));
            if (text)gfc_line_cpy(attack->type,text);
            text = sj_get_string_value(sj_object_get_value(item,"link"));
            if (text)gfc_line_cpy(attack->link,text);
            text = sj_get_string_value(sj_object_get_value(item,"spell"));
            if (text)gfc_line_cpy(attack->spell,text);
            text = sj_get_string_value(sj_object_get_value(item,"aim"));
            if (text)gfc_line_cpy(attack->aim,text);
            text = sj_get_string_value(sj_object_get_value(item,"sound"));
            if (text)gfc_line_cpy(attack->sound,text);
            sj_get_float_value(sj_object_get_value(item,"min_range"),&attack->min_range);
            sj_get_float_value(sj_object_get_value(item,"max_range"),&attack->max_range);
            sj_get_float_value(sj_object_get_value(item,"attack_range"),&attack->attack_range);
            sj_value_as_vector2d(sj_object_get_value(item,"offset"),&attack->offset);
            frames = sj_object_get_value(item,"damage_frames");
            if (frames)
            {
                attack->attack_frames = gfc_list_new();
                d = sj_array_get_count(frames);
                for (j = 0; j < d; j++)
                {
                    frame = sj_array_get_nth(frames,j);
                    if (!frame)continue;
                    if (sj_get_integer_value(frame,(int *)&index))
                    {
                        #pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
                        #pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
                        attack->attack_frames = gfc_list_append(attack->attack_frames,(void *)index);
                    }
                }
            }
            self->attacks = gfc_list_append(self->attacks,attack);
        }
    }
    links = sj_object_get_value(file,"links");
    if (links)
    {
        self->links = gfc_list_new();// only make a list for the few entities that are large enough to need em
        if (!self->links)
        {
            slog("failed to make new list for links");
            goto done;
        }
        c = sj_array_get_count(links);
        for (i = 0; i < c; i++)
        {
            item = sj_array_get_nth(links,i);
            if (!item)continue;
            link = gfc_allocate_array(sizeof(EntityLink),1);
            if (!link)continue;
            text = sj_get_string_value(sj_object_get_value(item,"bonename"));
            if (text)
            {
                gfc_line_cpy(link->bonename,text);
            }
            else
            {
                slog("failed to get bonename from entity link");
            }
            sj_value_as_vector2d(sj_object_get_value(item,"offset"),&link->offset);
            gfc_line_sprintf(linkname,"%s.%s",self->name,link->bonename);
            entity_config_parse_physics(&link->body,&link->shape,self, vector2d(0,0), linkname,item);
            self->links = gfc_list_append(self->links,link);
        }
    }
done:

    sj_get_float_value(sj_object_get_value(file,"light_level"),&self->light_level);
    colorJS = sj_object_get_value(file,"light_color");
    if (colorJS)
    {
        sj_value_as_vector4d(colorJS,&color);
        self->light_color = gfc_color_from_vector4(color);
    }
    
    sj_get_float_value(sj_object_get_value(file,"speed"),&self->speed);
    sj_get_float_value(sj_object_get_value(file,"jump_height"),&self->jump_height);
    sj_get_float_value(sj_object_get_value(file,"jumpdelay"),&self->jumpdelay);
    sj_get_float_value(sj_object_get_value(file,"attack_range"),&self->attack_range);
    sj_get_float_value(sj_object_get_value(file,"sight_range"),&self->sight_range);
    sj_get_integer_value(sj_object_get_value(file,"attackCooldown"),&self->attackCooldown);
    
    sj_get_float_value(sj_object_get_value(file,"health"),&self->health);
    sj_get_float_value(sj_object_get_value(file,"mana"),&self->mana);
    sj_get_integer_value(sj_object_get_value(file,"healthmax"),&self->healthmax);
    sj_get_integer_value(sj_object_get_value(file,"manamax"),&self->manamax);
    //damage
    sounds = sj_object_get_value(file,"sounds");
    if (sounds)
    {
        self->sound = gfc_sound_pack_parse(sounds);
    }
}

Entity *entity_config_from_file(Entity *self,SJson *args,Vector2D position,const char *defFile)
{
    Vector2D    flip = {0};
    SJson *file,*keys;
    if (!self)return NULL;
    file = sj_load(defFile);
    if (!file)
    {
        slog("no entity file %s",defFile);
        return self;
    }
    gfc_line_cpy(self->name,"unnamed entity");

    entity_config_from_def(self,position, file);
    sj_free(file);
    //slog("parsing entity def file %s",defFile);
    
    keys = sj_object_get_value(args,"keys");
    if (keys)
    {
        if (sj_value_as_vector2d(sj_object_get_value(keys,"flip"),&flip))
        {
            vector2d_copy(self->flip,flip);
        }
        entity_config_from_def(self,position, keys);
    }    
    
    return self;
}


int entity_is_persistent(Entity *self)
{
    SJson *keys;
    int persistent = 0;
    if (!self)return 0;
    keys = sj_object_get_value(self->args,"keys");
    if (!keys)return 0;
    sj_get_integer_value(sj_object_get_value(keys,"persistent"),&persistent);
    return persistent;
}


float entity_calc_damage(float *damage,float *armor)
{
    float total = 0;
    int i;
    if ((!damage)||(!armor))return 0;
    for (i = 0; i < DT_MAX; i++)
    {
        if (!damage[i])continue; // no damage of this type
        if (armor[i] == 0)
        {// no defense against this damage type, deal it all
            total += damage[i];
            continue;
        }
        if (armor[i] < 1)// treat it as a percentage reduction
        {
            total += damage[i] - (damage[i] * armor[i]);
            continue;
        }
        //else treat it as a raw subtract, but don't give health this way
        total += MAX((damage[i] - armor[i]),0);
    }
    return total;
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

int entity_attack_update(
    Entity *self,
    Shape shape,
    float *damage,
    Uint32 layers,
    Uint32 team)
{
    int connectedAttack = 0;
    int i,count;
    Entity *other;
    Collision *c;
    List *collisionList = NULL;
    
    if (!self)return 0;
    if (!damage)return 0;
    
    collisionList = entity_get_clipped_entities(self,shape, layers, team);
    
    if(__DebugMode)gf2d_draw_shape(shape,gfc_color(255,255,0,255),gf2d_camera_get_offset());

    count = gfc_list_get_count(collisionList);
    if(__DebugMode)slog("hit %i targets",count);
    for (i = 0; i < count;i++)
    {
        c = (Collision*)gfc_list_get_nth(collisionList,i);
        if (!c)continue;
        if (!c->body)continue;
        if (!c->body->data)continue;
        other = c->body->data;
        if (other->damage)
        {
            connectedAttack = 1;
            other->damage(other,damage,self);
            break;
        }
    }
    gf2d_collision_list_free(collisionList);
    return connectedAttack;
}


int entity_melee_slash_update(
    Entity *self,
    Vector2D position,
    Vector2D facing,
    float attack_range,
    float impact_radius,
    float *damage,
    Uint32 layers,
    Uint32 team)
{
    Circle c;
    Vector2D dir;
    float angle = 0;
    int attackframe;
    if (!self)return 0;
    if (!damage)return 0;
    
    //check if this is a damage frame
    attackframe = gf2d_entity_get_percent_complete(self);
    if (attackframe <= 25)return 0;
//    if (__DebugMode)slog("percent complete: %i",attackframe);

    angle = (((float)attackframe/100.0)*180-45)*GFC_DEGTORAD;
    dir.x = sin(angle) * attack_range;
    dir.y = -cos(angle) * attack_range;

    c = gfc_circle(position.x + (facing.x * dir.x),position.y + dir.y,impact_radius);

    return entity_attack_update(self,gfc_shape_from_circle(c),damage,layers,team);
}

int entity_melee_thrust_update(
    Entity *self,
    Vector2D position,
    Vector2D facing,
    float attack_range,
    float impact_radius,
    float *damage,
    Uint32 layers,
    Uint32 team)
{
    Circle c;
    if (!self)return 0;
    if (!damage)return 0;
    
    //check if this is a damage frame
    c = gfc_circle(position.x + attack_range*facing.x,position.y,impact_radius);

    return entity_attack_update(self,gfc_shape_from_circle(c),damage,layers,team);
}

void entity_run_trigger(Entity *self)
{
    SJson *keys;
    Entity *target;
    int id = 0;
    if (!self)return;
    keys = sj_object_get_value(self->args,"keys");
    if (!keys)return;
    sj_get_integer_value(sj_object_get_value(keys,"target"),&id);
    if (!id)return;
    target = gf2d_entity_get_by_id(id);
    if (!target)return;
    if (!target->activate)return;
    target->activate(target,self);
}


/*eol@eof*/
