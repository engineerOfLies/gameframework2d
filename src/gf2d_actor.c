#include "gf2d_actor.h"
#include "simple_logger.h"

#include <stdio.h>

typedef struct
{
    Uint32 maxActionLists;
    ActionList * actionLists;
}ActionManager;

static ActionManager action_list_manager;


void gf2d_action_list_clear_all();

void gf2d_action_list_close()
{
    gf2d_action_list_clear_all();
    if (action_list_manager.actionLists != NULL)
    {
        free(action_list_manager.actionLists);
    }
    action_list_manager.actionLists = NULL;
    action_list_manager.maxActionLists = 0;
    slog("actionList system closed");
}

void gf2d_action_list_init(Uint32 max)
{
    if (!max)
    {
        slog("cannot intialize a actionList manager for Zero actionLists!");
        return;
    }
    action_list_manager.maxActionLists = max;
    action_list_manager.actionLists = (ActionList *)malloc(sizeof(ActionList)*max);
    memset (action_list_manager.actionLists,0,sizeof(ActionList)*max);
    slog("Action system initialized");
    atexit(gf2d_action_list_close);
}

void gf2d_action_list_delete(ActionList *actionList)
{
    if (!actionList)return;
    if (actionList->actions != NULL)
    {
        free(actionList->actions);
    }    
    memset(actionList,0,sizeof(ActionList));//clean up all other data
}

void gf2d_action_list_free(ActionList *actionList)
{
    if (!actionList) return;
    actionList->ref_count--;
}

void gf2d_action_list_clear_all()
{
    int i;
    for (i = 0;i < action_list_manager.maxActionLists;i++)
    {
        gf2d_action_list_delete(&action_list_manager.actionLists[i]);// clean up the data
    }
}

ActionList *gf2d_action_list_new()
{
    int i;
    /*search for an unused sprite address*/
    for (i = 0;i < action_list_manager.maxActionLists;i++)
    {
        if ((action_list_manager.actionLists[i].ref_count == 0)&&(action_list_manager.actionLists[i].actions == NULL))
        {
            action_list_manager.actionLists[i].ref_count = 1;//set ref count
            return &action_list_manager.actionLists[i];//return address of this array element        }
        }
    }
    /*find an unused sprite address and clean up the old data*/
    for (i = 0;i < action_list_manager.maxActionLists;i++)
    {
        if (action_list_manager.actionLists[i].ref_count == 0)
        {
            gf2d_action_list_delete(&action_list_manager.actionLists[i]);// clean up the old data
            action_list_manager.actionLists[i].ref_count = 1;//set ref count
            return &action_list_manager.actionLists[i];//return address of this array element
        }
    }
    slog("error: out of actionList addresses");
    return NULL;
}

ActionList *gf2d_action_list_get_by_filename(char * filename)
{
    int i;
    if (!filename)
    {
        slog("no filename provided");
        return NULL;
    }
    for (i = 0;i < action_list_manager.maxActionLists;i++)
    {
        if (gf2d_line_cmp(action_list_manager.actionLists[i].filename,filename)==0)
        {
            return &action_list_manager.actionLists[i];
        }
    }
    return NULL;// not found
}


int gf2d_action_file_get_count(FILE *file)
{
    char buf[512];
    int count = 0;
    if (!file)return 0;
    rewind(file);
    while(fscanf(file, "%s", buf) != EOF)
    {
        if(strcmp(buf,"action:") == 0)
        {
            count++;
        }
        fgets(buf, sizeof(buf), file);
    }
    return count;
}

void gf2d_action_file_load_actions(FILE *file,ActionList *actionList)
{
    Action *actions;
    char buf[512];
    if (!file)return;
    rewind(file);
    actions = actionList->actions;
    actions--;
    while(fscanf(file, "%s", buf) != EOF)
    {
        if(strcmp(buf,"sprite:") == 0)
        {
            fscanf(file,"%s",(char*)&actionList->sprite);
            continue;
        }
        if(strcmp(buf,"frameWidth:") == 0)
        {
            fscanf(file,"%i",&actionList->frameWidth);
            continue;
        }
        if(strcmp(buf,"frameHeight:") == 0)
        {
            fscanf(file,"%i",&actionList->frameHeight);
            continue;
        }
        if(strcmp(buf,"framesPerLine:") == 0)
        {
            fscanf(file,"%i",&actionList->framesPerLine);
            continue;
        }
        if(strcmp(buf,"scale:") == 0)
        {
            fscanf(file,"%lf,%lf",&actionList->scale.x,&actionList->scale.y);
            continue;
        }
        if(strcmp(buf,"color:") == 0)
        {
            fscanf(file,"%lf,%lf,%lf,%lf",&actionList->color.x,&actionList->color.y,&actionList->color.z,&actionList->color.w);
            continue;
        }
        if(strcmp(buf,"colorSpecial:") == 0)
        {
            fscanf(file,"%lf,%lf,%lf,%lf",&actionList->colorSpecial.x,&actionList->colorSpecial.y,&actionList->colorSpecial.z,&actionList->colorSpecial.w);
            continue;
        }
        if(strcmp(buf,"action:") == 0)
        {
            actions++;
            fscanf(file,"%s",(char*)&actions->name);
            continue;
        }
        if (actions < actionList->actions)
        {
            slog("file formatting error, expect action: tag before rest of data");
            continue;
        }
        if(strcmp(buf,"type:") == 0)
        {
            fscanf(file,"%s",buf);
            if (strcmp(buf,"loop")==0)
            {
                actions->type = AT_LOOP;
                continue;
            }
            if (strcmp(buf,"pass")==0)
            {
                actions->type = AT_PASS;
                continue;
            }
            continue;
        }
        if(strcmp(buf,"startFrame:") == 0)
        {
            fscanf(file,"%i",&actions->startFrame);
            continue;
        }
        if(strcmp(buf,"endFrame:") == 0)
        {
            fscanf(file,"%i",&actions->endFrame);
            continue;
        }
        if(strcmp(buf,"frameRate:") == 0)
        {
            fscanf(file,"%f",&actions->frameRate);
            continue;
        }
        fgets(buf, sizeof(buf), file);
    }
}

ActionList *gf2d_action_list_load(
    char *filename
)
{
    FILE *file;
    ActionList *actionList;
    int count;
    actionList = gf2d_action_list_get_by_filename(filename);
    if (actionList != NULL)
    {
        // found a copy already in memory
        actionList->ref_count++;
        return actionList;
    }
    
    actionList = gf2d_action_list_new();
    if (!actionList)
    {
        return NULL;
    }
    file = fopen(filename,"r");
    if (!file)
    {
        gf2d_action_list_delete(actionList);
        slog("failed to open action file: %s",filename);
        return NULL;
    }
    gf2d_line_cpy(actionList->filename,filename);
    count = gf2d_action_file_get_count(file);
    if (!count)
    {
        gf2d_action_list_delete(actionList);
        fclose(file);
        slog("No actions found in file: %s",filename);
        return NULL;
    }
    actionList->actions = (Action*)malloc(sizeof(Action)*count);
    memset(actionList->actions,0,sizeof(Action)*count);
    actionList->numActions = count;
    gf2d_action_file_load_actions(file,actionList);
    
    fclose(file);
    return actionList;
}

Action *gf2d_action_list_get_action(ActionList *al, char *name)
{
    int i;
    if (!al)
    {
        slog("no action list provided");
        return NULL;
    }
    if (!name)
    {
        slog("no filename provided");
        return NULL;
    }
    for (i = 0; i < al->numActions;i++)
    {
        if (gf2d_line_cmp(al->actions[i].name,name) == 0)
        {
            return &al->actions[i];
        }
    }
    return NULL;// not found
}

float gf2d_action_set(ActionList *al,char *name)
{
    Action *action;
    action = gf2d_action_list_get_action(al, name);
    if (!action)
    {
        slog("no action found by name %s",name);
        return ART_ERROR;
    }
    return action->startFrame;
}

ActionReturnType gf2d_action_list_get_next_frame(
    ActionList *al,
    float * frame,
    char *name)
{
    Action *action;
    if (!frame)
    {
        slog("missing frame data");
        return ART_ERROR;
    }
    action = gf2d_action_list_get_action(al, name);
    if (!action)
    {
        slog("no action found by name %s",name);
        return ART_ERROR;
    }
    *frame += action->frameRate;
    if (*frame >= action->endFrame)
    {
        switch (action->type)
        {
            case AT_NONE:
                break;
            case AT_LOOP:
                *frame = action->startFrame;
                break;
            case AT_PASS:
                *frame = action->endFrame;
                return ART_END;
        }
    }
    return ART_NORMAL;
}

void gf2d_actor_free(Actor *actor)
{
    if (!actor)return;
    gf2d_sprite_free(actor->sprite);
    gf2d_action_list_free(actor->al);
    memset(actor,0,sizeof(Actor));
}

void gf2d_actor_load(Actor *actor,char *file)
{
    if (!actor)
    {
        slog("no actor specified to load into");
        return;
    }
    actor->al = gf2d_action_list_load(file);
    if (!actor->al)
    {
        return;// should have logged the error already
    }
    vector4d_copy(actor->color,actor->al->color);
    actor->sprite = gf2d_sprite_load_all(
        actor->al->sprite,
        actor->al->frameWidth,
        actor->al->frameHeight,
        actor->al->framesPerLine);
    gf2d_actor_set_action(actor,actor->al->actions[0].name);
}

void gf2d_actor_set_action(Actor *actor,char *action)
{
    if (!actor)return;
    actor->frame = gf2d_action_set(actor->al,action);
    gf2d_line_cpy(actor->action,action);
}

void gf2d_actor_next_frame(Actor *actor)
{
    if (!actor)return;
    actor->at = gf2d_action_list_get_next_frame(actor->al,&actor->frame,actor->action);
}

void gf2d_actor_draw(
    Actor *actor,
    Vector2D position,
    Vector2D * scale,
    Vector2D * scaleCenter,
    Vector3D * rotation,
    Vector2D * flip
)
{
    Vector2D drawScale;
    if (!actor)return;
    vector2d_copy(drawScale,actor->al->scale);
    if (scale)
    {
        drawScale.x *= scale->x;
        drawScale.y *= scale->y;
    }
    gf2d_sprite_draw(
    actor->sprite,
    position,
    &drawScale,
    scaleCenter,
    rotation,
    flip,
    &actor->color,
    (int)actor->frame);
}

/*eol@eof*/
