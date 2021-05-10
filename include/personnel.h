#ifndef __PERSONNEL_H__
#define __PERSONNEL_H__

#include "simple_json.h"
#include "gfc_types.h"
#include "gfc_list.h"


typedef enum
{
    PE_None,
    PE_Research,
    PE_Military,
    PE_Espionage,
    PE_Diplomacy,
    PE_Management,
    PE_Politics
}PersonnelExpertise;

typedef enum
{
    GT_None,        //they/them
    GT_Masculine,   //he/him
    GT_Feminine,    //she/her
    GT_Pan,         //they/them
    GT_Mechanoid    //it/its
}GenderType;

typedef enum
{
    ST_Terran,
    ST_Orocutani,
    ST_Gabori,
    ST_TreMaKen,
    ST_DuShocken,
    ST_Chakta,
    ST_Levigoth,
    ST_Technorganic,
    ST_Mechanoid
}SpeciesTypes;

typedef struct
{
    TextLine    name;
    TextLine    family;
    TextLine    state;          /**<current state: hired/unhired/injured/compromized/dead*/
    TextLine    assignment;     /**<what they are currently working on.  This will need to be parsed.  Research:Thermo*/
    Uint32      system;         /**<location*/
    Uint32      planet;         /**<location*/
    Uint32      region;         /**<location*/
    Uint32      allegience;     /**<to whom do they work*/
    Uint32      loyalty;        /**<how loyal to the above they are*/
    Uint32      id;             /**<unique ID for the game*/
    Uint32      species;        /**<in case it comes up*/
    Uint32      gender;         /**<see GenderType*/
    Uint32      age;            /**<time to live*/
    Uint32      occupation;     /**<occupation ID*/
    Uint32      expertise;      /**<how good they are at it*/
    TextLine    specialty;      /**<what they are best at in their occupation*/
}Personnel;

Personnel *personnel_generate(Uint32 id);
void personnel_list_free(List *list);
List *personnel_list_get_from_json(SJson *json);
SJson *personnel_list_to_json(List *personnelList);
Personnel *personnel_get_by_id(List *personnelList, Uint32 id);

void personnel_free(Personnel *person);
Personnel *personnel_new();
Personnel *personnel_load_from_json(SJson *json);
SJson *personnel_save_to_json(Personnel *person);

#endif
