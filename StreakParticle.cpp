#include "StreakParticle.h"

#include "Appearance.h"
#include "Emitter.h"

namespace Confetti
{
// Vertex shader data declaration info

D3DVERTEXELEMENT11 const StreakParticle::aVSDataDeclarationInfo_[] =
{
    { 0,  0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12,  D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
    D3DDECL_END()
};

//! @param	pEmitter		The emitter that controls this particle
//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	color			Color at birth.

StreakParticle::StreakParticle(BasicEmitter const *      pEmitter,
                               float                     lifetime,
                               float                     age,
                               DirectX::XMFLOAT3 const & position,
                               DirectX::XMFLOAT3 const & velocity,
                               DirectX::XMFLOAT4 const & color)
    : Particle(pEmitter, lifetime, age, position, velocity, color)
{
}

//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	color			Color at birth.

void StreakParticle::Initialize(float                     lifetime,
                                float                     age,
                                DirectX::XMFLOAT3 const & position,
                                DirectX::XMFLOAT3 const & velocity,
                                DirectX::XMFLOAT4 const & color)
{
    Particle::Initialize(lifetime, age, position, velocity, color);
}

bool StreakParticle::Update(float dt)
{
    bool reborn;

    reborn = Particle::Update(dt);

    if (reborn)
        dt = age_;

    // Update the location of the tail

    DirectX::XMVECTOR position_simd(XMLoadFloat3(&position_));
    DirectX::XMVECTOR velocity_simd(XMLoadFloat3(&velocity_));

    DirectX::XMVECTOR tail_simd;
    tail_simd = position_simd - velocity_simd * dt;

    XMStoreFloat3(&tail_, tail_simd);
    return reborn;
}

void StreakParticle::Draw(ID3D11Device * pD3dDevice) const
{
    // Nothing to do here because all drawing is done by the emitter. This function should not be called
    assert(false);
}
} // namespace Confetti
