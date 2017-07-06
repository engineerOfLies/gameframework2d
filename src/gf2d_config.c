#include "gf2d_config.h"
#include "simple_logger.h"

int sj_value_as_vector2d(SJson *json,Vector2D *output)
{
    const char *text = NULL;
    float numbers[4];
    int i,count;
    SJson *value;
    if (!json)return 0;
    if (sj_is_array(json))
    {
        count = sj_array_get_count(json);
        if (count < 2)return 0;
        if (count > 2)count = 2;
        for (i = 0; i < count;i++)
        {
            value = sj_array_get_nth(json,i);
            sj_get_float_value(value,&numbers[i]);
        }
        if (output)
        {
            output->x = numbers[0];
            output->y = numbers[1];
        }
        return 1;
    }
    if (sj_is_string(json))
    {
        text = sj_get_string_value(json);
        if(sscanf(text,"%f,%f",&numbers[0],&numbers[1]) != 2)
        {
            return 0;
        }
        if (output)
        {
            output->x = numbers[0];
            output->y = numbers[1];
        }
        return 1;
    }
    return 0;
}

int sj_value_as_vector3d(SJson *json,Vector3D *output)
{
    const char *text = NULL;
    float numbers[4];
    int i,count;
    SJson *value;
    if (!json)return 0;
    if (sj_is_array(json))
    {
        count = sj_array_get_count(json);
        if (count < 3)return 0;
        if (count > 3)count = 3;
        for (i = 0; i < count;i++)
        {
            value = sj_array_get_nth(json,i);
            sj_get_float_value(value,&numbers[i]);
        }
        if (output)
        {
            output->x = numbers[0];
            output->y = numbers[1];
            output->z = numbers[2];
        }
        return 1;
    }
    if (sj_is_string(json))
    {
        text = sj_get_string_value(json);
        if(sscanf(text,"%f,%f,%f",&numbers[0],&numbers[1],&numbers[2]) != 3)
        {
            return 0;
        }
        if (output)
        {
            output->x = numbers[0];
            output->y = numbers[1];
            output->z = numbers[2];
        }
        return 1;
    }
    return 0;
}

int sj_value_as_vector4d(SJson *json,Vector4D *output)
{
    const char *text = NULL;
    float numbers[4];
    int i,count;
    SJson *value;
    if (!json)
    {
        slog("no json provided");
        return 0;
    }
    if (sj_is_array(json))
    {
        count = sj_array_get_count(json);
        if (count < 4)
        {
            slog("not enough elements in the array (%i) for a Vector4D",count);
            return 0;
        }
        if (count > 4)count = 4;
        for (i = 0; i < count;i++)
        {
            value = sj_array_get_nth(json,i);
            sj_get_float_value(value,&numbers[i]);
        }
        if (output)
        {
            output->x = numbers[0];
            output->y = numbers[1];
            output->z = numbers[2];
            output->w = numbers[3];
        }
        return 1;
    }
    if (sj_is_string(json))
    {
        text = sj_get_string_value(json);
        if(sscanf(text,"%f,%f,%f,%f",&numbers[0],&numbers[1],&numbers[2],&numbers[3]) != 4)
        {
            return 0;
        }
        if (output)
        {
            output->x = numbers[0];
            output->y = numbers[1];
            output->z = numbers[2];
            output->w = numbers[3];
        }
        return 1;
    }
    slog("json is not an array or string");
    sj_echo(json);
    return 0;
}

/*eol@eof*/
