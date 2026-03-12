#include "simple_logger.h"

#include "camera.h"
#include "parallax.h"

Parallax *parallax_new()
{
    Parallax *parallax;
    parallax = gfc_allocate_array(sizeof(Parallax),1);
    if (!parallax)return NULL;
    parallax->layers = gfc_list_new();
    if (!parallax->layers)
    {
        free(parallax);
        return NULL;
    }
    return parallax;
}

Parallax *parallax_load(const char *filename)
{
    Uint8 baseLayer = 0;
    int i,c;
    Sprite *layer;
    const char *str;
    SJson *json,*list, *item;
    Parallax *parallax;
    if (!filename)return NULL;
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to load parallax %s",filename);
        return NULL;
    }
    parallax = parallax_new();
    if (!parallax)
    {
        sj_free(json);
        return NULL;
    }
    list = sj_object_get_value(json,"parallax");
    c = sj_array_count(list);
    slog("the json had %i layers",c);
    for (i = 0; i < c; i++)
    {
        item = sj_array_nth(list,i);
        if (!item)continue;
        str = sj_get_string_value(item);
        if (!str)continue;
        layer = gf2d_sprite_load_image(str);
        if (!layer)continue;
        gfc_list_append(parallax->layers,layer);
    }
    sj_object_get_uint8(json,"backgroundLayer",&baseLayer);
    parallax->baseLayer = gfc_list_nth(parallax->layers,baseLayer);
    if (parallax->baseLayer)
    {
        slog("selecting layer %i %s as base layer",baseLayer,parallax->baseLayer->filepath);
        parallax->baseSize.x = parallax->baseLayer->frame_w;
        parallax->baseSize.y = parallax->baseLayer->frame_h;
    }
    camera_set_bounds(gfc_rect(0,0,parallax->baseSize.x,parallax->baseSize.y));
    slog("loaded a parallax with %i layers",gfc_list_count(parallax->layers));
    return parallax;
}


void parallax_free(Parallax *parallax)
{
    if (!parallax)return;
    if (parallax->layers)
    {
        gfc_list_foreach(parallax->layers,(gfc_work_func*)gf2d_sprite_free);
    }
    free(parallax);
}

void parallax_draw(Parallax *parallax)
{
    int i,c;
    Sprite *layer;
    GFC_Vector2D screenSize, cameraOffset,drawOffset, ratio;
    if (!parallax)return;
    screenSize = camera_get_dimension();
    c = gfc_list_count(parallax->layers);
    cameraOffset = camera_get_offset();
    for (i = 0; i < c; i++)
    {
        layer = gfc_list_nth(parallax->layers,i);
        if (!layer)continue;
        ratio.x = (layer->frame_w - screenSize.x)/(parallax->baseSize.x - screenSize.x);
        ratio.y = (layer->frame_h - screenSize.y)/(parallax->baseSize.y - screenSize.y);
        gfc_vector2d_scale_by(drawOffset,cameraOffset,ratio);
        gf2d_sprite_draw_image(layer,drawOffset);
    }
}

/*eol@eof*/
