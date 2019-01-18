/** @file *//********************************************************************************************************

                                                   PointParticle.h

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/PointParticle.h#7 $

    $NoKeywords: $

********************************************************************************************************************/

#pragma once

#include "Particle.h"

#include <d3d11.h>
#include <DirectXMath.h>
struct IDirect3DDevice11;

namespace Confetti
{
class BasicEmitter;

//! A point Particle scaled by distance.
//
//! @ingroup	Particles
//!

class PointParticle : public Particle
{
public:

    //! Constructor
    PointParticle() = default;

    //! Constructor
    PointParticle(BasicEmitter const *      pEmitter,
                  float                     lifetime,
                  float                     age,
                  DirectX::XMFLOAT3 const & position,
                  DirectX::XMFLOAT3 const & velocity,
                  DirectX::XMFLOAT3 const & color);

    // Destructor
    virtual ~PointParticle() override = default;

    //! Initializes a particle constructed with the default constructor
    void Initialize(float                     lifetime,
                    float                     age,
                    DirectX::XMFLOAT3 const & position,
                    DirectX::XMFLOAT3 const & velocity,
                    DirectX::XMFLOAT3 const & color);

    //! @name Overrides Particle
    //@{
    virtual bool Update(float dt) override;
    virtual void Draw(IDirect3DDevice11 * pD3dDevice) const override;
    //@}

    // Vertex buffer info

    struct VBEntry
    {
        static int const NUM_VERTICES = 1;
        struct Vertex
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT3 color;
        };

        Vertex v[NUM_VERTICES];
    };

    enum
    {
        POOL  = D3DPOOL_DEFAULT,
        USAGE = D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY
    };

    //! Vertex shader data declaration
    static D3DVERTEXELEMENT11 const aVSDataDeclarationInfo_[];
};
} // namespace Confetti
