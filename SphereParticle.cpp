/** @file *//********************************************************************************************************

                                                  SphereParticle.cpp

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/SphereParticle.cpp#8 $

    $NoKeywords: $

********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "SphereParticle.h"

#include "Appearance.h"
#include "Emitter.h"
#include "Environment.h"

namespace Confetti
{
/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	pEmitter		The emitter that controls this particle
//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	color			Color at birth.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	radius			Radius at birth.

SphereParticle::SphereParticle(BasicEmitter const * pEmitter,
                               float                lifetime,
                               float                age,
                               D3DXVECTOR3 const &  position,
                               D3DXVECTOR3 const &  velocity,
                               D3DXCOLOR const &    color,
                               float                radius)
    : Particle(pEmitter, lifetime, age, position, velocity, color),
    m_InitialRadius(radius)
{
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

SphereParticle::~SphereParticle()
{
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	color			Color at birth.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	radius			Radius at birth.

void SphereParticle::Initialize(float               lifetime,
                                float               age,
                                D3DXVECTOR3 const & position,
                                D3DXVECTOR3 const & velocity,
                                D3DXCOLOR const &   color,
                                float               radius)
{
    Particle::Initialize(lifetime, age, position, velocity, color);

    m_InitialRadius = radius;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//!
//! @param	dt	The amount of time that has passed since the last update

bool SphereParticle::Update(float dt)
{
    // Update base class

    bool const reborn = Particle::Update(dt);

    if (reborn)
    {
        m_Radius = m_InitialRadius;
        dt       = m_Age;
    }

    Appearance const * const pA = m_pEmitter->GetAppearance();

    // Update size and rotation

    m_Radius += dt * pA->GetRadiusRate();

    return reborn;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void SphereParticle::Draw(IDirect3DDevice9 * pD3dDevice) const
{
    // Nothing to do because the particle is drawn by the emitter. This function should not be called.
    assert(false);
}
} // namespace Confetti
