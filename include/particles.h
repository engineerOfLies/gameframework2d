#ifndef __PARTICLES_H__
#define __PARTICLES_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gfc_color.h"
#include "gfc_shape.h"

#include "gf2d_sprite.h"

typedef enum
{
    PT_Point,
    PT_Shape,
    PT_Sprite,
    PT_MAX
}ParticleType;

typedef struct
{
    Uint32  ttl;            /**<time to live in frames*/
    ParticleType pType;     /**<how this particle should be drawn*/
    
    Vector2D position;      /**<*/
    Vector2D velocity;      /**<*/
    Vector2D acceleration;  /**<*/
    
    Color   color;
    Color   colorVelocity;
    Color   colorAcceleration;
    
    Shape shape;
    Sprite *sprite;
    float frame;
    float frameRate;
    int   frameStart;
    int   frameEnd;     /**<last frame of the animation*/
    int   loops;        /**<if true, loop, else stop on last frame*/
}Particle;


/**
 * @brief sets up the manager, pre-allocates all particles
 * @param particleMax the maximum number of particles that can exist at once
 */
void particle_manager_init(Uint32 particleMax);

/**
 * @brief clear all active particles
 */
void particle_manager_clear();

/**
 * @brief grab an empty particle;
 */
Particle *particle_new();

/**
 * @brief spawn a spray of particles from position in direction
 * @param count how many particles to spawn
 * @param ttl how many frames each particle should live
 * @param position where to spawn from
 * @param color what base color to make the particles
 * @param variation how much that color should vary
 * @param dir direction of spray
 * @param speed how fast the particles should move
 * @param speedVariation how much the speed of particles should vary
 * @param spread how much the direction should vary (in radians)
 * @param acceleration how much, if at all, the particles should accelerate
 */
void particle_spray(
    Uint32 count,
    Uint32 ttl,
    Vector2D position,
    Color color,
    Color variation,
    Vector2D dir,
    float speed,
    float speedVariation,
    float spread,
    Vector2D acceleration);

/**
 * @brief update and draw all particles for the new frame
 */
void particle_manager_draw();


#endif
