#include "particle_effects.h"

typedef struct
{
    Sprite *blood;
    Sprite *explosion[3];
}PE_Common_Assets;

static PE_Common_Assets commonAssets = {0};

void pe_free_assets()
{
    int i;
    gf2d_sprite_free(commonAssets.blood);
    for (i = 0; i < 3;i++)
    {
        gf2d_sprite_free(commonAssets.explosion[i]);
    }
}

void pe_load_assets()
{
    commonAssets.blood = gf2d_sprite_load_all(
        "images/blood2.png",
        32,
        32,
        16);
    commonAssets.explosion[0] = gf2d_sprite_load_all(
        "images/explosion1.png",
        64,
        64,
        16);
    commonAssets.explosion[1] = gf2d_sprite_load_all(
        "images/explosion2.png",
        64,
        64,
        16);
    commonAssets.explosion[2] = gf2d_sprite_load_all(
        "images/explosion3.png",
        64,
        64,
        16);
    atexit(pe_free_assets);
}

void pe_explosion(
    ParticleEmitter *pe,
    Vector2D position,
    Color color,
    int count)
{
    int i;
    for (i = 0; i < count;i++)
    {
        gf2d_particle_new_full(
            pe,
            100,
            commonAssets.explosion[0],
            NULL,
            vector2d(
               position.x + gf2d_crandom()*2,
               position.y + gf2d_crandom()*2),
            vector2d(
                gf2d_crandom()*2,
                gf2d_crandom()*2),
            vector2d(0,0),
            color,
            gf2d_color(0,0,0,-0.01),
            PT_Sprite,
            0,
            0.01,
            0,
            25,
            SDL_BLENDMODE_ADD);
    }
}


void pe_thrust(
    ParticleEmitter *pe,
    Vector2D position,
    Vector2D positionVariance,
    Vector2D velocity,
    Vector2D velocityVariance,
    Vector2D acceleration,
    Vector2D accelerationVariance,
    Color color,
    int count)
{
    int i;
    if (!pe)return;
    for (i = 0; i < count;i++)
    {
        gf2d_particle_new_full(
            pe,
            100,
            NULL,
            NULL,
            vector2d(
               position.x + gf2d_crandom()*positionVariance.x,
               position.y + gf2d_crandom()*positionVariance.y),
            vector2d(
                velocity.x + gf2d_crandom()*velocityVariance.x,
                velocity.y + gf2d_crandom()*velocityVariance.y),
            vector2d(
                acceleration.x + gf2d_crandom()*accelerationVariance.x,
                acceleration.y + gf2d_crandom()*accelerationVariance.y),
            color,
            gf2d_color(0,0,0,-0.01),
            PT_Pixel,
            0,
            0,
            0,
            0,
            SDL_BLENDMODE_ADD);
    }
}

void pe_blood_spray(ParticleEmitter *pe, Vector2D position,Vector2D direction,Color color,int amount)
{
    int i;
    for (i = 0; i < amount;i++)
    {
        gf2d_particle_new_full(
            pe,
            100,
            commonAssets.blood,
            NULL,
            vector2d(
               position.x + gf2d_crandom()*5,
               position.y + gf2d_crandom()*5),
            vector2d(
                direction.x + gf2d_crandom()*2,
                direction.y + gf2d_crandom()*1),
            vector2d(0,0),
            color,
            gf2d_color(0,0,0,-0.01),
            PT_Sprite,
            0,
            0.01,
            0,
            6,
            SDL_BLENDMODE_ADD);
    }
}


/*eol@eof*/
