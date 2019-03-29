#include "StreakParticle.h"

#include "Appearance.h"
#include "Emitter.h"

namespace Confetti
{
// Vertex shader data declaration info

D3DVERTEXELEMENT11 const StreakParticle::aVSDataDeclarationInfo_[] =
{
    { 0,  0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12,  D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
    D3DDECL_END()
};

//! @param	pEmitter		The emitter that controls this particle
//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	color			Color at birth.

StreakParticle::StreakParticle(BasicEmitter const * pEmitter,
                               float                lifetime,
                               float                age,
                               glm::vec3 const &    position,
                               glm::vec3 const &    velocity,
                               glm::vec4 const &    color)
    : Particle(pEmitter, lifetime, age, position, velocity, color)
{
}

//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	color			Color at birth.

void StreakParticle::Initialize(float             lifetime,
                                float             age,
                                glm::vec3 const & position,
                                glm::vec3 const & velocity,
                                glm::vec4 const & color)
{
    Particle::initialize(lifetime, age, position, velocity, color);
}

bool StreakParticle::update(float dt)
{
    bool reborn;

    reborn = Particle::Update(dt);

    if (reborn)
        dt = age_;

    // Update the location of the tail

    glm::vec4 position = position_;
    glm::vec4 velocity = velocity_;

    glm::vec4 tail;
    tail = position - velocity * dt;

    tail_ = tail;
    return reborn;
}

void StreakParticle::draw(std::shared_ptr<Vkx::Device> device) const
{
    // Nothing to do here because all drawing is done by the emitter. This function should not be called
    assert(false);
}
} // namespace Confetti
