#if !defined(CONFETTI_SPHEREPARTICLE_H)
#define CONFETTI_SPHEREPARTICLE_H

#pragma once

#include <Confetti/Particle.h>
#include <glm/glm.hpp>

namespace Vkx
{
    class Device;
}

namespace Confetti
{
//! A sphere-shaped lit Particle with a radius.
//!
//! @ingroup	Particles
//!

class SphereParticle : public Particle
{
public:

    //! Constructor.
    SphereParticle() = default;

    //! Constructor.
    SphereParticle(float             lifetime,
                   float             age,
                   glm::vec3 const & position,
                   glm::vec3 const & velocity,
                   glm::vec4 const & color,
                   float             radius);

    //! Destructor.
    virtual ~SphereParticle() override = default;

    //! @name Overrides Particle
    //@{
    void Initialize(float             lifetime,
                    float             age,
                    glm::vec3 const & position,
                    glm::vec3 const & velocity,
                    glm::vec4 const & color,
                    float             radius);

    virtual bool update(float dt) override;
    virtual void draw(std::shared_ptr<Vkx::Device> device) const override;
    //!@}

    //! Returns the particle's radius.
    float GetRadius() const { return radius_; }

    // Vertex buffer info

    struct VBEntry
    {
        glm::vec4 position;
        glm::vec4 color;
    };

//     static UINT32 constexpr FVF   = D3DFVF_XYZ | D3DFVF_DIFFUSE;

//     //! Vertex shader data declaration
//     static D3DVERTEXELEMENT11 const aVSDataDeclarationInfo_[];

private:

    // Appearance data

    float initialRadius_;                      // Radius (distance from center to edge) at birth.
    float radius_;                             // Current radius.
};
} // namespace Confetti

#endif // !defined(CONFETTI_SPHEREPARTICLE_H)
