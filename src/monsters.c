#include "simple_logger.h"

#include "particle_effects.h"
#include "entity_common.h"
#include "level.h"
#include "player.h"
#include "monsters.h"

Vector2D monster_direction_to_player(Entity *self);

Entity *monster_spawn_common(Vector2D position,Uint32 id, SJson *args,const char *entityDef)
{
    Entity *self;
    self = gf2d_entity_new();
    if (!self)return NULL;
    self->id = id;
    self->parent = NULL;
    self->state = ES_Idle;
    self = entity_config_from_file(self,args,position,entityDef);
    
    self->think = monster_think;
    self->update = monster_update;
    self->touch = NULL;
    self->damage = monster_damage;
    self->die = monster_die;
    self->nextattack = -1;
    self->args = args;
    return self;
}

Entity *monster_spawn_generic(Vector2D position,Uint32 id, SJson *args,const char *entityDef)
{
    Entity *self = monster_spawn_common(position,id,args,entityDef);
    if (!self)return NULL;
    level_add_entity(self);
    return self;
}

float monster_get_player_range(Entity *self)
{
    Entity *player = player_get();
    if ((!self)||(!player))return -1;
    return vector2d_magnitude(vector2d(self->body.position.x - player->body.position.x,self->body.position.y - player->body.position.y));
}

int monster_player_in_range(Entity *self,float range)
{
    Entity *player = player_get();
    if (!player)return 0;
    if (vector2d_magnitude_compare(vector2d(self->body.position.x - player->body.position.x,self->body.position.y - player->body.position.y),range) < 0)
        return 1;
    return 0;
}

int monster_player_sight_check(Entity *self)
{
    Entity *sight;
    CollisionFilter filter = {0};
    Entity *player = player_get();
    if (!player)return 0;
    if (monster_player_in_range(self,self->sight_range))
    {
        //in range, check for obstructions
        filter.cliplayer = self->body.touchlayer;
        filter.worldclip = 1;
        sight = entity_scan_hit(self,self->body.position,player->body.position,filter,NULL);
        if (sight != player)return 0;

        gf2d_entity_play_sound(self, "alert");
        return 1;
    }
    return 0;
}

void monster_think_patroling(Entity *self)
{
    if (monster_player_sight_check(self))
    {
        self->think = monster_think_hunting;
        return;
    }
    if ((!entity_platform_end_check(self))||entity_wall_check(self, vector2d(3 *self->facing.x,0)))
    {
        monster_turn(self,self->facing.x * -1);
    }
    self->body.velocity.x = self->speed * self->facing.x;
    if (strcmp(gf2d_entity_get_action(self),"walk")!= 0)gf2d_entity_set_action(self, "walk");
}

void monster_think_flying_patroling(Entity *self)
{
    if (monster_player_sight_check(self))
    {
        self->think = monster_think_flying_hunting;
        return;
    }
    if (entity_wall_check(self, vector2d(3 *self->facing.x,0)))
    {
        monster_turn(self,self->facing.x * -1);
    }
    self->body.velocity.x = self->speed * self->facing.x;
    self->body.velocity.y = 0;//maintain elevation
}


MonsterAttack *monster_get_attack_by_index(Entity *self,int index)
{
    if (!self)return NULL;
    if (!self->attacks)return NULL;
    return gfc_list_get_nth(self->attacks,index);
}

void monster_attack(Entity *self)
{
    MonsterAttack *attack;
    if ((!self)||(self->nextattack < 0))return;
    
    attack = monster_get_attack_by_index(self,self->nextattack);
    if (!attack)return;
    self->state = ES_Attacking;
    gf2d_entity_set_action(self, attack->action);
    self->cooldown = gf2d_entity_get_action_frames_remaining(self) * 2;
    self->connectedAttack = 0;
    self->attack = self->nextattack;
    self->attack_range = attack->attack_range;
    self->think = monster_think_attacking;
    self->nextattack = -1;
    gf2d_entity_play_sound(self, attack->sound);
}

void monster_body_attack(Entity *self)
{
    Shape shape;
    EntityLink *link;
    MonsterAttack *attack;
    if ((!self)||(!self->figureInstance.figure)||(!self->figureInstance.figure))return;
    attack = monster_get_attack_by_index(self,self->attack);
    if (!attack)return;
    link = gf2d_entity_link_get(self,attack->link);
    if (!link)return;
    shape = gf2d_body_to_shape(&link->body);
    
    self->cooldown = self->attackCooldown;
    self->connectedAttack = entity_attack_update(
        self,
        shape,
        attack->damages,
        self->body.touchlayer,
        self->body.team);
    gf2d_entity_play_sound(self,attack->sound);
}

void monster_self_attack(Entity *self)
{
    Circle impact;
    self->cooldown = self->attackCooldown;
    vector2d_copy(impact,self->body.position);
    impact.r = self->attack_range;
    self->connectedAttack = entity_attack_update(
        self,
        gfc_shape_from_circle(impact),
        self->damages,
        self->body.touchlayer,
        self->body.team);
    gf2d_entity_play_sound(self, "attack");
}

int monster_attack_check_frame(MonsterAttack *attack, int frame)
{
    int i,c,f;
    if (!attack)return 0;
    c = gfc_list_get_count(attack->attack_frames);
    for (i = 0; i < c; i++)
    {
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
        f = (int)gfc_list_get_nth(attack->attack_frames,i);
        if (f == frame)return 1;
    }
    return 0;
}

void monster_think_attacking(Entity *self)
{
    MonsterAttack *attack;
    if (!self)return;
    monster_face_player(self);

    attack = monster_get_attack_by_index(self,self->attack);
    if (!attack)return;
    
    if (self->last_attack_frame == (int)self->figureInstance.frame)return;//don't attack more than once per frame
    self->last_attack_frame = (int)self->figureInstance.frame;
    if (monster_attack_check_frame(attack, (int)self->figureInstance.frame))
    {
        //only on attack frames do this:
        if (strcmp(attack->type,"melee")==0)
        {
            self->connectedAttack = melee_thrust_update(self);
        }
        else if (strcmp(attack->type,"self")==0)
        {
            monster_self_attack(self);
        }
        else if (strcmp(attack->type,"body")==0)
        {
            monster_body_attack(self);
        }
    }
    if (gf2d_entity_get_action_frames_remaining(self) <= 0)
    {
        self->think = monster_think_hunting;
        self->state = ES_Idle;
    }
}

void monster_update(Entity *self)
{
    const char *action;
    if (self->jumpcool > 0) self->jumpcool -= 0.2;
    else self->jumpcool = 0;
    if (self->damageCooldown)
    {
        self->damageCooldown -= 1;
        if (self->damageCooldown < 0)self->damageCooldown = 0;
    }

    if (self->cooldown > 0) self->cooldown--;
    if (self->cooldown < 0) self->cooldown = 0;
    
    // walk dampening
    if (self->body.velocity.x)
    {
        self->body.velocity.x *= 0.6;
        if (fabs(self->body.velocity.x) < 1)self->body.velocity.x = 0;
    }
    if (self->state == ES_Idle)
    {
        if (gf2d_entity_get_action_frames_remaining(self) == 0)
        {
            action = gf2d_entity_get_action(self);
            if ((action)&&(strcmp(action,"idle") != 0))
            {
                gf2d_entity_set_action(self,"idle");
            }
        }
    }
    entity_apply_gravity(self);
    entity_world_snap(self);    // error correction for collision system
}

void monster_turn(Entity *self,int dir)
{
    if (dir < 0)
    {
        self->facing.x = -1;
        self->flip.x = 0;
    }
    else if (dir > 0)
    {
        self->facing.x = 1;
        self->flip.x = 1;
    }
}

MonsterAttack *monster_get_best_attack_by_range(Entity *self,float range,int *out)
{
    int i,c;
    float distance;
    float best_range = -1;
    int index = -1;
    MonsterAttack *attack, *best = NULL;
    if (!self)return NULL;
    c = gfc_list_get_count(self->attacks);
    for (i = 0; i < c; i++)
    {
        attack = gfc_list_get_nth(self->attacks,i);
        if (!attack)continue;
        if ((range >= attack->min_range)&&(range <= attack->max_range))
        {
            // can't do better than the target attack
            if (out)*out = i;
            return attack;
        }
        if (range < attack->min_range)
        {
            distance = fabs(range - attack->min_range);
            if (distance < best_range)
            {
                best = attack;
                best_range = distance;
                index = i;
            }
        }
        else if (range > attack->max_range)
        {
            distance = fabs(range - attack->max_range);
            if (distance < best_range)
            {
                best = attack;
                best_range = distance;
                index = i;
            }
        }
    }
    if (out)*out = index;
    return best;
}

void monster_choose_attack(Entity *self)
{
    MonsterAttack *attack;
    float range;
    int index = 0;
    Entity *player;
    if (!self)return;
    player = player_get();
    if (!player)return;
    range = monster_get_player_range(self);
    attack = monster_get_best_attack_by_range(self,range,&index);
    if (!attack)return;// nothing to do
    self->attack_range = attack->attack_range;
//    self->attack_range = (attack->min_range + attack->max_range) *0.5;
    self->nextattack = index;
    self->last_attack_frame = -1;//reset
    //aim for the middle
}

void monster_jump(Entity *self)
{
    if (!self)return;
    if (!self->grounded)return;
    if (self->jumpcool)return;
    if (!self->jump_height)return;
    self->jumpcool = 20;
    self->body.velocity.y = -self->jump_height;
    gf2d_entity_set_action(self, "jump");
}

void monster_face_player(Entity *self)
{
    Entity *player = player_get();
    if ((player->body.position.x - self->body.position.x) < -2)
    {
        monster_turn(self,-1);
    }
    else if ((player->body.position.x - self->body.position.x) > 2)
    {
        monster_turn(self,1);
    }
}

void monster_fly_to_player(Entity *self)
{
    Entity *player = player_get();
    if ((!self)||(!player))return;
        // fly to player
    if (strcmp(gf2d_entity_get_action(self),"flying")!= 0)gf2d_entity_set_action(self, "flying");
    monster_face_player(self);

    if ((player->body.position.x - self->body.position.x) < -(self->attack_range*0.5))
    {
        self->body.velocity.x -= self->speed;
    }
    else if ((player->body.position.x - self->body.position.x) > (self->attack_range * 0.5))
    {
        self->body.velocity.x += self->speed;
    }
    else
    {
        self->body.velocity.x = 0;
    }
    
    if (((player->body.position.y - 14) - self->body.position.y) < -(self->attack_range * 0.5))
    {
        self->body.velocity.y -= self->speed;
    }
    else if (((player->body.position.y - 14) - self->body.position.y) > (self->attack_range * 0.5))
    {
        self->body.velocity.y += self->speed;
    }
    else
    {
        self->body.velocity.y = 0;
    }
    vector2d_set_magnitude(&self->body.velocity,self->speed);
}

void monster_think_flying_hunting(Entity *self)
{
    Entity *player = player_get();
    if (self->cooldown)return;
    //monster loses sight of player
    if (!monster_player_in_range(self,self->sight_range *2))
    {
        self->think = monster_think_flying_patroling;// idle think
        return;
    }
    
    if (vector2d_magnitude_compare(
        vector2d(self->body.position.x - player->body.position.x,self->body.position.y - (player->body.position.y - 14)),
        self->attack_range) <= 0)
    {
        monster_self_attack(self);
        vector2d_clear(self->body.velocity);
        return;
    }
    
    monster_fly_to_player(self);
}

void monster_walk_to_player(Entity *self)
{
    Entity *player = player_get();
    if ((!self)||(!player))return;
    if (self->cooldown)return;
    if ((player->body.position.x - self->body.position.x) <= -self->speed)
    {
        self->body.velocity.x -= self->speed;
        self->cooldown = 1;
        if (entity_wall_check(self, vector2d(-5,0)))
        {
            monster_jump(self);
        }
        else if (strcmp(gf2d_entity_get_action(self),"walk")!= 0)gf2d_entity_set_action(self, "walk");
    }
    else if ((player->body.position.x - self->body.position.x) > self->speed)
    {
        self->body.velocity.x += self->speed;
        self->cooldown = 1;
        if (entity_wall_check(self, vector2d(5,0)))
        {
            monster_jump(self);
        }
        else if (strcmp(gf2d_entity_get_action(self),"walk")!= 0)gf2d_entity_set_action(self, "walk");
    }
    else
    {
        self->body.velocity.x = 0;
        gf2d_entity_set_action(self, "idle");
    }
}

int monster_lost_player(Entity *self)
{
    if (!self)return 1;
    if (!monster_player_in_range(self,self->sight_range *2))
    {
        return 1;
    }
    return 0;
}

void monster_think_hunting(Entity *self)
{
    if (self->cooldown)return;
    //monster loses sight of player
    monster_face_player(self);
    //monster gets in range of player
    if (monster_lost_player(self))
    {
        self->think = monster_think_patroling;
        return;
    }
    if (self->nextattack == -1)
    {
        monster_choose_attack(self);
    }
    if (monster_player_in_range(self,self->attack_range))
    {
        monster_attack(self);
        return;
    }
    // walk to player
    monster_walk_to_player(self);
}

Vector2D monster_direction_to_player(Entity *self)
{
    Entity *player = player_get();
    Vector2D dir = {0};
    if ((!self)||(!player))return dir;
    vector2d_sub(dir,player->body.position,self->body.position);
    vector2d_normalize(&dir);
    return dir;
}

float monster_damage(Entity *self,float *amount, Entity *source)
{
    const char *action;
    float damage = 0;
    Vector2D dir;
    if ((!self)||(!amount))return 0;
    if (self->damageCooldown)return 0;
    damage = entity_calc_damage(amount,self->armor);
    self->health -= damage;
    if ((self->state < ES_Charging)||(self->state > ES_Attacking))
    {
        action = gf2d_entity_get_action(self);
        if ((action)&&(strcmp(action,"pain")!=0))
        {
            gf2d_entity_set_action(self,"pain");
        }
    }
    gf2d_entity_play_sound(self, "pain");
    if (source)
    {   
        vector2d_sub(dir,self->body.position,source->body.position);
    }
    else
    {
        dir.y = -1;
        dir.x = gfc_crandom();
    }
    vector2d_normalize(&dir);
    particle_spray(
        self->body.position,
        dir,
        gfc_color8(200,200,200,200),//from config
        40*damage);
    self->damageCooldown = 50;
    if (self->health <= 0)
    {
        self->health = 0;
        self->state = ES_Dying;
        gf2d_entity_set_action(self, "death");
        self->think = self->die;
        self->body.cliplayer = 0;
        return 0;
    }
    if (self->state == ES_Inactive)
    {
        self->state = ES_Idle;
        if (self->activate)self->activate(self,source);
    }
    return 0;
}

void monster_die(Entity *self)
{
    entity_run_trigger(self);
    //leave the body
    //check if there were any drops, if so drop em
    if (!gf2d_entity_get_action_frames_remaining(self))
    {
        self->think = NULL;
        self->damage = NULL;
        //if there is a drop, do it here
    }
}


void monster_think(Entity *self)
{
    if (monster_player_sight_check(self))
    {
        self->think = monster_think_hunting;
        self->cooldown = self->attackCooldown;
    }
}

int melee_thrust_update(
    Entity *self)
{
    MonsterAttack *attack;
    if (!self)return 0;
    attack = monster_get_attack_by_index(self,self->attack);
    if (!attack)return 0;
    return entity_melee_thrust_update(
        self,
        self->body.position,
        self->facing,
        attack->attack_range,
        15,//TODO: from config
        attack->damages,
        self->body.touchlayer,
        self->body.team);
}


/*eol@eof*/
