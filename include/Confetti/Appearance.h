#pragma once

#if !defined(CONFETTI_APPEARANCE_H)
#define CONFETTI_APPEARANCE_H

#include <DirectXMath.h>

struct ID3D11Texture2D;
namespace Dxx { class Camera; }

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
    explicit Appearance(Dxx::Camera const * pCamera,
                        DirectX::XMFLOAT4 const & colorRate = { 0.0f, 0.0f, 0.0f, 0.0f },
                        float radiusRate      = 0.0f,
                        float angularVelocity = 0.0f,
                        ID3D11Texture2D *         pTexture = nullptr,
                        float size = 1.0f);

    // Destructor
    virtual ~Appearance();

    // Copy constructor
    Appearance(Appearance const & src);

    // Assignment operator
    Appearance & operator =(Appearance const & rhs);

    //! Updates the appearance values.
    void update(float dt);

    //! Returns the texture.
    ID3D11Texture2D * texture() const { return pTexture_; }

    //! Returns the rate of change in the color.
    DirectX::XMFLOAT4 const & colorRate() const { return colorRate_; }

    //! Returns the rate of change of the radius.
    float radiusRate() const { return radiusRate_; }

    //! Returns the angular velocity.
    float angularVelocity() const { return angularVelocity_; }

    //! Returns a pointer to the camera used to render.
    Dxx::Camera const * camera() const { return pCamera_; }

    //! Returns the current line width or point size.
    float size() const { return size_; }

private:

    ID3D11Texture2D * pTexture_;    // The texture.
    DirectX::XMFLOAT4 colorRate_;   // Color rate of change
    float radiusRate_;              // Radius rate of change
    float angularVelocity_;         // Angular velocity
    Dxx::Camera const * pCamera_;   // Rendering camera
    float size_;                    // Particle size (width or radius)
};
} // namespace Confetti

#endif // !defined(CONFETTI_APPEARANCE_H)
