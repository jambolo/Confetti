#include "SphereParticle.h"

#include "Appearance.h"
#include "Emitter.h"
#include "Environment.h"

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

namespace Confetti
{
//! @param	pEmitter		The emitter that controls this particle
//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	color			Color at birth.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	radius			Radius at birth.

SphereParticle::SphereParticle(BasicEmitter const *      pEmitter,
                               float                     lifetime,
                               float                     age,
                               DirectX::XMFLOAT3 const & position,
                               DirectX::XMFLOAT3 const & velocity,
                               DirectX::XMFLOAT4 const & color,
                               float                     radius)
    : Particle(pEmitter, lifetime, age, position, velocity, color)
    , initialRadius_(radius)
{
}

//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	color			Color at birth.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	radius			Radius at birth.

void SphereParticle::Initialize(float                     lifetime,
                                float                     age,
                                DirectX::XMFLOAT3 const & position,
                                DirectX::XMFLOAT3 const & velocity,
                                DirectX::XMFLOAT4 const & color,
                                float                     radius)
{
    Particle::Initialize(lifetime, age, position, velocity, color);

    initialRadius_ = radius;
}

//!
//! @param	dt	The amount of time that has passed since the last update

bool SphereParticle::Update(float dt)
{
    // Update base class

    bool const reborn = Particle::Update(dt);

    if (reborn)
    {
        radius_ = initialRadius_;
        dt      = age_;
    }

    Appearance const * const pA = pEmitter_->appearance();

    // Update size and rotation

    radius_ += dt * pA->radiusRate();

    return reborn;
}

void SphereParticle::Draw(ID3D11Device * pD3dDevice) const
{
    // Nothing to do because the particle is drawn by the emitter. This function should not be called.
    assert(false);
}
} // namespace Confetti
