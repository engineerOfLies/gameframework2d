#include "spawn.h"

#include "player.h"
#include "space_bug.h"

#include "gf2d_text.h"

#include "simple_logger.h"

typedef struct
{
    char   *name;
    Entity *(*spawnFunction)(Vector2D position);
}SpawnPair;


Entity * player_start(Vector2D position)
{
    Entity *player;
    player = player_get();
    if (!player)
    {
        return player_new(position);
    }
    player_set_position(position);
    return player;
}

static SpawnPair _spawnList[] = 
{
    {
        "player_start",
        player_start
    },
    {
        "space_bug",
        space_bug_new
    },
    {0,NULL}
};

Entity *spawn_entity(char *name,Vector2D position)
{
    SpawnPair *spawn;
    for (spawn = _spawnList;spawn->name != 0;spawn++)
    {
        if ((gf2d_line_cmp(spawn->name,name)==0) && (spawn->spawnFunction != NULL))
        {
            return spawn->spawnFunction(position);
        }
    }
    slog("no spawn candidate found for %s",name);
    return NULL;
}

/*eol@eof*/
