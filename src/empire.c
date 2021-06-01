#include <simple_logger.h>
#include <simple_json.h>


#include "galaxy.h"
#include "systems.h"
#include "planet.h"

#include "empire.h"

typedef struct
{
    Uint32  idPool;
    List   *list;
}EmpireData;

static EmpireData empire_data = {0};


void empire_close()
{
    Empire *empire;
    int i,count;
    count = gfc_list_get_count(empire_data.list);
    for (i = 0;i < count;i++)
    {
        empire = gfc_list_get_nth(empire_data.list,i);
        if (!empire)continue;
        empire_free(empire);
    }
    gfc_list_delete(empire_data.list);
    memset(&empire_data,0,sizeof(EmpireData));
    slog("empire system closed");
}

void empire_init()
{
    empire_data.idPool = 1;
    empire_data.list = gfc_list_new();
    atexit(empire_close);
    slog("empire system initialized");
}

Empire *empire_new()
{
    Empire *empire;
    
    empire = gfc_allocate_array(sizeof(Empire),1);
    
    if (!empire)
    {
        slog("failed to allocate memory for a new empire");
        return NULL;
    }
    empire->id = empire_data.idPool++;
    empire->systems = gfc_list_new();
    empire->planets = gfc_list_new();
    empire->armada = gfc_list_new();
    empire->ships = gfc_list_new();
    empire->technologies = gfc_list_new();
    empire_data.list = gfc_list_append(empire_data.list,empire);
    empire->progress = sj_object_new();
    slog("generated new empire");
    return empire;
}

void empire_give_system(Empire *empire,System *system)
{
    if ((!empire)||(!system))return;
    empire->systems = gfc_list_append(empire->systems,system);
}

void empire_give_planet(Empire *empire,Planet *planet)
{
    if ((!empire)||(!planet))return;
    empire->planets = gfc_list_append(empire->planets,planet);
}

void empire_set_home_system(Empire *empire,System *system)
{
    if ((!empire)||(!system))return;
    empire->homeSystem = system;
}

void empire_set_home_planet(Empire *empire,Planet *planet)
{
    if ((!empire)||(!planet))return;
    empire->homeWorld = planet;
}

Empire *empire_generate()
{
    Empire *empire;
    empire = empire_new();
    if (!empire)return NULL;
    
    return empire;
}

void empire_free(Empire *empire)
{
    if (!empire)return;
    slog("freeing empire");
    gfc_list_delete(empire->systems);
    gfc_list_delete(empire->planets);
    gfc_list_delete(empire->armada);
    gfc_list_delete(empire->ships);
    sj_free(empire->progress);
    free(empire);
}

SurveyState empire_region_get_survey_state(Empire *empire,Uint32 regionId,SurveyType surveyType)
{
    int i,c,id,state = 0;
    int survey_type;
    SJson *surveys,*survey;
    if (!empire)return SS_Max;
    surveys = sj_object_get_value(empire->progress,"surveys");
    if (!surveys)
    {
        return SS_Max;
    }
    // check if the survey has already been ordered
    c = sj_array_get_count(surveys);
    for (i = 0;i < c;i ++)
    {
        survey = sj_array_get_nth(surveys,i);
        if (!survey)continue;
        if (sj_get_integer_value(sj_object_get_value(survey,"regionId"),&id))
        {
            if (id == regionId)
            {
                //started a survery with this region already
                if (sj_get_integer_value(sj_object_get_value(survey,"surveyType"),&survey_type))
                {
                    if (survey_type == surveyType)
                    {
                        // this exact survey type has already been started
                        sj_get_integer_value(sj_object_get_value(survey,"surveyState"),&state);
                        return (SurveyState)state;
                    }
                }
            }
        }
    }
    return SS_Unserveyed;
}

int empire_survery_region(Empire *empire,Uint32 regionId,SurveyType surveyType)
{
    SurveyState state;
    SJson *surveys,*survey;
    if (!empire)return -1;
    
    state = empire_region_get_survey_state(empire,regionId,surveyType);
    if (state == SS_Max)
    {
        surveys = sj_array_new();
        if (!surveys)
        {
            slog("failed to allocate memory for survey progress!!");
            return -1;
        }
        sj_object_insert(empire->progress,"surveys",surveys);
    }
    else if (state != SS_Unserveyed)
    {
        return (int)state;
    }
    surveys = sj_object_get_value(empire->progress,"surveys");

    
    // this exact type of survey has not been started for this regionId yet
    survey = sj_object_new();// so build it
    if (!survey)
    {
        slog("failed to create object data for survey");
        return -1;
    }
    
    sj_object_insert(survey,"regionId",sj_new_int((int)regionId));
    sj_object_insert(survey,"surveyType",sj_new_int((int)surveyType));
    sj_object_insert(survey,"surveyState",sj_new_int((int)SS_Started));

    sj_array_append(surveys,survey);
    return SS_Started;
}

/*eol@eof*/
