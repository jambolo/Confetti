/** @file *//********************************************************************************************************

                                                     TexturedParticle.h

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/TexturedParticle.h#15 $

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

//! A square camera-facing Particle with a texture, radius, and 2D rotation.
//
//! @ingroup	Particles
//!

class TexturedParticle : public Particle
{
public:

    //! Constructor
    TexturedParticle() {}

    //! Constructor
    TexturedParticle(BasicEmitter const * pEmitter,
                     float                lifetime,
                     float                age,
                     D3DXVECTOR3 const &  position,
                     D3DXVECTOR3 const &  velocity,
                     D3DXCOLOR const &    color,
                     float                radius,
                     float                rotation);

    // Destructor
    virtual ~TexturedParticle();

    //! Initializes a particle constructed with the default constructor
    void Initialize(float               lifetime,
                    float               age,
                    D3DXVECTOR3 const & position,
                    D3DXVECTOR3 const & velocity,
                    D3DXCOLOR const &   color,
                    float               radius,
                    float               rotation = 0);

    //! @name Overrides Particle
    //@{
    virtual bool Update(float dt);
    virtual void Draw(IDirect3DDevice9 * pD3dDevice) const;
    //@}

    //! Returns the particle's current radius.
    float GetRadius() const { return m_Radius; }

    //! Returns the particle's current rotation.
    float GetRotation() const { return m_Rotation; }

    //! Vertex buffer info
    struct VBEntry
    {
        static int const NUM_VERTICES = 4;      //!< Number of vertices in the particle

        //! Vertex buffer entry
        struct Vertex
        {
            D3DVECTOR position;                 //!< Particle (not vertex!) position
            D3DCOLOR color;                     //!< Color
            FLOAT u, v;                         //!< Texture position
            FLOAT radius;                       //!< Radius of the particle
            FLOAT rotation;                     //!< Amount of rotation of the particle
        };

        Vertex v[NUM_VERTICES];                 //!< Vertex buffer entries
    };

    static UINT32 const POOL  = D3DPOOL_DEFAULT;                            //!< Vertex buffer memory pool
    static UINT32 const USAGE = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;      //!< Vertex buffer behavior

    //! Vertex shader data declaration
    static D3DVERTEXELEMENT9 const m_aVSDataDeclarationInfo[];

private:

    // Appearance data

    float m_InitialRadius;          // Radius (distance from center to edge) at birth.
    float m_Radius;                 // Current radius.

    float m_InitialRotation;        // Rotation at birth (0 is unrotated).
    float m_Rotation;               // Current rotation.
};
} // namespace Confetti
