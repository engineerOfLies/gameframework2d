#include <time.h>
#include "gf2d_list.h"
#include "gf2d_text.h"
#include "simple_json.h"
#include "simple_logger.h"
#include "decision.h"

typedef struct
{
    float       success;
    Uint32      lastUsed;
    TextLine    choice;
}Decision;

Decision *decision_new()
{
    Decision *dec;
    dec = (Decision*)malloc(sizeof(Decision));
    if (!dec)
    {
        slog("failed to allocate memory for new decision item");
        return NULL;
    }
    memset(dec,0,sizeof(Decision));
    return dec;
}

Decision* decision_new_full(float success,Uint32 lastUsed,const char *choice)
{
    Decision *dec;
    dec = decision_new();
    if (!dec)return NULL;
    dec->success = success;
    dec->lastUsed = lastUsed;
    gf2d_line_cpy(dec->choice,choice);
    return dec;
}

void decision_free(Decision *dec)
{
    if (!dec)return;
    free(dec);
}

List *decision_list_new()
{
    List *list = gf2d_list_new();
    return list;
}

void dicision_list_free(List *decisions)
{
    int count,i;
    if (decisions)return;
    count = gf2d_list_get_count(decisions);
    for (i = 0; i < count; i++)
    {
        decision_free(gf2d_list_get_nth(decisions,i));
    }
    gf2d_list_delete(decisions);
}

List *decision_list_load(const char *filename)
{
    SJson *json,*jlist,*item;
    List *list;
    int count,i;
    float success;
    int lastUsed;
    TextLine choice;
    json = sj_load(filename);
    if (!json)return NULL;
    list = decision_list_new();
    if (!list)
    {
        sj_free(json);
        return NULL;
    }
    jlist = sj_object_get_value(json,"decisions");
    count = sj_array_get_count(jlist);
    for (i = 0; i < count; i++)
    {
        item = sj_array_get_nth(jlist,i);
        if (!item)continue;
        sj_get_float_value(sj_object_get_value(item,"success"),&success);
        sj_get_integer_value(sj_object_get_value(item,"lastUsed"),&lastUsed);
        gf2d_line_cpy(choice,sj_get_string_value(sj_object_get_value(item,"choice")));
        gf2d_list_append(list, decision_new_full(success,lastUsed,choice));
    }
    return list;
}

SJson *decision_to_json(Decision *dec)
{
    SJson *json;
    if (!dec)return NULL;
    json = sj_object_new();
    if (!json)return NULL;
    sj_object_insert(json,"success",sj_new_float(dec->success));
    sj_object_insert(json,"lastUsed",sj_new_int(dec->lastUsed));
    sj_object_insert(json,"choice",sj_new_str(dec->choice));
    return json;
}

void  decision_list_save(const char *filename,List *decisionList)
{
    int count,i;
    SJson *json,*jlist;
    Decision *dec;
    if (!decisionList)return;
    json = sj_object_new();
    if (json)
    {
        slog("failed to create json structure to save descion list");
        return;
    }
    count = gf2d_list_get_count(decisionList);
    jlist = sj_array_new();
    sj_object_insert(json,"decisions",jlist);
    for (i = 0; i < count;i++)
    {
        dec = (Decision *)gf2d_list_get_nth(decisionList,i);
        if (!dec)continue;
        sj_array_append(jlist,decision_to_json(dec));
    }
    sj_save(json,(char *)filename);
}

void decision_list_insert_choice(List *decisions, const char *choice)
{
    if (!decisions)return;
    gf2d_list_append(decisions,decision_new_full(1,0,choice));
}

Decision *decision_list_get_choice(List *decisions, const char *choice)
{
    int count,i;
    Decision *dec;
    count = gf2d_list_get_count(decisions);
    for (i = 0; i < count; i++)
    {
        dec = (Decision *)gf2d_list_get_nth(decisions,i);
        if (!dec)continue;
        if (gf2d_line_cmp(dec->choice,choice)==0)
        {
            return dec;
        }
    }
    slog("choice %s not found",choice);
    return NULL;
}

void decision_list_update_choice(List *decisions, const char *choice, float success)
{
    Decision *dec;
    if (!decisions)return;
    dec = decision_list_get_choice(decisions, choice);
    if (!dec)return;
    dec->success = (dec->success * 0.9) + (success * 0.1);
    dec->lastUsed = (Uint32)time(NULL);
}

float decision_evaluate(Decision *dec)
{
    time_t now;
    float score;
    if (!dec)return -1;
    now = time(NULL);
    score = (now - dec->lastUsed) * (1 + dec->success);
//    score = (1.0-((double)dec->lastUsed/(double)now)) + dec->success;
//    score *= dec->success;
    return score;
}

Decision *decision_list_get_best(List *decisions)
{
    int count,i;
    Decision *dec = NULL;
    Decision *best = NULL;
    float bestScore = -1,score;
    count = gf2d_list_get_count(decisions);
    for (i = 0; i < count; i++)
    {
        dec = (Decision *)gf2d_list_get_nth(decisions,i);
        if (!dec)continue;
        score = decision_evaluate(dec);
        if (score > bestScore)
        {
            bestScore = score;
            best = dec;
        }
    }
    return best;
}

const char *decision_list_get_best_choice(List *decisions)
{
    Decision *dec;
    dec = decision_list_get_best(decisions);
    if (!dec)return NULL;
    return dec->choice;
}

/*eol@eof*/
