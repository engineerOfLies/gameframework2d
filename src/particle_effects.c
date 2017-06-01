#include "particle_effects.h"


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
    pe_thrust(
        pe,
        position,
        vector2d(5,5),
        direction,
        vector2d(10,10),
        vector2d(0,0.01),
        vector2d(0,0),
        color,
        amount);
}


/*eol@eof*/
