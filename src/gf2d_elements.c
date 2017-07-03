#include <stdlib.h>
#include <string.h>
#include "gf2d_elements.h"
#include "simple_logger.h"

Element *gf2d_element_new()
{
    Element *e;
    e = (Element *)malloc(sizeof(Element));
    if (!e)
    {
        slog("failed to allocate a new window element");
        return NULL;
    }
    memset(e,0,sizeof(Element));
    return e;
}

Element *gf2d_element_new_full(
    int      index,
    TextLine name,
    Rect bounds,
    Color color,
    int state
)
{
    Element *e;
    e = gf2d_element_new();
    if (!e)return NULL;
    gf2d_line_cpy(e->name,name);
    e->index = index;
    e->color = color;
    e->state = state;
    e->bounds = bounds;
    return e;
}

void gf2d_element_free(Element *e)
{
    if (!e)return;
    if (e->free_data)
    {
        e->free_data(e);
    }
    free(e);
}

void gf2d_element_draw(Element *e, Vector2D offset)
{
    if (!e)
    {
        return;
    }
    if (e->draw)e->draw(e,offset);
}

List * gf2d_element_update(Element *e, Vector2D offset)
{
    if (!e)
    {
        return NULL;
    }
    if (e->draw)return e->update(e,offset);
    return NULL;
}
/*eol@eof*/
