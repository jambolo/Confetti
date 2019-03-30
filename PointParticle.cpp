#include "PointParticle.h"

#include "Appearance.h"
#include "Emitter.h"

#include <glm/glm.hpp>

namespace Confetti
{
// Vertex shader data declaration info

// D3DVERTEXELEMENT11 const PointParticle::aVSDataDeclarationInfo_[] =
// {
//     { 0,  0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
//     { 0, 12,  D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
//     D3DDECL_END()
// };

//! @param	pEmitter		The emitter that controls this particle
//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	color			Color at birth.

PointParticle::PointParticle(BasicEmitter const * pEmitter,
                             float                lifetime,
                             float                age,
                             glm::vec3 const &    position,
                             glm::vec3 const &    velocity,
                             glm::vec4 const &    color)
    : Particle(pEmitter, lifetime, age, position, velocity, color)
{
}

void PointParticle::Initialize(float             lifetime,
                               float             age,
                               glm::vec3 const & position,
                               glm::vec3 const & velocity,
                               glm::vec4 const & color)
{
    Particle::initialize(lifetime, age, position, velocity, color);
}

bool PointParticle::update(float dt)
{
    return Particle::update(dt);
}

void PointParticle::draw(std::shared_ptr<Vkx::Device> device) const
{
    // Nothing to do. The particles are drawn by the emitter. This function should not be called.
    assert(false);
}
} // namespace Confetti
