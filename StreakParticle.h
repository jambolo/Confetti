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
#include <d3dx9math.h>

struct IDirect3DDevice9;

namespace Confetti
{
class BasicEmitter;

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! A line-shaped Particle whose length and direction depend on its velocity.
//
//! @ingroup	Particles
//!

class StreakParticle : public Particle
{
public:

    //! Constructor
    StreakParticle() {}

    //! Constructor
    StreakParticle(BasicEmitter const * pEmitter,
                   float                lifetime,
                   float                age,
                   D3DXVECTOR3 const &  position,
                   D3DXVECTOR3 const &  velocity,
                   D3DXCOLOR const &    color);

    // Destructor
    virtual ~StreakParticle();

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

    //! Returns the position of the particle's tail.
    D3DXVECTOR3 const & GetTailPosition() const { return m_Tail; }

    // Vertex buffer info

    struct VBEntry
    {
        static int const NUM_VERTICES = 2;          //!< Number of vertices in the particle
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
        USAGE = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY
    };

    //! Vertex shader data declaration
    static D3DVERTEXELEMENT9 const m_aVSDataDeclarationInfo[];

private:

    // Appearance data

    D3DXVECTOR3 m_Tail;                         //!< Location of the tail
};
} // namespace Confetti
