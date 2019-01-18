/** @file *//********************************************************************************************************

                                                     Appearance.h

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Appearance.h#11 $

    $NoKeywords: $

********************************************************************************************************************/

#pragma once

#include <cassert>
#include <DirectXMath.h>
#include <Dxx/Camera.h>

class D3DMATERIAL9;

namespace Confetti
{
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
    Appearance(Dxx::Camera const *       pCamera,
               DirectX::XMFLOAT4 const & colorRate = DirectX::XMVectorZero(),
               float                     radiusRate      = 0.0f,
               float                     angularVelocity = 0.0f,
               D3DMATERIAL9 const *      pMaterial = nullptr,
               IDirect3DTexture11 *      pTexture  = nullptr,
               float                     size = 1.0f);

    // Destructor
    virtual ~Appearance();

    // Copy constructor
    Appearance(Appearance const & src);

    // Assignment operator
    Appearance & operator =(Appearance const & rhs);

    //! Updates the appearance values.
    void Update(float dt);

    //! Returns the material.
    D3DMATERIAL9 const * GetMaterial()              const { return pMaterial_; }

    //! Returns the texture.
    IDirect3DTexture11 * GetTexture()                const { return pTexture_; }

    //! Returns the rate of change in the color.
    DirectX::XMFLOAT4 const & GetColorRate()                const { return colorRate_; }

    //! Returns the rate of change of the radius.
    float GetRadiusRate()                           const { return radiusRate_; }

    //! Returns the angular velocity.
    float GetRadialVelocity()                       const { return angularVelocity_; }

    //! Returns a pointer to the camera used to render.
    Dxx::Camera const * GetCamera()                 const { return pCamera_; }

    //! Returns the current line width or point size.
    float GetSize() const { return size_; }

private:

    D3DMATERIAL9 const * pMaterial_;               // The material.
    IDirect3DTexture11 * pTexture_;                 // The texture.
    DirectX::XMFLOAT4 colorRate_;                          // Color rate of change
    float radiusRate_;                             // Radius rate of change
    float angularVelocity_;                        // Angular velocity
    Dxx::Camera const * pCamera_;                  // Rendering camera
    float size_;                                   // Particle size (width or radius)
};
} // namespace Confetti
