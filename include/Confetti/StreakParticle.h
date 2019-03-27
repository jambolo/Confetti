#pragma once

#if !defined(CONFETTI_STREAKPARTICLE_H)
#define CONFETTI_STREAKPARTICLE_H

#include <Confetti/Particle.h>
#include <glm/glm.hpp>

struct ID3D11Device;

namespace Confetti
{
class BasicEmitter;

//! A line-shaped Particle whose length and direction depend on its velocity.
//!
//! @ingroup	Particles
//!

class StreakParticle : public Particle
{
public:

    //! Constructor.
    StreakParticle() = default;

    //! Constructor.
    StreakParticle(BasicEmitter const * pEmitter,
                   float                lifetime,
                   float                age,
                   glm::vec3 const &    position,
                   glm::vec3 const &    velocity,
                   glm::vec4 const &    color);

    //! Destructor.
    virtual ~StreakParticle() override = default;

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

    //! Returns the position of the particle's tail.
    glm::vec3 const & GetTailPosition() const { return tail_; }

    //! Vertex buffer info
    struct VBEntry
    {
        static int const NUM_VERTICES = 2;          //!< Number of vertices in the particle
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;
        };

        Vertex v[NUM_VERTICES];
    };

//
//     //! Vertex shader data declaration
//     static D3DVERTEXELEMENT11 const aVSDataDeclarationInfo_[];

private:

    // Appearance data

    glm::vec3 tail_;                         //!< Location of the tail
};
} // namespace Confetti

#endif // !defined(CONFETTI_STREAKPARTICLE_H)
