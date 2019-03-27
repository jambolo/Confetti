#pragma once

#if !defined(CONFETTI_APPEARANCE_H)
#define CONFETTI_APPEARANCE_H

#include <glm/glm.hpp>
#include <memory>

namespace Vkx
{
    class Camera;
    class Texture;
}

namespace Confetti
{
//! %Appearance characteristics shared by multiple particles.
//!
//! @ingroup	Controls
//!
//! @note	Some characteristics are not used by all particle types and actual usage of each characteristic is up
//!			to the Emitter and Particle implementations.

class Appearance
{
public:
    Vkx::Camera const * camera;            //!< Rendering camera
    std::shared_ptr<Vkx::Texture> texture; //!< The texture.
    glm::vec4 colorRate;                   //!< Color rate of change
    float radiusRate;                      //!< Radius rate of change
    float angularVelocity;                 //!< Angular velocity
    float size;                            //!< Particle size (width or radius)

    void update(float) { /* nothing to do */ }
};
} // namespace Confetti

#endif // !defined(CONFETTI_APPEARANCE_H)
