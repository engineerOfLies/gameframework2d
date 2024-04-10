#include "simple_logger.h"

#include "gf2d_draw.h"

#include "particles.h"

typedef struct
{
    Particle *particleList;
    Uint32    particleMax;
}ParticleManager;

static ParticleManager particle_manager = {0};

void particle_manager_close()
{
    if (particle_manager.particleList)
    {
        free(particle_manager.particleList);
    }
    memset(&particle_manager,0,sizeof(ParticleManager));
}

void particle_manager_init(Uint32 particleMax)
{
    if (!particleMax)
    {
        slog("cannot allocate zero particles for the particle manager");
        return;
    }
    
    particle_manager.particleList = gfc_allocate_array(sizeof(Particle),particleMax);
    if (!particle_manager.particleList)
    {
        slog("failed to allocate particles for the manager");
        return;
    }
    particle_manager.particleMax = particleMax;
    
    atexit(particle_manager_close);
}

void particle_manager_clear()
{
    int i;
    if (!particle_manager.particleList)return;
    for (i = 0;i < particle_manager.particleMax;i++)
    {
        if (particle_manager.particleList[i].sprite)
        {
            gf2d_sprite_free(particle_manager.particleList[i].sprite);
        }
    }
    memset(particle_manager.particleList,0,sizeof(Particle)*particle_manager.particleMax);
}

Particle *particle_new()
{
    int i;
    for (i = 0;i < particle_manager.particleMax;i++)
    {
        if (particle_manager.particleList[i].ttl == 0)
        {
            if (particle_manager.particleList[i].sprite)
            {
                gf2d_sprite_free(particle_manager.particleList[i].sprite);
            }
            memset(&particle_manager.particleList[i],0,sizeof(Particle));
            particle_manager.particleList[i].ttl = 1;
            return &particle_manager.particleList[i];
        }
    }
    return NULL;//all out of slots for new particles;
}

void particle_update(Particle *p)
{
    if (!p)return;
    p->ttl--;
    if (!p->ttl)return;
    vector2d_add(p->position,p->position,p->velocity);
    vector2d_add(p->velocity,p->velocity,p->acceleration);
    
    gfc_color_add(&p->color,p->color,p->colorVelocity);
    gfc_color_add(&p->colorVelocity,p->colorVelocity,p->colorAcceleration);
    
    if (p->pType == PT_Sprite)
    {
        p->frame += p->frameRate;
        
        if (p->frame > p->frameEnd)
        {
            if (p->loops)
            {
                p->frame = p->frameStart;
            }
            else
            {
                p->frame = p->frameEnd;
                p->frameRate = 0;
            }
        }
    }
}

void particle_draw(Particle *p)
{
    if (!p)return;
    switch (p->pType)
    {
        case PT_MAX:
        case PT_Point:
            gf2d_draw_pixel(p->position,p->color);
            break;
        case PT_Shape:
            gf2d_draw_shape(p->shape,p->color,p->position);
            break;
        case PT_Sprite:
            gf2d_sprite_draw(
                p->sprite,
                p->position,
                NULL,
                NULL,
                NULL,
                NULL,
                &p->color,
                (Uint32)p->frame);
            break;
    }
}


void particle_manager_draw()
{
    int i;
    for (i = 0;i < particle_manager.particleMax;i++)
    {
        if (particle_manager.particleList[i].ttl > 0)
        {
            particle_update(&particle_manager.particleList[i]);
            particle_draw(&particle_manager.particleList[i]);
        }
    }
}

void particle_spray(Uint32 count, Uint32 ttl, Vector2D position, Color color, Color variation, Vector2D dir, float speed,float speedVariation, float spread,Vector2D acceleration)
{
    Particle *p;
    int i;
    Vector2D d;
    vector2d_normalize (&dir);
    for (i = 0; i < count;i++)
    {
        p = particle_new();
        if (!p)return;
        p->ttl = ttl;
        gfc_color_copy(p->color,color);
        p->color.r += gfc_crandom()*variation.r;
        p->color.g += gfc_crandom()*variation.g;
        p->color.b += gfc_crandom()*variation.b;
        p->color.a += gfc_crandom()*variation.a;
        vector2d_copy(p->position,position);
        
        d = vector2d_rotate(dir, gfc_crandom()*spread);
        vector2d_scale(p->velocity,d,speed * gfc_random()*speedVariation);
        vector2d_copy(p->acceleration,acceleration);
        
        p->pType = PT_Sprite;
        p->sprite = gf2d_sprite_load_all(
            "images/effects/cloud.png",
            16,
            16,
            1,
            0);
    }
}


/*eol@eof*/
