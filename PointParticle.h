/** @file *//********************************************************************************************************

                                                   PointParticle.h

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/PointParticle.h#7 $

    $NoKeywords: $

********************************************************************************************************************/

#pragma once

#include "Particle.h"

#include <d3dx9math.h>

struct IDirect3DDevice9;

namespace Confetti
{
class BasicEmitter;

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! A point Particle scaled by distance.
//
//! @ingroup	Particles
//!

class PointParticle : public Particle
{
public:

    //! Constructor
    PointParticle() {}

    //! Constructor
    PointParticle(BasicEmitter const * pEmitter,
                  float                lifetime,
                  float                age,
                  D3DXVECTOR3 const &  position,
                  D3DXVECTOR3 const &  velocity,
                  D3DXCOLOR const &    color);

    // Destructor
    virtual ~PointParticle();

    //! Initializes a particle constructed with the default constructor
    void Initialize(float               lifetime,
                    float               age,
                    D3DXVECTOR3 const & position,
                    D3DXVECTOR3 const & velocity,
                    D3DXCOLOR const &   color);

    //! @name Overrides Particle
    //@{
    virtual bool Update(float dt);
    virtual void Draw(IDirect3DDevice9 * pD3dDevice) const;
    //@}

    // Vertex buffer info

    struct VBEntry
    {
        static int const NUM_VERTICES = 1;
        struct Vertex
        {
            D3DVECTOR position;
            D3DCOLOR color;
        };

        Vertex v[NUM_VERTICES];
    };

    enum
    {
        POOL  = D3DPOOL_DEFAULT,
        USAGE = D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY
    };

    //! Vertex shader data declaration
    static D3DVERTEXELEMENT9 const m_aVSDataDeclarationInfo[];
};
} // namespace Confetti
