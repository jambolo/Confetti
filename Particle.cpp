/** @file *//********************************************************************************************************

                                                     Particle.cpp

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Particle.cpp#10 $

    $NoKeywords: $

********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "Particle.h"

#include "Appearance.h"
#include "Emitter.h"
#include "Environment.h"

#include "Math/FastMath.h"
#include "Misc/Max.h"

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

namespace Confetti
{
//! @param	pEmitter		The emitter that controls this particle
//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.

Particle::Particle(BasicEmitter const *      pEmitter,
                   float                     lifetime,
                   float                     age,
                   DirectX::XMFLOAT3 const & position,
                   DirectX::XMFLOAT3 const & velocity,
                   DirectX::XMFLOAT3 const & color)
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

void Particle::Initialize(float                     lifetime,
                          float                     age,
                          DirectX::XMFLOAT3 const & position,
                          DirectX::XMFLOAT3 const & velocity,
                          DirectX::XMFLOAT3 const & color)
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

    // If (re)born, then reset to initial values and adjust dt

    if (reborn)
    {
        velocity_ = pEmitter_->GetCurrentVelocity() + initialVelocity_;
        position_ = pEmitter_->GetCurrentPosition() + initialPosition_;
        color_    = initialColor_;
        dt        = age_;
    }

    Environment const * const pE = pEmitter_->GetEnvironment();
    Appearance const * const  pA = pEmitter_->GetAppearance();

    // Update velocity and position

    DirectX::XMFLOAT3         dv;
    DirectX::XMFLOAT3         ds;
    float const               c = pE->GetAirFriction();
    DirectX::XMFLOAT3 const & g = pE->GetGravity();

    if (c != 0.0f)
    {
        DirectX::XMFLOAT3 const & terminalVelocity = pE->GetTerminalVelocity();
        DirectX::XMFLOAT3 const & terminalDistance = pE->GetTerminalDistance();
        float const ect1 = pE->GetEct1();

        dv = (terminalVelocity - velocity_) * ect1;
        ds = terminalDistance - dv / c;
    }
    else
    {
        dv = g * dt;
        ds = (velocity_ + dv * 0.5f) * dt;
    }

    velocity_ += dv;
    position_ += ds;

    // Check for collision with clip planes

    Environment::ClipPlaneList const * pClipPlanes = pE->GetClipPlanes();

    if (pClipPlanes != 0)
    {
        for (Environment::ClipPlaneList::const_iterator pC = pClipPlanes->begin(); pC != pClipPlanes->end(); ++pC)
        {
            DirectX::XMFLOAT3 const & plane = *pC;

            if (XMPlaneDotCoord(plane, position_) < 0.0f)
            {
                age_ -= lifetime_;
                return reborn;
            }
        }
    }

    // Check for collision with bounce planes

    Environment::BouncePlaneList const * pBouncePlanes = pE->GetBouncePlanes();

    if (pBouncePlanes != 0)
    {
        for (Environment::BouncePlaneList::const_iterator pB = pBouncePlanes->begin(); pB != pBouncePlanes->end(); ++pB)
        {
            Environment::BouncePlane const & plane = *pB;

            if (XMPlaneDotCoord(&plane.plane_, &position_) < 0.0f)
            {
                float const f = 1.0f + plane.dampening_;
                velocity_ -=  DirectX::XMFLOAT3(plane.plane_) * (f * XMPlaneDotNormal(&plane.plane_, &velocity_));
                position_ -=  DirectX::XMFLOAT3(plane.plane_) * (f * XMPlaneDotCoord(&plane.plane_, &position_));
            }
        }
    }

    // Update color

    color_  += pA->GetColorRate() * dt;
    color_.r = limit(0.0f, color_.r, 1.0f);
    color_.g = limit(0.0f, color_.g, 1.0f);
    color_.b = limit(0.0f, color_.b, 1.0f);
    color_.a = limit(0.0f, color_.a, 1.0f);

    return reborn;
}

/// ********************************************************************************************************************/
/// *																													*/
/// ********************************************************************************************************************/
//
// DirectX::XMFLOAT3 Particle::GetColor() const
// {
//	float const	r	= limit( 0., initialColor_.R_ + colorDelta_.R_ * age_, 1. );
//	float const	g	= limit( 0., initialColor_.G_ + colorDelta_.G_ * age_, 1. );
//	float const	b	= limit( 0., initialColor_.B_ + colorDelta_.B_ * age_, 1. );
//	float const	a	= limit( 0., initialColor_.A_ + colorDelta_.A_ * age_, 1. );
//
//	return DirectX::XMFLOAT3( r, g, b, a );
// }
//
//
/// ********************************************************************************************************************/
/// *																													*/
/// ********************************************************************************************************************/
//
// DirectX::XMFLOAT3 Particle::GetPosition() const
// {
//	Environment const &	e	= *pEmitter_->GetEnvironment();
//
//	if ( Math::IsCloseToZero( e.GetAirFriction() ) )
//	{
//		DirectX::XMFLOAT3	position	= e.GetGravity();
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
//		double const	ect1c	= ( 1. - exp( -e.GetAirFriction() * age_ ) ) / e.GetAirFriction();
//
//		return absoluteInitialPosition_
//			   + terminalVelocity_ * age_
//			   - ( terminalVelocity_ - absoluteInitialVelocity_ ) * ect1c;
//
//	}
// }
//
/// ********************************************************************************************************************/
/// *																													*/
/// ********************************************************************************************************************/
//
// DirectX::XMFLOAT3 Particle::GetVelocity() const
// {
//	Environment const &	e	= *pEmitter_->GetEnvironment();
//
//	if ( Math::IsCloseToZero( e.GetAirFriction() ) )
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
//		float const		ect		= exp( -e.GetAirFriction() * age_ );
//
//		return terminalVelocity_ - ( terminalVelocity_ - absoluteInitialVelocity_ ) * ect;
//	}
// }
} // namespace Confetti
