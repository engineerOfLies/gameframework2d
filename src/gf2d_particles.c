#include "gf2d_particles.h"
#include "gf2d_sprite.h"
#include "gf2d_vector.h"
#include "gf2d_types.h"

struct ParticleEmitter_S
{
    Uint32      maxParticles;   /**<limit on how many particles can live at once*/
    Uint32      particleCount;  /**<current ammount of living particles*/
    Vector2D    position;       /**<position offset for all particles generated, the functional position of the emitter*/
    Vector2D    positionVariance;
    Vector2D    velocity;
    Vector2D    velocityVariance;
    Vector2D    acceleration;
    Vector2D    accelerationVariance;
    Vector4D    color;
    Vector4D    colorVector;
    Vector4D    colorVariance;
    Uint32      startFrame,endFrame,frameVariance;
    
};

/*eol@eof*/
