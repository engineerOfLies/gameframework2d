#include "simple_logger.h"
#include "gfc_text.h"
#include "gfc_color.h"

#include "gf2d_graphics.h"

#include "gf2d_font.h"

typedef struct
{
    Font *font_list;
    Font *font_tags[FT_MAX];
    Uint32 font_max;
}FontManager;

static FontManager font_manager = {0};

void gf2d_fonts_load(char *filename);

void gf2d_font_close()
{
    int i;
    for (i = 0;i < font_manager.font_max;i++)
    {
        if (font_manager.font_list[i].font != NULL)
        {
            TTF_CloseFont(font_manager.font_list[i].font);
        }
    }
    TTF_Quit();
    slog("text system closed");
}

void gf2d_font_init(char *configFile)
{
    if (TTF_Init() == -1)
    {
        slog("TTF_Init: %s\n", TTF_GetError());
        return;
    }
    gf2d_fonts_load(configFile);
    slog("text system initialized");
    atexit(gf2d_font_close);
}

int gf2d_fonts_get_count(FILE *file)
{
    char buf[512];
    int count = 0;
    if (!file)return 0;
    rewind(file);
    while(fscanf(file, "%s", buf) != EOF)
    {
        if(strcmp(buf,"font:") == 0)
        {
            count++;
        }
        fgets(buf, sizeof(buf), file);
    }
    return count;
}

void gf2d_fonts_parse(FILE *file)
{
    Font *font;
    char buf[512];
    if (!file)return;
    rewind(file);
    font = font_manager.font_list;
    font--;
    while(fscanf(file, "%s", buf) != EOF)
    {
        if(strcmp(buf,"font:") == 0)
        {
            font++;
            fscanf(file,"%s",(char*)&font->filename);
            continue;
        }
        if(strcmp(buf,"size:") == 0)
        {
            fscanf(file,"%ui",&font->pointSize);
            continue;
        }
        if(strcmp(buf,"tag:") == 0)
        {
            fscanf(file,"%s",buf);
            if (strcmp(buf,"normal")==0)
            {
                font_manager.font_tags[FT_Normal] = font;
            }
            else if (strcmp(buf,"small")==0)
            {
                font_manager.font_tags[FT_Small] = font;
            }
            else if (strcmp(buf,"H1")==0)
            {
                font_manager.font_tags[FT_H1] = font;
            }
            else if (strcmp(buf,"H2")==0)
            {
                font_manager.font_tags[FT_H2] = font;
            }
            else if (strcmp(buf,"H3")==0)
            {
                font_manager.font_tags[FT_H3] = font;
            }
            else if (strcmp(buf,"H4")==0)
            {
                font_manager.font_tags[FT_H4] = font;
            }
            else if (strcmp(buf,"H5")==0)
            {
                font_manager.font_tags[FT_H5] = font;
            }
            else if (strcmp(buf,"H6")==0)
            {
                font_manager.font_tags[FT_H6] = font;
            }
            continue;
        }
        fgets(buf, sizeof(buf), file);
    }
}

void gf2d_fonts_load(char *filename)
{
    FILE *file;
    int count;
    int i;
    file = fopen(filename,"r");
    if (!file)
    {
        slog("failed to open font config file %s",filename);
        return;
    }
    count = gf2d_fonts_get_count(file);
    if (!count)
    {
        slog("font config file %s contained no font information",filename);
        fclose(file);
        return;
    }
    font_manager.font_list = (Font*)malloc(sizeof(Font)*count);
    if (!font_manager.font_list)
    {
        slog("failed to allocate memory for %i fonts",count);
        fclose(file);
        return;
    }
    memset(font_manager.font_list,0,sizeof(Font)*count);
    for (i = 0 ; i < FT_MAX; i++)
    {
        font_manager.font_tags[i] = font_manager.font_list;
    }
    gf2d_fonts_parse(file);
    for (i = 0; i < count; i++)
    {
        font_manager.font_list[i].font = TTF_OpenFont(font_manager.font_list[i].filename, font_manager.font_list[i].pointSize);
        if (!font_manager.font_list[i].font)
        {
            slog("failed to load font: %s\n", TTF_GetError());
        }
    }
    font_manager.font_max = count;
    slog("font library loaded with %i fonts",count);
    fclose(file);
}

Font *gf2d_font_get_by_filename(char *filename)
{
    int i;
    if (!filename)return NULL;
    for (i = 0; i < font_manager.font_max;i++)
    {
        if (gfc_line_cmp(font_manager.font_list[i].filename,filename) == 0)
        {
            return &font_manager.font_list[i];
        }
    }
    return NULL;// not found
}

Font *gf2d_font_get_by_tag(FontTypes tag)
{
    if ((tag < 0) ||(tag >= FT_MAX))
    {
        slog("bad font tag: %i",tag);
        return NULL;
    }
    return font_manager.font_tags[tag];
}

void gf2d_font_draw_line_named(char *text,char *filename,Color color, Vector2D position)
{
    gf2d_font_draw_line(text,gf2d_font_get_by_filename(filename),color, position);
}

void gf2d_font_draw_line_tag(char *text,FontTypes tag,Color color, Vector2D position)
{
    gf2d_font_draw_line(text,gf2d_font_get_by_tag(tag),color, position);
}

void gf2d_font_draw_line(char *text,Font *font,Color color, Vector2D position)
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect dst = {0};
    if (!text)
    {
        slog("cannot draw text, none provided");
        return;
    }
    if (!font)
    {
        slog("cannot draw text, no font provided");
        return;
    }
    
    surface = TTF_RenderUTF8_Blended(font->font, text, gfc_color_to_sdl(color));
    if (!surface)
    {
        slog("failed to render text");
        return;
    }
    surface = gf2d_graphics_screen_convert(&surface);
    if (!surface)
    {
        slog("failed to convert text surface to screen format");
        return;
    }
    
    texture = SDL_CreateTextureFromSurface(gf2d_graphics_get_renderer(),surface);
    if (!texture)
    {
        slog("failed to convert text surface to texture");
        SDL_FreeSurface(surface);
        return;
    }
    SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);        
    SDL_UpdateTexture(texture,
                    NULL,
                    surface->pixels,
                    surface->pitch);
    vector2d_copy(dst,position);
    dst.w = surface->w;
    dst.h = surface->h;
    SDL_RenderCopy(
        gf2d_graphics_get_renderer(),
        texture,
        NULL,
        &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

Vector2D gf2d_font_get_bounds_tag(char *text,FontTypes tag)
{
    return gf2d_font_get_bounds(text,gf2d_font_get_by_tag(tag));
}

Vector2D gf2d_font_get_bounds(char *text,Font *font)
{
    int x = -1,y = -1;
    if (!text)
    {
        slog("cannot size text, none provided");
        return vector2d(-1,-1);
    }
    if (!font)
    {
        slog("cannot size text, no font provided");
        return vector2d(-1,-1);
    }
    
    TTF_SizeUTF8(font->font, text, &x,&y);
    return vector2d(x,y);
}

/*eol@eof*/
