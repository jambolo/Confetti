/** @file *//********************************************************************************************************

                                                   SphereParticle.h

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/SphereParticle.h#7 $

    $NoKeywords: $

********************************************************************************************************************/

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "Particle.h"
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

struct IDirect3DDevice11;

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
    SphereParticle() {}

    //! Constructor
    SphereParticle(BasicEmitter const *      pEmitter,
                   float                     lifetime,
                   float                     age,
                   DirectX::XMFLOAT3 const & position,
                   DirectX::XMFLOAT3 const & velocity,
                   DirectX::XMFLOAT3 const & color,
                   float                     radius);

    // Destructor
    virtual ~SphereParticle() override = default;

    //! @name Overrides Particle
    //@{
    void Initialize(float                     lifetime,
                    float                     age,
                    DirectX::XMFLOAT3 const & position,
                    DirectX::XMFLOAT3 const & velocity,
                    DirectX::XMFLOAT3 const & color,
                    float                     radius);

    virtual bool Update(float dt) override;
    virtual void Draw(IDirect3DDevice11 * pD3dDevice) const override;
    //!@}

    //! Returns the particle's radius.
    float GetRadius() const { return radius_; }

    // Vertex buffer info

    struct VBEntry
    {
        DirectX::XMFLOAT4 position;
        DirectX::XMFLOAT4 color;
    };

    enum
    {
        POOL  = D3DPOOL_DEFAULT,
        FVF   = D3DFVF_XYZ | D3DFVF_DIFFUSE,
        USAGE = D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY
    };

    //! Vertex shader data declaration
    static D3DVERTEXELEMENT11 const aVSDataDeclarationInfo_[];

private:

    // Appearance data

    float initialRadius_;                      // Radius (distance from center to edge) at birth.
    float radius_;                             // Current radius.
};
} // namespace Confetti
