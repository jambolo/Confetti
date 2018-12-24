/** @file *//********************************************************************************************************

                                                     Appearance.h

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Appearance.h#11 $

    $NoKeywords: $

********************************************************************************************************************/

#pragma once

#include <cassert>
#include <Dxx/Camera.h>

namespace Confetti
{
/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! %Appearance characteristics shared by multiple particles.
//
//! @ingroup	Controls
//!
//! @note	Some characteristics are not used by all particle types and actual usage of each characteristic is up
//!			to the Emitter and Particle implementations.

class Appearance
{
public:

    //! Constructor
    Appearance(Dxx::Camera const * pCamera,
               D3DXCOLOR const & colorRate = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f),
               float radiusRate      = 0.0f,
               float angularVelocity = 0.0f,
               D3DMATERIAL9 const *    pMaterial = 0,
               IDirect3DTexture9 *     pTexture  = 0,
               float size = 1.0f);

    // Destructor
    virtual ~Appearance();

    // Copy constructor
    Appearance(Appearance const & src);

    // Assignment operator
    Appearance & operator =(Appearance const & rhs);

    //! Updates the appearance values.
    void Update(float dt);

    //! Returns the material.
    D3DMATERIAL9 const * GetMaterial()              const { return m_pMaterial; }

    //! Returns the texture.
    IDirect3DTexture9 * GetTexture()                const { return m_pTexture; }

    //! Returns the rate of change in the color.
    D3DXCOLOR const & GetColorRate()                const { return m_ColorRate; }

    //! Returns the rate of change of the radius.
    float GetRadiusRate()                           const { return m_RadiusRate; }

    //! Returns the angular velocity.
    float GetRadialVelocity()                       const { return m_AngularVelocity; }

    //! Returns a pointer to the camera used to render.
    Dxx::Camera const * GetCamera()                 const { return m_pCamera; }

    //! Returns the current line width or point size.
    float GetSize() const { return m_Size; }

private:

    D3DMATERIAL9 const * m_pMaterial;               // The material.
    IDirect3DTexture9 * m_pTexture;                 // The texture.
    D3DXCOLOR m_ColorRate;                          // Color rate of change
    float m_RadiusRate;                             // Radius rate of change
    float m_AngularVelocity;                        // Angular velocity
    Dxx::Camera const * m_pCamera;                  // Rendering camera
    float m_Size;                                   // Particle size (width or radius)
};
} // namespace Confetti
