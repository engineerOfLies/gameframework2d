#include "gf2d_elements.h"
#include "editor.h"
#include "level.h"
#include "gf2d_text.h"
#include "simple_logger.h"
#include "simple_json.h"
#include "windows_common.h"


typedef struct EditorData_S
{
    TextLine filename;
    LevelInfo *level;       /**<working level*/
}EditorData;

EditorData editorData = {0};
static Window *_editor = NULL;

int editor_draw(Window *win)
{
    /*draw rects around the tile the mouse is over*/
    return 0; 
}

void editor_new_map(void *data)
{
    level_clear();//cleanup
    editorData.level = level_info_create(
        "images/backgrounds/bg_flat.png",
        "music/old_city_theme.ogg",
        "images/tiles/basetile.png",
        vector2d(32,32),
        vector2d(38,24));
    level_init(editorData.level);
}

int editor_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    if (!win)return 0;
    if (!updateList)return 0;
    count = gf2d_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = (Element *)gf2d_list_get_nth(updateList,i);
        if (!e)continue;
        switch(e->index)
        {
            case 110:
                slog("load");
                window_text_entry("enter file to load", editorData.filename, GF2DTEXTLEN, NULL, NULL);
                break;
            case 130:
                slog("new file");
                window_yes_no("Create New Map, previous data lost?",editor_new_map,NULL,NULL,NULL);
                break;
        }
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
    _editor->draw = editor_draw;
}

/*eol@eof*/
