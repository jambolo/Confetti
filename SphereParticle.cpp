#include "SphereParticle.h"

#include "Appearance.h"
#include "Emitter.h"
#include "Environment.h"

#include <glm/glm.hpp>

namespace Confetti
{
//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	color			Color at birth.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	radius			Radius at birth.

SphereParticle::SphereParticle(float             lifetime,
                               float             age,
                               glm::vec3 const & position,
                               glm::vec3 const & velocity,
                               glm::vec4 const & color,
                               float             radius)
    : Particle(lifetime, age, position, velocity, color)
    , initialRadius_(radius)
{
}

//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	color			Color at birth.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	radius			Radius at birth.

void SphereParticle::Initialize(float             lifetime,
                                float             age,
                                glm::vec3 const & position,
                                glm::vec3 const & velocity,
                                glm::vec4 const & color,
                                float             radius)
{
    Particle::initialize(lifetime, age, position, velocity, color);

    initialRadius_ = radius;
}

//!
//! @param	dt	The amount of time that has passed since the last update

bool SphereParticle::update(float dt)
{
    // Update base class

    bool const reborn = Particle::update(dt);

    if (reborn)
    {
        radius_ = initialRadius_;
        dt      = age_;
    }

    // Update size and rotation

    radius_ += dt * emitter_->appearance()->radiusRate;

    return reborn;
}

void SphereParticle::draw(std::shared_ptr<Vkx::Device> device) const
{
    // Nothing to do because the particle is drawn by the emitter. This function should not be called.
    assert(false);
}
} // namespace Confetti
