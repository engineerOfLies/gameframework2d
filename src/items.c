#include "items.h"
#include "level.h"
#include "simple_logger.h"

typedef struct
{
    Item *items;
    Uint32 count;
}ItemManager;

static ItemManager item_manager;

void item_manager_close()
{
    if (item_manager.items != NULL)
    {
        free (item_manager.items);
    }
}

void item_manager_init()
{
    item_manager_close();
    item_manager.count = 0;
    item_manager.items = NULL;
    atexit(item_manager_close);
}

Item *item_get_by_name(char *name)
{
    int i = 0;
    for (i = 0; i < item_manager.count;i++)
    {
        if (gf2d_line_cmp(item_manager.items[i].name,name) == 0)
        {
            return &item_manager.items[i];
        }
    }
    return NULL;
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
        if(strcmp(buf,"sprite:") == 0)
        {
            fscanf(file,"%s",(char*)&item->sprite);
            continue;
        }
        if(strcmp(buf,"actor:") == 0)
        {
            fscanf(file,"%s",(char*)&item->actor);
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
    item_manager_close();//free anything already loaded just in case
    item_manager.items = (Item*)malloc(sizeof(Item)*count);
    if (!item_manager.items)
    {
        slog("failed to allocate memory for item list");
        fclose(file);
        return;
    }
    item_file_load_items(file,item_manager.items);
}


/*eol@eof*/
