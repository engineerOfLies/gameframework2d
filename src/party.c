#include "simple_logger.h"
#include "player.h"
#include "party.h"


void party_set_active_player(Party *party, Entity *player)
{
    if ((!party)||(!player))return;
    party->activePlayer = player;
}

Entity *party_get_active_player(Party *party)
{
    if (!party)return NULL;
    return party->activePlayer;
}

void party_set_scene(Party *party, char *scene)
{
    if ((!party)||(!scene))
    {
        return;
    }
    gfc_line_cpy(party->scene,scene);
}

char *party_get_scene(Party *party)
{
    if (!party)return NULL;
    return party->scene;
}

void party_add_player(Party *party, Entity *player)
{
    if ((!party)||(!player))return;
    if (party->members >= PARTY_SIZE)
    {
        slog("max PARTY_SIZE exceeded, cannot add any more players!");
        return;
    }
    party->players[party->members] = player;
    party->members++;
}

Party *party_new()
{
    Party *party = NULL;
    
    party = gfc_allocate_array(sizeof(Party),1);
    if (!party)
    {
        slog("failed to allocate party data");
        return NULL;
    }
    return party;
}

void party_free(Party *party)
{
    int i;
    if (!party)return;
    for (i = 0; i < party->members;i++)
    {
        gf2d_entity_free(party->players[i]);
    }
    sj_free(party->history);
}

Party *party_load(const char *filename)
{
    SJson *json;
    if (!filename)return NULL;
    
    json = sj_load(filename);
    if (!json)return NULL;
    
    return party_parse_from_json(json);
}

void party_save(Party *party, const char *filename)
{
    SJson *json;
    
    gfc_line_cpy(party->saveFile,filename);
    
    json = party_convert_to_json(party);
    
    sj_save(json,(char *)filename);
}

Party *party_parse_from_json(SJson *json)
{
    int i,c,active;
    const char *str;
    SJson *player,*value,*players;
    Party *party = NULL;
    if (!json)return NULL;
    
    party = party_new();
    if (!party)
    {
        sj_free(json);
        return NULL;
    }
    value = sj_object_get_value(json,"saveFile");
    if (value)
    {
        str = sj_get_string_value(value);
        if (str)
        {
            gfc_line_cpy(party->saveFile,str);
        }
    }
    value = sj_object_get_value(json,"scene");
    if (value)
    {
        str = sj_get_string_value(value);
        if (str)
        {
            gfc_line_cpy(party->scene,str);
        }
    }
    value = sj_object_get_value(json,"gold");
    if (value)
    {
        sj_get_integer_value(value,&party->gold);
    }
    value = sj_object_get_value(json,"activePlayer");
    if (value)
    {
        sj_get_integer_value(value,&active);
    }
    
    players = sj_object_get_value(json,"players");
    c = sj_array_get_count(players);
    for (i = 0;i < c;i++)
    {
        player = sj_array_get_nth(players,i);
        if (!player)continue;
        party->players[i] = player_parse_from_json(player);
        if (i == active)
        {
            party->activePlayer = party->players[i];
        }
    }
    party->members = c;
    party->history = sj_copy(sj_object_get_value(json,"history"));
    if (!party->history)
    {
        party->history = sj_object_new();
    }
    sj_free(json);
    return party;
}

SJson *party_convert_to_json(Party *party)
{
    int i,active = 0;
    SJson *json = NULL,*players;
    if (!party)return NULL;
    json = sj_object_new();
  
    sj_object_insert(json,"saveFile",sj_new_str(party->saveFile));
    sj_object_insert(json,"scene",sj_new_str(party->scene));
    sj_object_insert(json,"history",sj_copy(party->history));
    sj_object_insert(json,"gold",sj_new_int(party->gold));
    players = sj_array_new();
    for (i = 0; i < party->members;i++)
    {
        if (party->activePlayer == party->players[i])
        {
            active = i;
        }
        sj_array_append(players,player_to_json(party->players[i]));
    }
    sj_object_insert(json,"activePlayer",sj_new_int(active));
    sj_object_insert(json,"players",players);
    
    return json;
}


/*eol@eof*/
