#pragma once

#if !defined(CONFETTI_POINTPARTICLE_H)
#define CONFETTI_POINTPARTICLE_H

#include <Confetti/Particle.h>

#include <glm/glm.hpp>
struct ID3D11Device;

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
    PointParticle(BasicEmitter const * pEmitter,
                  float                lifetime,
                  float                age,
                  glm::vec3 const &    position,
                  glm::vec3 const &    velocity,
                  glm::vec4 const &    color);

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
    virtual bool Update(float dt) override;
    virtual void Draw(std::shared_ptr<Vkx::Device> pD3dDevice) const override;
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
