/** @file *//********************************************************************************************************

                                                  PointParticle.cpp

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/PointParticle.cpp#6 $

    $NoKeywords: $

********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "PointParticle.h"

#include "Appearance.h"
#include "Emitter.h"

#include "Misc/Max.h"

namespace Confetti
{
// Vertex shader data declaration info

D3DVERTEXELEMENT9 const PointParticle::m_aVSDataDeclarationInfo[] =
{
    { 0,  0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12,  D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
    D3DDECL_END()
};

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	pEmitter		The emitter that controls this particle
//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	color			Color at birth.

PointParticle::PointParticle(BasicEmitter const * pEmitter,
                             float                lifetime,
                             float                age,
                             D3DXVECTOR3 const &  position,
                             D3DXVECTOR3 const &  velocity,
                             D3DXCOLOR const &    color)
    : Particle(pEmitter, lifetime, age, position, velocity, color)
{
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

PointParticle::~PointParticle()
{
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void PointParticle::Initialize(float               lifetime,
                               float               age,
                               D3DXVECTOR3 const & position,
                               D3DXVECTOR3 const & velocity,
                               D3DXCOLOR const &   color)
{
    Particle::Initialize(lifetime, age, position, velocity, color);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool PointParticle::Update(float dt)
{
    return Particle::Update(dt);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void PointParticle::Draw(IDirect3DDevice9 * pD3dDevice) const
{
    // Nothing to do. The particles are drawn by the emitter. This function should not be called.
    assert(false);
}
} // namespace Confetti
