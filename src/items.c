#include "items.h"
#include "level.h"
#include "simple_logger.h"
#include "camera.h"

typedef struct
{
    Item *items;
    Uint32 count;
}ItemManager;

static ItemManager item_manager = {NULL,0};

void item_draw(Entity *self);
void item_think(Entity *self);
void item_update(Entity *self);
int  item_touch(Entity *self,Entity *other);
int  heal_touch(Entity *self,Entity *other);
void item_damage(Entity *self,int amount, Entity *source);
void item_die(Entity *self);

void item_manager_close()
{
    if (item_manager.items != NULL)
    {
        free (item_manager.items);
    }
    slog("item manager closed");
}

void item_manager_init()
{
    item_manager.count = 0;
    item_manager.items = NULL;
    atexit(item_manager_close);
    slog("item manager initialized");
}

Item *item_get_by_name(char *name)
{
    int i = 0;
    slog("searching for %s",name);
    for (i = 0; i < item_manager.count;i++)
    {
        slog("checking %s",item_manager.items[i].name);
        if (gf2d_line_cmp(item_manager.items[i].name,name) == 0)
        {
            slog("found it!");
            return &item_manager.items[i];
        }
    }
    slog("failed to find an item named '%s'",name);
    return NULL;
}

Entity *item_spawn(char *name,Vector2D position)
{
    Entity *self;
    Item *item;
    item = item_get_by_name(name);
    if (!item)
    {
        slog("item not found, not spawning");
        return NULL;
    }
    self = gf2d_entity_new();
    if (!self)
    {
        slog("no entities left, not spawning");
        return NULL;
    }
    gf2d_line_cpy(self->name,item->name);
    self->parent = NULL;
    
    self->shape = gf2d_shape_circle(0,0, item->radius);
    gf2d_body_set(
        &self->body,
        item->name,
        LAYER_ITEMS,
        2,
        position,
        vector2d(0,0),
        10,
        0,
        0,
        &self->shape,
        self,
        NULL,
        NULL);

    gf2d_actor_load(&self->actor,item->actor);
    gf2d_actor_set_action(&self->actor,"idle");

    self->state = ES_Idle;
    
    vector2d_copy(self->position,position);
    
    vector2d_copy(self->scale,self->actor.al->scale);
    vector2d_set(self->scaleCenter,self->actor.al->frameWidth/2,self->actor.al->frameHeight/2);
    vector3d_set(self->rotation,self->actor.al->frameWidth/2,self->actor.al->frameHeight/2,gf2d_random()*360);
    
    self->pe = gf2d_particle_emitter_new(50);
    
    self->think = item_think;
    self->draw = item_draw;
    self->update = item_update;
    self->damage = NULL;
    self->die = item_die;
    self->free = level_remove_entity;
    switch (item->type)
    {
        case IT_HEAL:
            self->touch = heal_touch;
            break;
        case IT_NONE:
        case IT_AMMO:
        case IT_CHARGE:
            self->touch = item_touch;
    }

    self->count = item->count;
    
    level_add_entity(self);
    return self;
}

void item_draw(Entity *self)
{
    
}

void item_think(Entity *self)
{
}

void item_update(Entity *self)
{
    Rect camera;
    if (!self)return;
    camera = camera_get_dimensions();
    if (self->position.x < camera.x - 128)
    {
        self->dead = 1;
        return;
    }    
}

int  item_touch(Entity *self,Entity *other)
{
    self->dead = 1;
    return 0;
}

int  heal_touch(Entity *self,Entity *other)
{
    if ((!other)||(!self))return 0;
    other->health += self->count;
    other->health = MIN(other->health,other->maxHealth);
    self->dead = 1;
    return 1;
}

void item_damage(Entity *self,int amount, Entity *source)
{
    return;
}

void item_die(Entity *self)
{
    
}

int item_file_get_count(FILE *file)
{
    char buf[512];
    int count = 0;
    if (!file)return 0;
    rewind(file);
    while(fscanf(file, "%s", buf) != EOF)
    {
        if(strcmp(buf,"name:") == 0)
        {
            count++;
        }
        fgets(buf, sizeof(buf), file);
    }
    return count;
}

void item_file_load_items(FILE *file,Item *item)
{
    char buf[512];
    if (!file)return;
    rewind(file);
    item--;
    while(fscanf(file, "%s", buf) != EOF)
    {
        if(strcmp(buf,"name:") == 0)
        {
            item++;
            fscanf(file,"%s",(char*)&item->name);
            continue;
        }
        if(strcmp(buf,"actor:") == 0)
        {
            fscanf(file,"%s",(char*)&item->actor);
            continue;
        }
        if(strcmp(buf,"type:") == 0)
        {
            fscanf(file,"%s",buf);
            if (strcmp(buf,"heal")==0)
            {
                item->type = IT_HEAL;
            }
            continue;
        }
        if(strcmp(buf,"count:") == 0)
        {
            fscanf(file,"%i",&item->count);
            continue;
        }
        if(strcmp(buf,"radius:") == 0)
        {
            fscanf(file,"%f",&item->radius);
            continue;
        }
        fgets(buf, sizeof(buf), file);
    }
}


void item_load_list(char *filename)
{
    FILE *file;
    int count;
    file = fopen(filename,"r");
    if (!file)
    {
        slog("failed to load item file: %s",filename);
        return;
    }
    count = item_file_get_count(file);
    if (!count)
    {
        slog("no items loaded!");
        fclose(file);
        return;
    }
    item_manager.items = (Item*)malloc(sizeof(Item)*count);
    if (!item_manager.items)
    {
        slog("failed to allocate memory for item list");
        fclose(file);
        return;
    }
    memset(item_manager.items,0,sizeof(Item)*count);
    item_file_load_items(file,item_manager.items);
    item_manager.count = count;
    fclose(file);
}


/*eol@eof*/
