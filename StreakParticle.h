/** @file *//********************************************************************************************************

                                                      StreakParticle.h

                                            Copyright 2002, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/StreakParticle.h#11 $

    $NoKeywords: $

 *********************************************************************************************************************/

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "Particle.h"
#include <DirectXMath.h>

struct IDirect3DDevice11;

namespace Confetti
{
class BasicEmitter;

//! A line-shaped Particle whose length and direction depend on its velocity.
//
//! @ingroup	Particles
//!

class StreakParticle : public Particle
{
public:

    //! Constructor
    StreakParticle() = default;

    //! Constructor
    StreakParticle(BasicEmitter const *      pEmitter,
                   float                     lifetime,
                   float                     age,
                   DirectX::XMFLOAT3 const & position,
                   DirectX::XMFLOAT3 const & velocity,
                   DirectX::XMFLOAT3 const & color);

    // Destructor
    virtual ~StreakParticle() override = default;

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

    //! Returns the position of the particle's tail.
    DirectX::XMFLOAT3 const & GetTailPosition() const { return tail_; }

    // Vertex buffer info

    struct VBEntry
    {
        static int const NUM_VERTICES = 2;          //!< Number of vertices in the particle
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
        USAGE = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY
    };

    //! Vertex shader data declaration
    static D3DVERTEXELEMENT11 const aVSDataDeclarationInfo_[];

private:

    // Appearance data

    DirectX::XMFLOAT3 tail_;                         //!< Location of the tail
};
} // namespace Confetti
