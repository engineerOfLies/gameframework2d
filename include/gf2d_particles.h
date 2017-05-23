#ifndef __GF2D_PARTICLES_H__
#define __GF2D_PARTICLES_H__

#include <SDL.h>

#include "gf2d_vector.h"
#include "gf2d_color.h"

typedef enum
{
    PT_Pixel,
    PT_Circle,
    PT_Rect,
    PT_Sprite
}ParticleTypes;

typedef struct Particle_S
{
    Uint32 ttl;             /**<time to live in update frames*/
    Vector2D position;      /**<position of the particle*/
    Vector2D velocity;      /**<movement velocity of the particle*/
    Vector2D acceleration;  /**<movement acceleration of the particle*/
    Color color;         /**<rendering color of the particle*/
    Color colorVector;   /**<*/
    ParticleTypes type;     /**<*/
    float frame,framerate;  /**<*/
    int startFrame,endFrame;/**<*/
}Particle;

typedef struct ParticleEmitter_S ParticleEmitter;


#endif
