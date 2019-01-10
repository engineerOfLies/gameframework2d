#include "editor.h"
#include "level.h"
#include "simple_logger.h"

static Window *_editor = NULL;

int editor_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    if (!win)return 0;
    if (!updateList)return 0;
    count = gf2d_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gf2d_list_get_nth(updateList,i);
        if (!e)continue;
/*        switch(e->index)
        {
            case 51:
                slog("ok");
                break;
            case 52:
                slog("cancel");
                break;
        }*/
    }
    return 0;
}

void editor_launch()
{
    if (_editor != NULL)return;
    _editor = gf2d_window_load("config/editor_main.json");
    if (!_editor)
    {
        slog("failed to load editor window");
        return;
    }
    _editor->update = editor_update;
}

/*eol@eof*/
