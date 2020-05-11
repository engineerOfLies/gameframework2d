#ifndef __PARTY_H__
#define __PARTY_H__

#include "simple_json.h"

#include "gfc_vector.h"

#include "gf2d_entity.h"

#define PARTY_SIZE 4

typedef struct
{
    TextLine    saveFile;               /**<the save file used for this party*/
    Entity     *players[PARTY_SIZE];
    Entity     *activePlayer;           /**<which player was the active player*/
    Uint32      members;
    int         gold;
    TextLine    scene;
    SJson      *history;
}Party;


/**
 * @brief get the scene associated with the party data
 */
char *party_get_scene(Party *party);

/**
 * @brief set the scene for the party info
 */
void party_set_scene(Party *party, char *scene);

/**
 * @brief set the active player for the party
 */
void party_set_active_player(Party *party, Entity *player);

/**
 * @brief get the active player for the party
 */
Entity *party_get_active_player(Party *party);

/**
 * @brief add a player to the party
 */
void party_add_player(Party *party, Entity *player);

/**
 * @brief allocate a new party
 */
Party *party_new();

/**
 * @brief free a party
 */
void party_free(Party *party);

/**
 * @brief load a party from file
 */
Party *party_load(const char *filename);

/**
 * @brief save party data to file
 */
void party_save(Party *party, const char *filename);

/**
 * @brief parse party data from json
 */
Party *party_parse_from_json(SJson *json);

/**
 * @brief convert party data to json
 */

SJson *party_convert_to_json(Party *party);

#endif
