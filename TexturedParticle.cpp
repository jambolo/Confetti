#include "TexturedParticle.h"

#include "Appearance.h"
#include "Emitter.h"
#include "Environment.h"

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

namespace Confetti
{
// Vertex shader data declaration info

D3DVERTEXELEMENT11 const TexturedParticle::aVSDataDeclarationInfo_[] =
{
    { 0,  0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12,  D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
    { 0, 16,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    { 0, 24,  D3DDECLTYPE_FLOAT1,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
    { 0, 28,  D3DDECLTYPE_FLOAT1,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
    D3DDECL_END()
};

//! @param	pEmitter		The emitter that controls this particle
//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	color			Color at birth.
//! @param	radius			Radius at birth.
//! @param	rotation		Rotation at birth.

TexturedParticle::TexturedParticle(BasicEmitter const *      pEmitter,
                                   float                     lifetime,
                                   float                     age,
                                   DirectX::XMFLOAT3 const & position,
                                   DirectX::XMFLOAT3 const & velocity,
                                   DirectX::XMFLOAT4 const & color,
                                   float                     radius,
                                   float                     rotation /* = 0.0f*/)
    : Particle(pEmitter, lifetime, age, position, velocity, color)
    , initialRadius_(radius)
    , radius_(radius)
    , initialRotation_(rotation)
    , rotation_(rotation)
{
}

//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	color			Color at birth.
//! @param	radius			Radius at birth.
//! @param	rotation		Rotation at birth.

void TexturedParticle::Initialize(float                     lifetime,
                                  float                     age,
                                  DirectX::XMFLOAT3 const & position,
                                  DirectX::XMFLOAT3 const & velocity,
                                  DirectX::XMFLOAT4 const & color,
                                  float                     radius,
                                  float                     rotation)
{
    Particle::Initialize(lifetime, age, position, velocity, color);

    initialRadius_   = radius;
    initialRotation_ = rotation;

    radius_   = radius;
    rotation_ = rotation;
}

//
// @param	dt	The amount of time that has passed since the last update

bool TexturedParticle::Update(float dt)
{
    // Update base class

    bool const reborn = Particle::Update(dt);

    if (reborn)
    {
        radius_   = initialRadius_;
        rotation_ = initialRotation_;
        dt        = age_;
    }

    Appearance const * const pA = pEmitter_->appearance();

    // Update size and rotation

    radius_   += dt * pA->radiusRate();
    rotation_ += dt * pA->angularVelocity();

    return reborn;
}

void TexturedParticle::Draw(ID3D11Device * pD3dDevice) const
{
    // Nothing to do because the particle is drawn by the emitter. This function should not be called.
    assert(false);
}
} // namespace Confetti
