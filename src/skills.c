#include "skills.h"
#include "simple_json.h"
#include "simple_logger.h"

static List *skills_list = NULL;

void skill_parse(SJson *object);


void skill_free(Skill *skill)
{
    int count,i;
    char *tag;
    if (!skill)return;
    count = gf2d_list_get_count(skill->tags);
    for (i = 0; i < count; i++)
    {
        tag = gf2d_list_get_nth(skill->tags,i);
        if (!tag)continue;
        free(tag);
    }
    gf2d_list_delete(skill->tags);
    count = gf2d_list_get_count(skill->animation);
    for (i = 0; i < count; i++)
    {
        tag = gf2d_list_get_nth(skill->animation,i);
        if (!tag)continue;
        free(tag);
    }
    gf2d_list_delete(skill->animation);
    free(skill);
}


void skill_list_close()
{
    int count,i;
    Skill *skill;
    if (!skills_list)return;
    count = gf2d_list_get_count(skills_list);
    for (i = 0; i < count; i++)
    {
        skill = gf2d_list_get_nth(skills_list,i);
        if (!skill)continue;
        skill_free(skill);
    }
    gf2d_list_delete(skills_list);
}

void skill_list_init()
{
    skills_list = gf2d_list_new();
    atexit(skill_list_close);
}

void skill_list_load(char *filename)
{
    int count,i;
    SJson *json,*value,*list;
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to load json skill file");
        return;
    }
    skill_list_init();
    list = sj_object_get_value(json,"skilllist");
    if (!list)return;
    count = sj_array_get_count(list);
    for (i = 0;i < count; i++)
    {
        value = sj_array_get_nth(list,i);
        if (!value)continue;
        skill_parse(value);
    }
    sj_free(json);
}

Skill *skill_new()
{
    Skill *skill;
    skill = (Skill*)malloc(sizeof(Skill));
    if (!skill)
    {
        slog("failed to allocate memory for new skill");
        return NULL;
    }
    memset(skill,0,sizeof(Skill));
    skill->tags = gf2d_list_new();
    skill->animation = gf2d_list_new();
    gf2d_list_append(skills_list,skill);
    return skill;
}

void skill_parse(SJson *object)
{
    Skill *skill;
    SJson *tags;
    int count,i;
    const char *tag;
    if (!object)return;
    skill = skill_new();
    if (!skill)return;
    gf2d_line_cpy(skill->name,sj_get_string_value(sj_object_get_value(object,"name")));
    sj_get_float_value(sj_object_get_value(object,"windup"),&skill->windup);
    sj_get_float_value(sj_object_get_value(object,"actionTime"),&skill->actionTime);
    sj_get_float_value(sj_object_get_value(object,"recover"),&skill->recover);
    sj_get_float_value(sj_object_get_value(object,"healthCost"),&skill->healthCost);
    sj_get_float_value(sj_object_get_value(object,"manaCost"),&skill->manaCost);
    sj_get_float_value(sj_object_get_value(object,"staminaCost"),&skill->staminaCost);
    sj_get_float_value(sj_object_get_value(object,"damageMod"),&skill->damageMod);
    tags = sj_object_get_value(object,"tags");
    if (!tags)return;
    count = sj_array_get_count(tags);
    for (i = 0; i < count; i++)
    {
        tag = sj_get_string_value(sj_array_get_nth(tags,i));
        if (!tag)continue;
        gf2d_list_append(skill->tags,gf2d_text_copy(tag));
    }
    tags = sj_object_get_value(object,"animation");
    if (!tags)return;
    count = sj_array_get_count(tags);
    for (i = 0; i < count; i++)
    {
        tag = sj_get_string_value(sj_array_get_nth(tags,i));
        if (!tag)continue;
        gf2d_list_append(skill->animation,gf2d_text_copy(tag));
    }

}

Skill *skill_get_by_name(char *name)
{
    Skill *skill = NULL;
    int count,i;
    if (!name)return NULL;
    count = gf2d_list_get_count(skills_list);
    for (i = 0; i < count; i++)
    {
        skill = (Skill *)gf2d_list_get_nth(skills_list,i);
        if (!skill)continue;
        if (gf2d_line_cmp(skill->name,name) == 0)return skill;
    }
    return NULL;
}

Uint8 skill_check_tag(Skill *skill, char *check)
{
    char *tag;
    int count,i;
    if (!skill)return 0;
    count = gf2d_list_get_count(skill->tags);
    for (i = 0; i < count; i++)
    {
        tag = (char *)gf2d_list_get_nth(skill->tags,i);
        if (!tag)continue;
        if (gf2d_line_cmp(tag,check) == 0)return 1;
    }
    return 0;
}


/*eol@eof*/
