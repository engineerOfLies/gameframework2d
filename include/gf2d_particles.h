#ifndef __GF2D_PARTICLES_H__
#define __GF2D_PARTICLES_H__

#include <SDL.h>

#include "gfc_vector.h"
#include "gfc_color.h"
#include "gfc_text.h"

#include "gf2d_sprite.h"
#include "gf2d_shape.h"

typedef enum
{
    PT_Pixel,
    PT_Sprite,
    PT_Shape
}ParticleTypes;

typedef struct Particle_S
{
    Uint8    inuse;          /**<do not touch this*/
    Uint32   ttl;             /**<time to live in update frames*/
    Sprite *sprite;         /**<if this particle is using a sprite, use this one*/
    Shape    shape;         /**<shape*/
    Vector2D position;      /**<position of the particle*/
    Vector2D velocity;      /**<movement velocity of the particle*/
    Vector2D acceleration;  /**<movement acceleration of the particle*/
    Color color;            /**<rendering color of the particle*/
    Color colorVector;      /**<rate of change for color over time*/
    ParticleTypes type;     /**< which type, see enumeration*/
    float frame,framerate;  /**<current frame for a sprite, rate of change for the sprite*/
    int startFrame,endFrame;/**<frames will loop between these frames*/
    int drawSolid;          /**<if a shape should be drawn solid only matters for some shape types*/
    SDL_BlendMode mode;     /**<the render mode for drawing ADD, BLEND*/
}Particle;

typedef struct ParticleEmitter_S ParticleEmitter;

/**
 * @brief create a new particle emitter
 * must be freed with gf2d_particle_emitter_free();
 * @param maxParticles the maximum number of particles that can be alive with this emitter at once
 * @return NULL on error or a pointer to a particle emitter
 */
ParticleEmitter *gf2d_particle_emitter_new(int maxParticles);

/**
 * @brief create a new particle emitter specifying all default values for the emitter
 * @return NULL on error or a pointer to a particle emitter
 */
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
    Shape       *shape,
    Uint32      startFrame,
    Uint32      endFrame,
    Uint32      frameVariance,
    TextLine    spriteFile,
    Uint32      frameWidth,
    Uint32      frameHeigh,
    Uint32      framesPerLine,
    float       framerate,
    SDL_BlendMode mode);

/**
 * @brief free an allocated particle emitter
 * @param pe the particle emitter to free
 */
void gf2d_particle_emitter_free(ParticleEmitter *pe);

/**
 * @brief update all particles in the provided emitter
 * @param pe the particle emitter to update
 */
void gf2d_particle_emitter_update(ParticleEmitter *pe);

/**
 * @brief draw all particles to current rendering context
 * @param pe the particle emitter to draw
 */
void gf2d_particle_emitter_draw(ParticleEmitter *pe);

/**
 * @brief create a new particle for the provided emitted and get a pointer to it
 * @param pe the particle emitter to create a particle for
 * @return NULL on error or if the emitter is full or the particle to specify.
 *  TTL must be set > 0 or the particle will be cleaned up on next update
 */
Particle * gf2d_particle_new(ParticleEmitter *pe);

/**
 * @brief create a new particle for the provided emitter
 */
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
        SDL_BlendMode mode);

/**
 * @brief create a bunch of new particles for the provided emitter
 */
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
        SDL_BlendMode mode);

/**
 * @brief create a set of new particles for the emitter using its default parameters
 * @param pe the particle emitter to create particles for
 * @param count how many to create
 */
void gf2d_particle_new_default(
        ParticleEmitter *pe,
        Uint32   count);

#endif
