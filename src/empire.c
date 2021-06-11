#include <simple_logger.h>
#include <simple_json.h>


#include "message_buffer.h"
#include "systems.h"
#include "planet.h"

#include "empire.h"

typedef struct
{
    Uint32  idPool;
    List   *list;
}EmpireData;

static EmpireData empire_data = {0};

void empire_give_planet_recursively(Empire *empire,Planet *planet);

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

Empire *empire_get_by_id(Uint32 id)
{
    int i,count;
    Empire *empire;
    count = gfc_list_get_count(empire_data.list);
    for (i = 0; i < count;i++)
    {
        empire = gfc_list_get_nth(empire_data.list,i);
        if (!empire)continue;
        if (empire->id == id)return empire;
    }
    return NULL;
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
    empire->id = 1 + empire_data.idPool++;
    empire->systems = gfc_list_new();
    empire->planets = gfc_list_new();
    empire->armada = gfc_list_new();
    empire->ships = gfc_list_new();
    empire->installations = gfc_list_new();
    empire->technologies = gfc_list_new();
    empire_data.list = gfc_list_append(empire_data.list,empire);
    empire->progress = sj_object_new();
    sj_object_insert(empire->progress,"surveys",sj_array_new());

    slog("generated new empire");
    return empire;
}

void empire_setup(Empire *empire)
{
    Planet *planet;
    int i,count;
    Galaxy *galaxy;

    if (!empire)return;

    galaxy = galaxy_generate(2,10);     //TODO: from config

    empire->galaxy = galaxy;
    empire->empireColor = gfc_color(0.6,0.6,1,1);

    empire->credits = 1000;
    empire->minerals = 1000;
    empire->population = 100;
    empire->agriculture = 100;

    // designate a home system near the galactic center
    empire->homeSystem = galaxy_get_nearest_system(galaxy,NULL,vector2d(0.5,0.5),0.5);
    gfc_line_sprintf(empire->empireName,"The %s Empire",empire->homeSystem->name);
    
    // TODO: be more selective in what system can be chosen for a homeSystem
    empire_give_system(empire,empire->homeSystem);
    // now give every planet in that system to the player
    count = gfc_list_get_count(empire->homeSystem->planets);
    for (i = 0; i < count;i++)
    {
        planet = gfc_list_get_nth(empire->homeSystem->planets,i);
        if (!planet)continue;
        empire_give_planet_recursively(empire,planet);
    }
}

void empire_give_system(Empire *empire,System *system)
{
    if ((!empire)||(!system))return;
    system->allegience = empire->id;
    empire->systems = gfc_list_append(empire->systems,system);
}

void empire_give_planet_recursively(Empire *empire,Planet *planet)
{
    int i,count;
    Planet *child;
    if ((!empire)||(!planet))return;
    empire_give_planet(empire,planet);
    count = gfc_list_get_count(planet->children);
    for (i = 0; i < count; i++)
    {
        child = gfc_list_get_nth(planet->children,i);
        if (!child)continue;
        empire_give_planet_recursively(empire,child);
    }
}

void empire_give_planet(Empire *empire,Planet *planet)
{
    if ((!empire)||(!planet))return;
    planet->allegience = empire->id;
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

SJson *empire_region_get_survey_json(Empire *empire,Uint32 regionId,SurveyType surveyType)
{
    int i,c,id;
    int survey_type;
    SJson *surveys,*survey;
    if (!empire)return NULL;
    surveys = sj_object_get_value(empire->progress,"surveys");
    if (!surveys)
    {
        return NULL;
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
                        return survey;
                    }
                }
            }
        }
    }
    return NULL;
}

SurveyState empire_region_get_survey_state(Empire *empire,Uint32 regionId,SurveyType surveyType)
{
    int state = 0;
    SJson *survey;
    if (!empire)return SS_Max;
    
    survey = empire_region_get_survey_json(empire,regionId,surveyType);
    if (!survey)
    {
        //not there
        return SS_Unserveyed;
    }
    sj_get_integer_value(sj_object_get_value(survey,"surveyState"),&state);
    return (SurveyState)state;
}

void empire_surveys_update(Empire *empire)
{
    int i,c;
    int state = 0;
    int startTime = 0;
    SJson *surveys,*survey;
    if (!empire)return;
    surveys = sj_object_get_value(empire->progress,"surveys");
    if (!surveys)
    {
//        slog("no surveys ordered");
        return;
    }
    // check if the survey has already been ordered
    c = sj_array_get_count(surveys);
    for (i = 0;i < c;i ++)
    {
        survey = sj_array_get_nth(surveys,i);
        if (!survey)continue;
        // do survey update
        sj_get_integer_value(sj_object_get_value(survey,"surveyState"),&state);
        if (state == SS_Completed)continue;
        sj_get_integer_value(sj_object_get_value(survey,"startTime"),&startTime);
        switch(state)
        {
            case    SS_Started:
                if (startTime < empire->gameTime)
                {
                    sj_object_delete_key(survey,"surveyState");
                    sj_object_insert(survey,"surveyState",sj_new_int((int)SS_Underway));
                }
                break;
            case    SS_Underway:
                if ((startTime + 100) < empire->gameTime)   //TODO: take this from a config
                {
                    sj_object_delete_key(survey,"surveyState");
                    sj_object_insert(survey,"surveyState",sj_new_int((int)SS_Completed));
                    message_new("survey complete");
                }
            default:
                continue;
        }
    }
}

int empire_survery_region(Empire *empire,Uint32 regionId,SurveyType surveyType)
{
    SurveyState state;
    SJson *surveys,*survey;
    if (!empire)
    {
        slog("no empire provided");
        return -1;
    }
    
    state = empire_region_get_survey_state(empire,regionId,surveyType);
    if (state == SS_Max)
    {
        return -1;
    }
    else if (state != SS_Unserveyed)
    {
        return (int)state;
    }
    surveys = sj_object_get_value(empire->progress,"surveys");
    if (!surveys)
    {
        slog("surveys list not found");
        return -1;
    }
    
    // this exact type of survey has not been started for this regionId yet
    survey = sj_object_new();// so build it
    if (!survey)
    {
        slog("failed to create object data for survey");
        return -1;
    }

    sj_object_insert(survey,"startTime",sj_new_int((int)empire->gameTime));    
    sj_object_insert(survey,"regionId",sj_new_int((int)regionId));
    sj_object_insert(survey,"surveyType",sj_new_int((int)surveyType));
    sj_object_insert(survey,"surveyState",sj_new_int((int)SS_Started));

    sj_array_append(surveys,survey);
    return SS_Started;
}

void empire_update(Empire *empire)
{
    if (!empire)
    {
        slog("no empire provided");
        return;
    }
    empire->gameTime++;
    empire_surveys_update(empire);
}

int empire_get_credits(Empire *empire)
{
    if (!empire)return 0;
    return empire->credits;
}

int empire_change_credits(Empire *empire,int credits)
{
    if (!empire)return 0;
    empire->credits += credits;
    return empire->credits;
}

/*eol@eof*/
