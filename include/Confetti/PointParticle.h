#if !defined(CONFETTI_POINTPARTICLE_H)
#define CONFETTI_POINTPARTICLE_H

#pragma once

#include <Confetti/Particle.h>

#include <glm/glm.hpp>
namespace Vkx
{
    class Device;
}

namespace Confetti
{
class BasicEmitter;

//! A point Particle scaled by distance.
//!
//! @ingroup	Particles
//!

class PointParticle : public Particle
{
public:

    //! Constructor.
    PointParticle() = default;

    //! Constructor.
    PointParticle(float             lifetime,
                  float             age,
                  glm::vec3 const & position,
                  glm::vec3 const & velocity,
                  glm::vec4 const & color);

    //! Destructor.
    virtual ~PointParticle() override = default;

    //! Initializes a particle constructed with the default constructor
    void Initialize(float             lifetime,
                    float             age,
                    glm::vec3 const & position,
                    glm::vec3 const & velocity,
                    glm::vec4 const & color);

    //! @name Overrides Particle
    //@{
    virtual bool update(float dt) override;
    virtual void draw(std::shared_ptr<Vkx::Device> device) const override;
    //@}

    //! Vertex buffer info.
    struct VBEntry
    {
        static int const NUM_VERTICES = 1;
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;
        };

        Vertex v[NUM_VERTICES];
    };

//     //! Vertex shader data declaration
//     static D3DVERTEXELEMENT11 const aVSDataDeclarationInfo_[];
};
} // namespace Confetti

#endif // !defined(CONFETTI_POINTPARTICLE_H)
