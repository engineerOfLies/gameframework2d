#include "simple_logger.h"
#include "gf2d_config.h"
#include "windows_common.h"
#include "camera.h"
#include "player.h"
#include "exhibits.h"

static int exhibit_paused = 0;

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

void exhibit_unpause(void *data)
{
    exhibit_paused = 0;
}

void exhibit_set_scene(Exhibit *exhibit,Scene *scene)
{
    if ((!exhibit)||(!scene))return;
    exhibit->scene = scene;
}

void exhibit_player_walk_to(Exhibit *exhibit)
{
    if (!exhibit)return;
    scene_active_player_walk_to(exhibit->scene,exhibit->near);
}

int exhibit_interact(Exhibit *exhibit)
{
    Bool proximity = 0;
    SJson *arg = NULL,*action = NULL;
    arg = sj_object_get_value(exhibit->args,"interact");
    if (!arg)return false;
    sj_get_bool_value(sj_object_get_value(arg,"proximity"),(short int *)&proximity);
    if (proximity)
    {
        if (!player_near_point(scene_get_active_player(exhibit->scene),exhibit->near))
        {
            exhibit_player_walk_to(exhibit);
            return 1;
        }
    }
    // either we don't need to be near it, or we are already here
    action = sj_object_get_value(arg,"action");
}

int exhibit_mouse_check(Exhibit *exhibit)
{
    Vector2D mp = {0,0};
    Shape aS = {0};
    MouseFunction mf;
    SJson *arg = NULL;
    const char *defaultText;
    
    if (exhibit_paused) return 0;
    if (!exhibit)return 0;
    mp = gf2d_mouse_get_position();
    
    gf2d_shape_copy(&aS,exhibit->entity->shape);
    gf2d_shape_move(&aS,exhibit->entity->position);
    gf2d_shape_move(&aS,camera_get_offset());
    if (!gf2d_point_in_rect(mp,gf2d_shape_get_bounds(aS)))return 0;
    
    mf = gf2d_mouse_get_function();
    switch(mf)
    {
        case MF_Pointer:
            break;
        case MF_Walk:
            exhibit_player_walk_to(exhibit);
            return 1;
        case MF_Look:
            arg = sj_object_get_value(exhibit->args,"look");
            if (!arg)return false;
            break;
        case MF_Interact:
            if (exhibit_interact(exhibit))return 1;
            arg = sj_object_get_value(exhibit->args,"interact");
            break;
        case MF_Talk:
            arg = sj_object_get_value(exhibit->args,"talk");
            break;
        case MF_Item:
            arg = sj_object_get_value(exhibit->args,"item");
            if (!arg)return false;
            break;
        case MF_Spell:
            arg = sj_object_get_value(exhibit->args,"spell");
            if (!arg)return false;
            break;

    }
    arg = sj_object_get_value(arg,"DEFAULT");
    defaultText = sj_get_string_value(arg);
    if (defaultText)
    {
        window_alert(exhibit->name, (char *)defaultText,exhibit_unpause ,NULL);
        exhibit_paused = 1;
        return 1;
    }
    return 0;
}

Exhibit *exhibit_load(SJson *json)
{
    Vector4D vector = {0};
    Exhibit *exhibit = NULL;
    const char *string = NULL;
    if (!json)return NULL;

    exhibit = exhibit_new();
    if (!exhibit)return NULL;
    
    gfc_line_cpy(exhibit->name,sj_get_string_value(sj_object_get_value(json,"name")));
    sj_get_bool_value(sj_object_get_value(json,"displayName"),(short int *)&exhibit->displayName);
    
    sj_value_as_vector4d(sj_object_get_value(json,"rect"),&vector);
    exhibit->rect = gf2d_rect(vector.x,vector.y,vector.z,vector.w);

    sj_value_as_vector2d(sj_object_get_value(json,"near"),&exhibit->near);

    exhibit->args = sj_copy(sj_object_get_value(json,"args"));

    string = sj_get_string_value(sj_object_get_value(json,"actor"));
    if (string)gfc_line_cpy(exhibit->actor,string);
    string = sj_get_string_value(sj_object_get_value(json,"action"));
    if (string)gfc_line_cpy(exhibit->action,string);

    return exhibit;
}

void exhibit_draw(Entity *ent)
{
    Vector2D drawPosition;
    if (!ent)return;
    vector2d_add(drawPosition,ent->position,camera_get_offset());

    gf2d_shape_draw(ent->shape,gfc_color(0,1,1,1),drawPosition);
}

Entity *exhibit_entity_spawn(Exhibit *exhibit)
{
    Entity *ent = NULL;
    if (!exhibit)return NULL;
    
    ent = gf2d_entity_new();
    if (!ent)return NULL;
    
    gfc_line_sprintf(ent->name,"exhibit");
    if (strlen(exhibit->actor))
    {
        gf2d_actor_load(&ent->actor,exhibit->actor);
        gf2d_actor_set_action(&ent->actor,exhibit->action);
    }
    ent->position.x = exhibit->rect.x;
    ent->position.y = exhibit->rect.y;
    ent->shape = gf2d_shape_rect(0,0, exhibit->rect.w, exhibit->rect.h);
    ent->draw = exhibit_draw;
    exhibit->entity = ent;
    return ent;
}



/*eol@eof*/
