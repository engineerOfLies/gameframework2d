#include "simple_logger.h"

#include "gfc_vector.h"
#include "gfc_types.h"
#include "gfc_text.h"

#include "gf2d_sprite.h"
#include "gf2d_draw.h"
#include "gf2d_graphics.h"

#include "gf2d_particles.h"

struct ParticleEmitter_S
{
    Uint32          maxParticles;   /**<limit on how many particles can live at once*/
    Uint32          particleCount;  /**<current ammount of living particles*/
    ParticleTypes   particleType;   /**<default particle type*/
    Uint32          ttl;            /**<how long each particle will live*/
    Uint32          ttlVariance;    /**<how much ttl can vary +/-*/
    Vector2D        position;       /**<position offset for all particles generated, the functional position of the emitter*/
    Vector2D        positionVariance;/**<how much starting position can vary*/
    Vector2D        velocity;       /**<default starting velocity*/
    Vector2D        velocityVariance;/**<ho much velocity can vary*/
    Vector2D        acceleration;   /**<default particle acceleration*/
    Vector2D        accelerationVariance;/**<ho much particle acceleration can vary*/
    Color           color;          /**<default color*/
    Color           colorVector;    /**<how color changes over time*/
    Color           colorVariance;  /**<how much defualt color can vary*/
    Shape           shape;          /**<default shape*/
    Uint32          startFrame;     /**<starting frame for a sprite particle*/
    Uint32          endFrame;       /**<end frame for a sprite particle*/
    Uint32          frameVariance;  /**<how much starting frame can vary*/
    TextLine        spriteFile;     /**<for sprite particles use this sprite*/
    Uint32          frameWidth;     /**<if using a sprite, this is the width of the frame*/
    Uint32          frameHeight;    /**<if using a sprite, this is the height of the frame*/
    Uint32          framesPerLine;  /**<if using a sprite, this is how many frames are in one line of the sprite sheet*/
    float           framerate;      /**<how fast frames change for sprites*/
    Particle       *particleList;   /**<the list of particles managed by this emitter*/
    SDL_BlendMode   mode;           /**<the mode to render the particles with*/
};

void gf2d_particle_update(Particle *p,Uint32 now);
void gf2d_particle_draw(Particle *p);

void gf2d_particle_free(Particle *p)
{
    if (!p)return;
    if (p->sprite != NULL)
    {
        gf2d_sprite_free(p->sprite);
    }
    memset(p,0,sizeof(Particle));
}

void gf2d_particle_emitter_free(ParticleEmitter *pe)
{
    int i;
    if (!pe)return;
    if (pe->particleList != NULL)
    {
        for (i = 0; i < pe->maxParticles; i++)
        {
            gf2d_particle_free(&pe->particleList[i]);
        }
    }
    free(pe->particleList);
    free(pe);
}

ParticleEmitter *gf2d_particle_emitter_new_full(
    int         maxParticles,
    Uint32      ttl,
    Uint32      ttlVariance,
    ParticleTypes particleType,
    Vector2D    position,
    Vector2D    positionVariance,
    Vector2D    velocity,
    Vector2D    velocityVariance,
    Vector2D    acceleration,
    Vector2D    accelerationVariance,
    Color       color,
    Color       colorVector,
    Color       colorVariance,
    Shape      *shape,
    Uint32      startFrame,
    Uint32      endFrame,
    Uint32      frameVariance,
    TextLine    spriteFile,
    Uint32      frameWidth,
    Uint32      frameHeight,
    Uint32      framesPerLine,
    float       framerate,
    SDL_BlendMode mode)
{
    ParticleEmitter *pe;
    pe = gf2d_particle_emitter_new(maxParticles);
    if (!pe)return NULL;
    pe->ttl = SDL_GetTicks() + ttl;
    pe->ttlVariance = ttlVariance;
    pe->particleType = particleType;
    vector2d_copy(pe->position,position);
    vector2d_copy(pe->positionVariance,positionVariance);
    vector2d_copy(pe->velocity,velocity);
    vector2d_copy(pe->velocityVariance,velocityVariance);
    vector2d_copy(pe->acceleration,acceleration);
    vector2d_copy(pe->accelerationVariance,accelerationVariance);
    gfc_color_copy(pe->color,color);
    gfc_color_copy(pe->colorVector,colorVector);
    gfc_color_copy(pe->colorVariance,colorVariance);
    pe->startFrame = startFrame;
    pe->endFrame = endFrame;
    frameVariance = frameVariance;
    gfc_line_cpy(pe->spriteFile,spriteFile);
    pe->frameWidth = frameWidth;
    pe->frameHeight = frameHeight;
    pe->framesPerLine = framesPerLine;
    pe->framerate = framerate;
    pe->mode = mode;
    if (shape)
    {
        gf2d_shape_copy(&pe->shape,*shape);
    }
    return pe;
}


ParticleEmitter *gf2d_particle_emitter_new(int maxParticles)
{
    ParticleEmitter *pe;
    if (!maxParticles)
    {
        slog("cannot make a particle emitter for zero particles!");
        return NULL;
    }
    pe = (ParticleEmitter*)malloc(sizeof(ParticleEmitter));
    if (!pe)
    {
        slog("failed to allocate memory for particle emitter");
        return NULL;
    }
    memset(pe,0,sizeof(ParticleEmitter));
    pe->particleList = (Particle *)malloc(sizeof(Particle)*maxParticles);
    if (pe->particleList == NULL)
    {
        slog("failed to allocate memory for particle list");
        gf2d_particle_emitter_free(pe);
        return NULL;
    }
    memset(pe->particleList,0,sizeof(Particle)*maxParticles);
    pe->maxParticles = maxParticles;
    return pe;
}

void gf2d_particle_emitter_update(ParticleEmitter *pe)
{
    int i;
    Uint32 now;
    if (!pe)return;
    now = SDL_GetTicks();
    for (i = 0;i < pe->maxParticles;i++)
    {
        gf2d_particle_update(&pe->particleList[i],now);
    }
}

void gf2d_particle_emitter_draw(ParticleEmitter *pe)
{
    int i;
    if (!pe)return;
    for (i = 0;i < pe->maxParticles;i++)
    {
        gf2d_particle_draw(&pe->particleList[i]);
    }
}

void gf2d_particle_new_default(
        ParticleEmitter *pe,
        Uint32   count)
{
    Vector2D position;
    Vector2D velocity;
    Vector2D acceleration;
    Color color = {0}, colorVariance = {0};
    int i;
    if (!pe)
    {
        slog("no particle emmitter specified");
        return;
    }
    for (i = 0;i < count;i++)
    {
        colorVariance.r = pe->colorVariance.r * gfc_crandom();
        colorVariance.g = pe->colorVariance.g * gfc_crandom();
        colorVariance.b = pe->colorVariance.b * gfc_crandom();
        colorVariance.a = pe->colorVariance.a * gfc_crandom();
        gfc_color_add(&color,pe->color,colorVariance);
        position.x = pe->position.x + (pe->positionVariance.x * gfc_crandom());
        position.y = pe->position.y + (pe->positionVariance.y * gfc_crandom());
        velocity.x = pe->velocity.x + (pe->velocityVariance.x * gfc_crandom());
        velocity.y = pe->velocity.y + (pe->velocityVariance.y * gfc_crandom());
        acceleration.x = pe->acceleration.x + (pe->accelerationVariance.x * gfc_crandom());
        acceleration.y = pe->acceleration.y + (pe->accelerationVariance.y * gfc_crandom());
        gf2d_particle_new_full(
            pe,
            pe->ttl + (gfc_crandom()*pe->ttlVariance) + SDL_GetTicks(),
            (pe->particleType != PT_Sprite)?NULL:gf2d_sprite_load_all(
                pe->spriteFile,
                pe->frameWidth,
                pe->frameHeight,
                pe->framesPerLine,
                false),
            &pe->shape,
            position,
            velocity,
            acceleration,
            color,
            pe->colorVector,
            pe->particleType,
            pe->startFrame + (gfc_random() * pe->frameVariance),
            pe->framerate,
            pe->startFrame,
            pe->endFrame,
            pe->mode);
    }
}

void gf2d_particle_new_full_bulk(
        ParticleEmitter *pe,
        Uint32   count,
        Uint32   ttl,
        Sprite *sprite,
        Shape  *shape,
        Vector2D position,
        Vector2D velocity,
        Vector2D acceleration,
        Color color,
        Color colorVector,
        ParticleTypes type,
        float frame,
        float framerate,
        int startFrame,
        int endFrame,
        SDL_BlendMode mode)
{
    int i;
    for (i = 0; i < count;i++)
    {
        gf2d_particle_new_full(
            pe,
            ttl,
            sprite,
            shape,
            position,
            velocity,
            acceleration,
            color,
            colorVector,
            type,
            frame,
            framerate,
            startFrame,
            endFrame,
            mode);
    }
}


void gf2d_particle_new_full(
        ParticleEmitter *pe,
        Uint32   ttl,
        Sprite *sprite,
        Shape  *shape,
        Vector2D position,
        Vector2D velocity,
        Vector2D acceleration,
        Color color,
        Color colorVector,
        ParticleTypes type,
        float frame,
        float framerate,
        int startFrame,
        int endFrame,
        SDL_BlendMode mode)
{
    Particle *p;
    p = gf2d_particle_new(pe);
    if (!p)return;
    p->ttl = ttl + SDL_GetTicks();
    p->sprite = sprite;
    vector2d_copy(p->position,position);
    vector2d_copy(p->velocity,velocity);
    vector2d_copy(p->acceleration,acceleration);
    gfc_color_copy(p->color,color);
    gfc_color_copy(p->colorVector,colorVector);
    p->type = type;
    p->frame = frame;
    p->framerate = framerate;
    p->startFrame = startFrame;
    p->endFrame = endFrame;
    p->mode = mode;
    if (shape)
    {
        gf2d_shape_copy(&p->shape,*shape);
    }
}


Particle * gf2d_particle_new(ParticleEmitter *pe)
{
    int i;
    if (!pe)
    {
        slog("must specify a particle emitter for new particle");
        return NULL;
    }
    if (pe->maxParticles == pe->particleCount)
    {
        return NULL;
    }
    for (i = 0; i < pe->maxParticles;i++)
    {
        if (!pe->particleList[i].inuse)
        {
            memset(&pe->particleList[i],0,sizeof(Particle));
            pe->particleList[i].inuse = 1;
            return &pe->particleList[i];
        }
    }
    // all out
    return NULL;
}

void gf2d_particle_draw(Particle *p)
{
    Vector4D color;
    Shape shape;
    if ((!p)||(p->inuse == 0))return;
    switch(p->type)
    {
        case PT_Pixel:
            SDL_SetRenderDrawBlendMode(gf2d_graphics_get_renderer(),p->mode);
            color = gfc_color_to_vector4(p->color);
            gf2d_draw_pixel(p->position,gfc_color_to_vector4(p->color));
            SDL_SetRenderDrawBlendMode(gf2d_graphics_get_renderer(),SDL_BLENDMODE_BLEND);
            break;
        case PT_Shape:
            gf2d_shape_copy(&shape,p->shape);
            gf2d_shape_move(&shape,p->position);
            SDL_SetRenderDrawBlendMode(gf2d_graphics_get_renderer(),p->mode);
            color = gfc_color_to_vector4(p->color);
            gf2d_shape_draw(shape,p->color,vector2d(0,0));
            SDL_SetRenderDrawBlendMode(gf2d_graphics_get_renderer(),SDL_BLENDMODE_BLEND);
            break;
        case PT_Sprite:
            SDL_SetTextureBlendMode(p->sprite->texture,p->mode);
            color = gfc_color_to_vector4(p->color);
            gf2d_sprite_draw(
                p->sprite,
                vector2d(p->position.x - (p->sprite->frame_w/2),p->position.y - (p->sprite->frame_h/2)),
                NULL,
                NULL,
                NULL,
                NULL,
                &color,
                (int)p->frame);
            SDL_SetTextureBlendMode(p->sprite->texture,SDL_BLENDMODE_BLEND);
            break;
    }
}

void gf2d_particle_update(Particle *p,Uint32 now)
{
    if ((!p)||(p->inuse == 0))return;
    if (p->ttl <= now)
    {
        gf2d_particle_free(p);
        return;
    }
    vector2d_add(p->position,p->position,p->velocity);
    vector2d_add(p->velocity,p->velocity,p->acceleration);
    gfc_color_add(&p->color,p->color,p->colorVector);
    if (p->type == PT_Sprite)
    {
        p->frame += p->framerate;
        if (p->frame > p->endFrame)p->frame = p->startFrame;
    }
}

/*eol@eof*/
