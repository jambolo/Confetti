/** @file *//********************************************************************************************************

                                                     TexturedParticle.h

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/TexturedParticle.h#15 $

    $NoKeywords: $

********************************************************************************************************************/

#pragma once

#include "Particle.h"
#include <d3d11.h>
#include <DirectXMath.h>

struct IDirect3DDevice11;

namespace Confetti
{
//! A square camera-facing Particle with a texture, radius, and 2D rotation.
//
//! @ingroup	Particles
//!

class TexturedParticle : public Particle
{
public:

    //! Constructor
    TexturedParticle() = default;

    //! Constructor
    TexturedParticle(BasicEmitter const *      pEmitter,
                     float                     lifetime,
                     float                     age,
                     DirectX::XMFLOAT3 const & position,
                     DirectX::XMFLOAT3 const & velocity,
                     DirectX::XMFLOAT3 const & color,
                     float                     radius,
                     float                     rotation);

    // Destructor
    virtual ~TexturedParticle() override = default;

    //! Initializes a particle constructed with the default constructor
    void Initialize(float                     lifetime,
                    float                     age,
                    DirectX::XMFLOAT3 const & position,
                    DirectX::XMFLOAT3 const & velocity,
                    DirectX::XMFLOAT3 const & color,
                    float                     radius,
                    float                     rotation = 0);

    //! @name Overrides Particle
    //@{
    virtual bool Update(float dt) override;
    virtual void Draw(IDirect3DDevice11 * pD3dDevice) const override;
    //@}

    //! Returns the particle's current radius.
    float GetRadius() const { return radius_; }

    //! Returns the particle's current rotation.
    float GetRotation() const { return rotation_; }

    //! Vertex buffer info
    struct VBEntry
    {
        static int const NUM_VERTICES = 4;      //!< Number of vertices in the particle

        //! Vertex buffer entry
        struct Vertex
        {
            DirectX::XMFLOAT3 position; //!< Particle (not vertex!) position
            DirectX::XMFLOAT3 color;    //!< Color
            FLOAT u, v;                 //!< Texture position
            FLOAT radius;               //!< Radius of the particle
            FLOAT rotation;             //!< Amount of rotation of the particle
        };

        Vertex v[NUM_VERTICES]; //!< Vertex buffer entries
    };

    static UINT32 const POOL  = D3DPOOL_DEFAULT;                            //!< Vertex buffer memory pool
    static UINT32 const USAGE = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;      //!< Vertex buffer behavior

    //! Vertex shader data declaration
    static D3DVERTEXELEMENT11 const aVSDataDeclarationInfo_[];

private:

    // Appearance data

    float initialRadius_;          // Radius (distance from center to edge) at birth.
    float radius_;                 // Current radius.

    float initialRotation_;        // Rotation at birth (0 is unrotated).
    float rotation_;               // Current rotation.
};
} // namespace Confetti
