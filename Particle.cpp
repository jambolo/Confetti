#include "Particle.h"

#include "Appearance.h"
#include "Emitter.h"
#include "Environment.h"

#include <glm/glm.h>

#include <d3d11.h>
#include <glm/glm.hpp>
using namespace DirectX;

namespace Confetti
{
//! @param	pEmitter		The emitter that controls this particle
//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.

Particle::Particle(BasicEmitter const * pEmitter,
                   float                lifetime,
                   float                age,
                   glm::vec3 const &    position,
                   glm::vec3 const &    velocity,
                   glm::vec4 const &    color)
    : pEmitter_(pEmitter)
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

void Particle::Initialize(float             lifetime,
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

bool Particle::Update(float dt)
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

    glm::vec4 velocity(DirectX::XMLoadFloat3(&velocity_));
    glm::vec4 position(DirectX::XMLoadFloat3(&position_));

    // If (re)born, then reset to initial values and adjust dt

    if (reborn)
    {
        glm::vec4 emitterVelocity(DirectX::XMLoadFloat3(&pEmitter_->currentVelocity()));
        glm::vec4 emitterPosition(DirectX::XMLoadFloat3(&pEmitter_->currentPosition()));
        glm::vec4 initialVelocity(DirectX::XMLoadFloat3(&initialVelocity_));
        glm::vec4 initialPosition(DirectX::XMLoadFloat3(&initialPosition_));

        velocity = emitterVelocity + initialVelocity;
        position = emitterPosition + initialPosition;
        color_   = initialColor_;
        dt       = age_;
    }

    Environment const * pE = pEmitter_->environment();
    Appearance const *  pA = pEmitter_->appearance();

    // Update velocity and position
    glm::vec4 dv;
    glm::vec4 ds;
    float     c = pE->GetAirFriction();
    glm::vec4 g(DirectX::XMLoadFloat3(&pE->GetGravity()));

    if (c != 0.0f)
    {
        glm::vec4 terminalVelocity(DirectX::XMLoadFloat3(&pE->GetTerminalVelocity()));
        glm::vec4 terminalDistance(DirectX::XMLoadFloat3(&pE->GetTerminalDistance()));
        float     ect1 = pE->GetEct1();

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
    Environment::ClipPlaneList const * pClipPlanes = pE->GetClipPlanes();
    if (pClipPlanes)
    {
        for (auto const & plane : *pClipPlanes)
        {
            glm::vec4 plane(DirectX::XMLoadFloat4(&plane));
            if (XMVector3Less(XMPlaneDotCoord(plane, position), DirectX::XMVectorZero()))
            {
                age_ -= lifetime_;
                return reborn;
            }
        }
    }

    // Check for collision with bounce planes
    Environment::BouncePlaneList const * pBouncePlanes = pE->GetBouncePlanes();
    if (pBouncePlanes)
    {
        for (auto const & plane : *pBouncePlanes)
        {
            glm::vec4 plane(DirectX::XMLoadFloat4(&plane.plane_));
            if (XMVector3Less(XMPlaneDotCoord(plane, position), DirectX::XMVectorZero()))
            {
                float f = 1.0f + plane.dampening_;
                velocity -= plane * (f * XMPlaneDotNormal(plane, velocity));
                position -= plane * (f * XMPlaneDotCoord(plane, position));
            }
        }
    }

    // Update color
    glm::vec4 color(DirectX::XMLoadFloat4(&color_));
    glm::vec4 colorRate(DirectX::XMLoadFloat4(&pA->colorRate()));
    color += colorRate * dt;
    color  = DirectX::XMVectorClamp(color, DirectX::XMVectorZero(), DirectX::XMVectorSplatOne());

    DirectX::velocity_ = velocity;
    DirectX::position_ = position;
    DirectX::color_    = color;

    return reborn;
}

// glm::vec3 Particle::GetColor() const
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
// glm::vec3 Particle::GetPosition() const
// {
//	Environment const &	e	= *pEmitter_->GetEnvironment();
//
//	if ( glm::IsCloseToZero( e.GetAirFriction() ) )
//	{
//		glm::vec3	position	= e.GetGravity();
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
////				   + e.GetGravity() * ( 0.5f * age_ )
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
//		// c <== e.GetAirFriction(), w <== e.GetWindVelocity(), t <== age_,
//		// g <== e.GetGravity(), v0 <== absoluteInitialVelocity_,
//		// s0 <== absoluteInitialPosition_
//		//
//
//		double	ect1c	= ( 1. - exp( -e.GetAirFriction() * age_ ) ) / e.GetAirFriction();
//
//		return absoluteInitialPosition_
//			   + terminalVelocity_ * age_
//			   - ( terminalVelocity_ - absoluteInitialVelocity_ ) * ect1c;
//
//	}
// }
//
//
// glm::vec3 Particle::GetVelocity() const
// {
//	Environment const &	e	= *pEmitter_->GetEnvironment();
//
//	if ( glm::IsCloseToZero( e.GetAirFriction() ) )
//	{
//		return absoluteInitialVelocity_ + e.GetGravity() * age_;
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
//		// c <== e.GetAirFriction(), w <== e.GetWindVelocity(), t <== age_,
//		// g <== e.GetGravity(), v0 <== absoluteInitialVelocity_,
//
//		float		ect		= exp( -e.GetAirFriction() * age_ );
//
//		return terminalVelocity_ - ( terminalVelocity_ - absoluteInitialVelocity_ ) * ect;
//	}
// }
} // namespace Confetti
