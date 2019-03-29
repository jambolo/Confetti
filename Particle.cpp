#include "Particle.h"

#include "Appearance.h"
#include "Emitter.h"
#include "Environment.h"

#include <glm/geometric.hpp>
#include <glm/glm.hpp>

namespace Confetti
{
//! @param	pEmitter		The emitter that controls this particle
//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.

Particle::Particle(BasicEmitter const * emitter,
                   float                lifetime,
                   float                age,
                   glm::vec3 const &    position,
                   glm::vec3 const &    velocity,
                   glm::vec4 const &    color)
    : emitter_(emitter)
    , lifetime_(lifetime)
    , age_(age)
    , initialPosition_(position)
    , initialVelocity_(velocity)
    , position_(position)
    , velocity_(velocity)
    , initialColor_(color)
    , color_(color)
{
}

//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	color			Color at birth

void Particle::initialize(float             lifetime,
                          float             age,
                          glm::vec3 const & position,
                          glm::vec3 const & velocity,
                          glm::vec4 const & color)
{
    lifetime_        = lifetime;
    age_             = age;
    initialPosition_ = position;
    initialVelocity_ = velocity;
    initialColor_    = color;

    position_ = position;
    velocity_ = velocity;
    color_    = color;
}

//! @param	dt	The amount of time that has passed since the last update.
//!
//! @note	Methods overriding this method must call this first in order to determine the particle's age and if it
//!			has been reborn or not.

bool Particle::update(float dt)
{
    bool reborn;

    age_ += dt;

    // If the particle has not been born yet then do nothing

    if (age_ < 0.0f)
        return false;

    // See if the particle is (re)born this frame

    if (age_ < dt)
    {
        reborn = true;
    }
    else if (age_ >= lifetime_)
    {
        age_  -= lifetime_;
        reborn = true;
    }
    else
    {
        reborn = false;
    }

    glm::vec3 velocity = velocity_;
    glm::vec3 position = position_;
    glm::vec4 color = color_;

    // If (re)born, then reset to initial values and adjust dt

    if (reborn)
    {
        glm::vec3 emitterVelocity = emitter_->currentVelocity();
        glm::vec3 emitterPosition = emitter_->currentPosition();
        glm::vec3 initialVelocity = initialVelocity_;
        glm::vec3 initialPosition = initialPosition_;

        velocity = emitterVelocity + initialVelocity;
        position = emitterPosition + initialPosition;
        color    = initialColor_;
        dt       = age_;
    }

    Environment const * pE = emitter_->environment().get();
    Appearance const *  pA = emitter_->appearance().get();

    // Update velocity and position
    glm::vec3 dv;
    glm::vec3 ds;
    float     c = pE->airFriction();
    glm::vec3 g = pE->gravity();

    if (c != 0.0f)
    {
        glm::vec3 terminalVelocity = pE->terminalVelocity();
        glm::vec3 terminalDistance = pE->terminalDistance();
        float     ect1 = pE->ect1();

        dv = (terminalVelocity - velocity) * ect1;
        ds = terminalDistance - dv / c;
    }
    else
    {
        dv = g * dt;
        ds = (velocity + dv * 0.5f) * dt;
    }

    velocity += dv;
    position += ds;

    // Check for collision with clip planes
    Environment::ClipPlaneList const & clipPlanes = pE->clipPlanes();
    if (!clipPlanes.empty())
    {
        for (auto const & clip : clipPlanes)
        {
            if (glm::dot(clip, glm::vec4(position.x, position.y, position.z, 0.0f)) < 0.0f)
            {
                age_ -= lifetime_;
                return reborn;
            }
        }
    }

    // Check for collision with bounce planes
    Environment::BouncePlaneList const & bouncePlanes = pE->bouncePlanes();
    if (!bouncePlanes.empty())
    {
        for (auto const & bounce : bouncePlanes)
        {
            glm::vec4 const & plane = bounce.plane;
            if (glm::dot(plane, glm::vec4(position.x, position.y, position.z, 0.0f)) < 0.0f)
            {
                glm::vec3 normal(plane);
                float f = 1.0f + bounce.dampening;
                velocity -= normal * (f * glm::dot(normal, velocity));
                position -= normal * (f * glm::dot(plane, glm::vec4(position, 1.0f)));
            }
        }
    }

    // Update color
    color += pA->colorRate * dt;
    color  =  glm::clamp(color,  glm::zero<glm::vec4>(),  glm::one<glm::vec4>());

    velocity_ = velocity;
    position_ = position;
    color_    = color;

    return reborn;
}

// glm::vec3 Particle::Color() const
// {
//	float	r	= glm::limit( 0., initialColor_.R_ + colorDelta_.R_ * age_, 1. );
//	float	g	= glm::limit( 0., initialColor_.G_ + colorDelta_.G_ * age_, 1. );
//	float	b	= glm::limit( 0., initialColor_.B_ + colorDelta_.B_ * age_, 1. );
//	float	a	= glm::limit( 0., initialColor_.A_ + colorDelta_.A_ * age_, 1. );
//
//	return glm::vec3( r, g, b, a );
// }
//
//
// glm::vec3 Particle::Position() const
// {
//	Environment const &	e	= *emitter_->Environment();
//
//	if ( glm::IsCloseToZero( e.AirFriction() ) )
//	{
//		glm::vec3	position	= e.Gravity();
//
//		position *= ( 0.5f * age_ );
//		position += absoluteInitialVelocity_;
//		position *= age_;
//		position += absoluteInitialPosition_;
//
//		return position;
//
////		return absoluteInitialPosition_
////			   + ( absoluteInitialVelocity_
////				   + e.Gravity() * ( 0.5f * age_ )
////				 ) * age_;
//	}
//	else
//	{
//		// How this was derived:
//		// Acceleration due to air friction = -c * ( v - w ), so
//		//
//		//		ds2/dt2 = dv/dt = g - c * ( v - w )
//		//
//		// also note that "terminal velocity" vT = g/c + w:
//		//
//		//		ds2/dt2 = dv/dt = c * vT - cv
//		//
//		//		ds/dt = v = vT - e**( -c * t ) * e**( -c * D ) / c
//		//
//		// Assuming v = v0 at t = 0 ==> e**( -c * D ) / c = ( vT - v0 ):
//		//
//		//		ds/dt = v = vT - ( vT - v0 ) * e**( -c * t )
//		//
//		//		s = vT * t + ( vT - v0 ) / c * e**( -c * t ) + E
//		//
//		// Assuming s = s0 at t = 0 ==> E = s0 - ( vT - v0 ) / c:
//		//
//		//		s = s0 + vT * t - ( vT - v0 ) * ( 1 - e**( -c * t ) ) / c
//		//
//		// c <== e.AirFriction(), w <== e.WindVelocity(), t <== age_,
//		// g <== e.Gravity(), v0 <== absoluteInitialVelocity_,
//		// s0 <== absoluteInitialPosition_
//		//
//
//		double	ect1c	= ( 1. - exp( -e.AirFriction() * age_ ) ) / e.AirFriction();
//
//		return absoluteInitialPosition_
//			   + terminalVelocity_ * age_
//			   - ( terminalVelocity_ - absoluteInitialVelocity_ ) * ect1c;
//
//	}
// }
//
//
// glm::vec3 Particle::Velocity() const
// {
//	Environment const &	e	= *emitter_->Environment();
//
//	if ( glm::IsCloseToZero( e.AirFriction() ) )
//	{
//		return absoluteInitialVelocity_ + e.Gravity() * age_;
//	}
//	else
//	{
//		// How this was derived:
//		// Acceleration due to air friction = -c * ( v - w ), so
//		//
//		//		dv/dt = g - c * ( v - w )
//		//
//		// also note that "terminal velocity" vT = g/c + w:
//		//
//		//		dv/dt = c * vT - cv
//		//
//		//		v = vT - e**( -c * t ) * e**( -c * D ) / c
//		//
//		// Assuming v = v0 at t = 0, e**( -c * D ) / c = ( vT - v0 ):
//		//
//		//		v = vT - ( vT - v0 ) * e**( -c * t )
//		//
//		// c <== e.AirFriction(), w <== e.WindVelocity(), t <== age_,
//		// g <== e.Gravity(), v0 <== absoluteInitialVelocity_,
//
//		float		ect		= exp( -e.AirFriction() * age_ );
//
//		return terminalVelocity_ - ( terminalVelocity_ - absoluteInitialVelocity_ ) * ect;
//	}
// }
} // namespace Confetti
