#pragma once

#if !defined(CONFETTI_SPHEREPARTICLE_H)
#define CONFETTI_SPHEREPARTICLE_H

#include "Particle.h"
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

struct ID3D11Device;

namespace Confetti
{
//! A sphere-shaped lit Particle with a radius.
//
//! @ingroup	Particles
//!

class SphereParticle : public Particle
{
public:

    //! Constructor
    SphereParticle() = default;

    //! Constructor
    SphereParticle(BasicEmitter const *      pEmitter,
                   float                     lifetime,
                   float                     age,
                   DirectX::XMFLOAT3 const & position,
                   DirectX::XMFLOAT3 const & velocity,
                   DirectX::XMFLOAT4 const & color,
                   float                     radius);

    // Destructor
    virtual ~SphereParticle() override = default;

    //! @name Overrides Particle
    //@{
    void Initialize(float                     lifetime,
                    float                     age,
                    DirectX::XMFLOAT3 const & position,
                    DirectX::XMFLOAT3 const & velocity,
                    DirectX::XMFLOAT4 const & color,
                    float                     radius);

    virtual bool Update(float dt) override;
    virtual void Draw(ID3D11Device * pD3dDevice) const override;
    //!@}

    //! Returns the particle's radius.
    float GetRadius() const { return radius_; }

    // Vertex buffer info

    struct VBEntry
    {
        DirectX::XMFLOAT4 position;
        DirectX::XMFLOAT4 color;
    };

//     static UINT32 constexpr FVF   = D3DFVF_XYZ | D3DFVF_DIFFUSE;

//     //! Vertex shader data declaration
//     static D3DVERTEXELEMENT11 const aVSDataDeclarationInfo_[];

private:

    // Appearance data

    float initialRadius_;                      // Radius (distance from center to edge) at birth.
    float radius_;                             // Current radius.
};
} // namespace Confetti

#endif // !defined(CONFETTI_SPHEREPARTICLE_H)
