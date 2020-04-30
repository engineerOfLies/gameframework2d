#include "simple_logger.h"
#include "player.h"
#include "conditions.h"

int conditional_evaluate(SJson *json,Entity *player)
{
    SJson *value;
    int not = 0;
    InventoryItem *item;
    int number = 0;
    short int boolean = 0;
    const char *str = NULL;
    const char *strVal = NULL;
    if (!json)return 0;
    value = sj_object_get_value(json,"evaluate");
    sj_get_bool_value(value,&boolean);
    not = !boolean;
    value = sj_object_get_value(json,"condition");
    if (!value)
    {
        slog("no condition provide");
        return 0;
    }
    str = sj_get_string_value(value);
    value = sj_object_get_value(json,"value");
    if (!value)
    {
        slog("no value provide");
        return 0;
    }    
    strVal = sj_get_string_value(value);

    if (strcmp(str,"history")==0)
    {
        //search history for item, return if present or not
    }
    if (strcmp(str,"has_item")==0)
    {
        if (player_get_item(player, (char *)strVal))
        {
            return !not;
        }
        return not;
    }
    if (strcmp(str,"skill_level")==0)
    {
        item = player_get_skill(player, (char *)strVal);
        if (!item)
        {
            return !not;
        }
        sj_get_integer_value(sj_object_get_value(json,"number"),&number);
        if (item->skill >= number)
        {
            return !not;
        }
        return not;
    }
    if (strcmp(str,"spell_level")==0)
    {
        item = player_get_spell(player, (char *)strVal);
        if (!item)
        {
            return !not;
        }
        sj_get_integer_value(sj_object_get_value(json,"number"),&number);
        if (item->skill >= number)
        {
            return !not;
        }
        return not;
    }
    if (strcmp(str,"item_count")==0)
    {
        item = player_get_item(player, (char *)strVal);
        if (!item)
        {
            return !not;
        }
        sj_get_integer_value(sj_object_get_value(json,"number"),&number);
        if (item->count >= number)
        {
            return !not;
        }
        return not;
    }
    if (strcmp(str,"has_skill")==0)
    {
        if (player_get_skill(player, (char *)strVal))
        {
            return !not;
        }
        return not;
    }
    if (strcmp(str,"has_spell")==0)
    {
        if (player_get_spell(player, (char *)strVal))
        {
            return !not;
        }
        return not;
    }
    return 0;
}


int condition_evaluate(SJson *json,Entity *player)
{
    int i,c;
    SJson *operands;
    SJson *operand;
    SJson *value;
    const char *str;
    int and = 0;
    int retval = 0;
    int operandVal = 0;
    if (!json)return 0;
    
    value = sj_object_get_value(json,"logical");
    if (!value)
    {
        return conditional_evaluate(json,player);
    }
    str = sj_get_string_value(value);
    if (strcmp(str,"and")==0)
    {
        and = 1;
    }
    else if (strcmp(str,"or")==0)
    {
        and = 0;
    }
    else
    {
        slog("unsupported logical operation: %s",str);
        return 0;
    }
    operands = sj_object_get_value(json,"operands");
    c = sj_array_get_count(operands);
    if (!c)
    {
        slog("no operands provided for logical condition");
        return 0;
    }
    for (i = 0; i < c;i++)
    {
        operand = sj_array_get_nth(operands,i);
        if (!operand)continue;
        operandVal = condition_evaluate(operand,player);//recursion!
        if ((and)&&(!operandVal))return 0;// and with a false is all false
        if ((!and)&&(operandVal))return 1;// or with a true is all true
        retval = operandVal;
    }
    return retval;
}

/*eol@eof*/
