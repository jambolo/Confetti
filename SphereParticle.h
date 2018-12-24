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
#include <d3dx9math.h>

struct IDirect3DDevice9;

namespace Confetti
{
/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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
    SphereParticle(BasicEmitter const * pEmitter,
                   float                lifetime,
                   float                age,
                   D3DXVECTOR3 const &  position,
                   D3DXVECTOR3 const &  velocity,
                   D3DXCOLOR const &    color,
                   float                radius);

    // Destructor
    virtual ~SphereParticle();

    //! @name Overrides Particle
    //@{
    void Initialize(float               lifetime,
                    float               age,
                    D3DXVECTOR3 const & position,
                    D3DXVECTOR3 const & velocity,
                    D3DXCOLOR const &   color,
                    float               radius);

    virtual bool Update(float dt);
    virtual void Draw(IDirect3DDevice9 * pD3dDevice) const;
    //!@}

    //! Returns the particle's radius.
    float GetRadius() const;

    // Vertex buffer info

    struct VBEntry
    {
        D3DVECTOR position;
        D3DCOLOR color;
    };

    enum
    {
        POOL  = D3DPOOL_DEFAULT,
        FVF   = D3DFVF_XYZ | D3DFVF_DIFFUSE,
        USAGE = D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY
    };

    //! Vertex shader data declaration
    static D3DVERTEXELEMENT9 const m_aVSDataDeclarationInfo[];

private:

    // Appearance data

    float m_InitialRadius;                      // Radius (distance from center to edge) at birth.
    float m_Radius;                             // Current radius.
};
} // namespace Confetti
