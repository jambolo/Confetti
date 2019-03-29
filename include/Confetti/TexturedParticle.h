#pragma once

#if !defined(CONFETTI_TEXTUREDPARTICLE_H)
#define CONFETTI_TEXTUREDPARTICLE_H

#include <Confetti/Particle.h>
#include <glm/glm.hpp>

namespace Vkx
{
    class Device;
}

namespace Confetti
{
//! A square camera-facing Particle with a texture, radius, and 2D rotation.
//!
//! @ingroup	Particles
//!

class TexturedParticle : public Particle
{
public:

    //! Constructor.
    TexturedParticle() = default;

    //! Constructor.
    TexturedParticle(BasicEmitter const * pEmitter,
                     float                lifetime,
                     float                age,
                     glm::vec3 const &    position,
                     glm::vec3 const &    velocity,
                     glm::vec4 const &    color,
                     float                radius,
                     float                rotation);

    //! Destructor.
    virtual ~TexturedParticle() override = default;

    //! Initializes a particle constructed with the default constructor
    void Initialize(float             lifetime,
                    float             age,
                    glm::vec3 const & position,
                    glm::vec3 const & velocity,
                    glm::vec4 const & color,
                    float             radius,
                    float             rotation = 0);

    //! @name Overrides Particle
    //@{
    virtual bool update(float dt) override;
    virtual void draw(std::shared_ptr<Vkx::Device> device) const override;
    //@}

    //! Returns the particle's current radius.
    float radius() const { return radius_; }

    //! Returns the particle's current rotation.
    float rotation() const { return rotation_; }

    //! Vertex buffer info
    struct VBEntry
    {
        static int constexpr NUM_VERTICES = 4;      //!< Number of vertices in the particle

        //! Vertex buffer entry
        struct Vertex
        {
            glm::vec3 position; //!< Particle (not vertex!) position
            glm::vec3 color;    //!< Color
            float u, v;         //!< Texture position
            float radius;       //!< Radius of the particle
            float rotation;     //!< Amount of rotation of the particle
        };

        Vertex v[NUM_VERTICES]; //!< Vertex buffer entries
    };

//     //! Vertex shader data declaration
//     static D3DVERTEXELEMENT11 const aVSDataDeclarationInfo_[];

private:

    // Appearance data

    float initialRadius_;          // Radius (distance from center to edge) at birth.
    float radius_;                 // Current radius.

    float initialRotation_;        // Rotation at birth (0 is unrotated).
    float rotation_;               // Current rotation.
};
} // namespace Confetti

#endif // !defined(CONFETTI_TEXTUREDPARTICLE_H)
