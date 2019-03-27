#include "PointParticle.h"

#include "Appearance.h"
#include "Emitter.h"

#include <d3d11.h>
#include <glm/glm.hpp>
using namespace DirectX;

namespace Confetti
{
// Vertex shader data declaration info

D3DVERTEXELEMENT11 const PointParticle::aVSDataDeclarationInfo_[] =
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
    Particle::Initialize(lifetime, age, position, velocity, color);
}

bool PointParticle::Update(float dt)
{
    return Particle::Update(dt);
}

void PointParticle::Draw(std::shared_ptr<Vkx::Device> pD3dDevice) const
{
    // Nothing to do. The particles are drawn by the emitter. This function should not be called.
    assert(false);
}
} // namespace Confetti
