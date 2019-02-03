#include "Particle.h"

#include "Appearance.h"
#include "Emitter.h"
#include "Environment.h"

#include "MyMath/FastMath.h"
#include "MyMath/MyMath.h"

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
                   DirectX::XMFLOAT4 const & color)
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
                          DirectX::XMFLOAT4 const & color)
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

    DirectX::XMVECTOR velocity_simd(DirectX::XMLoadFloat3(&velocity_));
    DirectX::XMVECTOR position_simd(DirectX::XMLoadFloat3(&position_));

    // If (re)born, then reset to initial values and adjust dt

    if (reborn)
    {
        DirectX::XMVECTOR emitterVelocity_simd(DirectX::XMLoadFloat3(&pEmitter_->currentVelocity()));
        DirectX::XMVECTOR emitterPosition_simd(DirectX::XMLoadFloat3(&pEmitter_->currentPosition()));
        DirectX::XMVECTOR initialVelocity_simd(DirectX::XMLoadFloat3(&initialVelocity_));
        DirectX::XMVECTOR initialPosition_simd(DirectX::XMLoadFloat3(&initialPosition_));

        velocity_simd = emitterVelocity_simd + initialVelocity_simd;
        position_simd = emitterPosition_simd + initialPosition_simd;
        color_        = initialColor_;
        dt            = age_;
    }

    Environment const * pE = pEmitter_->environment();
    Appearance const *  pA = pEmitter_->appearance();

    // Update velocity and position
    DirectX::XMVECTOR dv_simd;
    DirectX::XMVECTOR ds_simd;
    float c = pE->GetAirFriction();
    DirectX::XMVECTOR g(DirectX::XMLoadFloat3(&pE->GetGravity()));

    if (c != 0.0f)
    {
        DirectX::XMVECTOR terminalVelocity_simd(DirectX::XMLoadFloat3(&pE->GetTerminalVelocity()));
        DirectX::XMVECTOR terminalDistance_simd(DirectX::XMLoadFloat3(&pE->GetTerminalDistance()));
        float ect1 = pE->GetEct1();

        dv_simd = (terminalVelocity_simd - velocity_simd) * ect1;
        ds_simd = terminalDistance_simd - dv_simd / c;
    }
    else
    {
        dv_simd = g * dt;
        ds_simd = (velocity_simd + dv_simd * 0.5f) * dt;
    }

    velocity_simd += dv_simd;
    position_simd += ds_simd;

    // Check for collision with clip planes
    Environment::ClipPlaneList const * pClipPlanes = pE->GetClipPlanes();
    if (pClipPlanes)
    {
        for (auto const & plane : *pClipPlanes)
        {
            DirectX::XMVECTOR plane_simd(DirectX::XMLoadFloat4(&plane));
            if (XMVector3Less(XMPlaneDotCoord(plane_simd, position_simd), DirectX::XMVectorZero()))
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
            DirectX::XMVECTOR plane_simd(DirectX::XMLoadFloat4(&plane.plane_));
            if (XMVector3Less(XMPlaneDotCoord(plane_simd, position_simd), DirectX::XMVectorZero()))
            {
                float f = 1.0f + plane.dampening_;
                velocity_simd -= plane_simd * (f * XMPlaneDotNormal(plane_simd, velocity_simd));
                position_simd -= plane_simd * (f * XMPlaneDotCoord( plane_simd, position_simd));
            }
        }
    }

    // Update color
    DirectX::XMVECTOR color_simd(DirectX::XMLoadFloat4(&color_));
    DirectX::XMVECTOR colorRate_simd(DirectX::XMLoadFloat4(&pA->colorRate()));
    color_simd += colorRate_simd * dt;
    color_simd = DirectX::XMVectorClamp(color_simd, DirectX::XMVectorZero(), DirectX::XMVectorSplatOne());

    DirectX::XMStoreFloat3(&velocity_, velocity_simd);
    DirectX::XMStoreFloat3(&position_, position_simd);
    DirectX::XMStoreFloat4(&color_, color_simd);

    return reborn;
}

// DirectX::XMFLOAT3 Particle::GetColor() const
// {
//	float	r	= MyMath::limit( 0., initialColor_.R_ + colorDelta_.R_ * age_, 1. );
//	float	g	= MyMath::limit( 0., initialColor_.G_ + colorDelta_.G_ * age_, 1. );
//	float	b	= MyMath::limit( 0., initialColor_.B_ + colorDelta_.B_ * age_, 1. );
//	float	a	= MyMath::limit( 0., initialColor_.A_ + colorDelta_.A_ * age_, 1. );
//
//	return DirectX::XMFLOAT3( r, g, b, a );
// }
//
//
// DirectX::XMFLOAT3 Particle::GetPosition() const
// {
//	Environment const &	e	= *pEmitter_->GetEnvironment();
//
//	if ( MyMath::IsCloseToZero( e.GetAirFriction() ) )
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
// DirectX::XMFLOAT3 Particle::GetVelocity() const
// {
//	Environment const &	e	= *pEmitter_->GetEnvironment();
//
//	if ( MyMath::IsCloseToZero( e.GetAirFriction() ) )
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
