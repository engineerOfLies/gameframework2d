#include "spawn.h"
#include "level.h"
#include "player.h"
#include "monster.h"
#include "pickup.h"
#include "simple_logger.h"

static Spawn spawnlist[] = 
{
    {
        "player_start",
        player_spawn
    },
    {
        "monster_start",
        monster_spawn
    },
    {
        "pickup_start",
        pickup_spawn
    },
    {0}
};

void spawn_entity(const char *name,Vector2D position,SJson *args)
{
    Spawn *spawn;
    if (!name)
    {
        slog("no spawn name provided");
        return;
    }
    for (spawn = spawnlist; spawn->name != 0; spawn++)
    {
        if (strcmp(spawn->name,name)==0)
        {
            if (spawn->spawn)
            {
                spawn->spawn(position,args);
                return;
            }
        }
    }
    slog("failed to spawn entity %s",name);
}


/*eol@eof*/
