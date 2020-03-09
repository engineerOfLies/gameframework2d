#include "exhibits.h"
#include "gf2d_config.h"

void exhibit_free(Exhibit *exhibit)
{
    if (!exhibit)return;
    sj_free(exhibit->args);
    free(exhibit);
}

Exhibit *exhibit_new()
{
    Exhibit *exhibit;
    exhibit = gfc_allocate_array(sizeof(Exhibit),1);
    return exhibit;
}

Exhibit *exhibit_load(SJson *json)
{
    Vector4D vector = {0};
    Exhibit *exhibit = NULL;
    const char *string;
    if (!json)return NULL;

    exhibit = exhibit_new();
    if (!exhibit)return NULL;
    
    gfc_line_cpy(exhibit->name,sj_get_string_value(sj_object_get_value(json,"name")));
    sj_get_bool_value(sj_object_get_value(json,"displayName"),(short int *)&exhibit->displayName);
    
    sj_value_as_vector4d(sj_object_get_value(json,"rect"),&vector);
    exhibit->rect = gf2d_rect(vector.x,vector.y,vector.z,vector.w);

    sj_get_bool_value(sj_object_get_value(json,"proximity"),(short int *)&exhibit->proximity);

    sj_value_as_vector2d(sj_object_get_value(json,"near"),&exhibit->near);

    exhibit->args = sj_copy(sj_object_get_value(json,"args"));

    string = sj_get_string_value(sj_object_get_value(json,"actor"));
    if (string)gfc_line_cpy(exhibit->actor,string);
    string = sj_get_string_value(sj_object_get_value(json,"action"));
    if (string)gfc_line_cpy(exhibit->action,string);

    return exhibit;
}

Entity *exhibit_entity_spawn(Exhibit *exhibit)
{
    Entity *ent = NULL;
    if (!exhibit)return NULL;
    
    ent = gf2d_entity_new();
    if (!ent)return NULL;
    
    
    
    return ent;
}



/*eol@eof*/
