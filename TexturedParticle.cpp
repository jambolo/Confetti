/** @file *//********************************************************************************************************

                                                 TexturedParticle.cpp

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/TexturedParticle.cpp#13 $

    $NoKeywords: $

********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "TexturedParticle.h"

#include "Appearance.h"
#include "Emitter.h"
#include "Environment.h"

namespace Confetti
{
// Vertex shader data declaration info

D3DVERTEXELEMENT9 const TexturedParticle::m_aVSDataDeclarationInfo[] =
{
    { 0,  0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12,  D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
    { 0, 16,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    { 0, 24,  D3DDECLTYPE_FLOAT1,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
    { 0, 28,  D3DDECLTYPE_FLOAT1,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
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
//! @param	radius			Radius at birth.
//! @param	rotation		Rotation at birth.

TexturedParticle::TexturedParticle(BasicEmitter const * pEmitter,
                                   float                lifetime,
                                   float                age,
                                   D3DXVECTOR3 const &  position,
                                   D3DXVECTOR3 const &  velocity,
                                   D3DXCOLOR const &    color,
                                   float                radius,
                                   float                rotation /* = 0.0f*/)
    : Particle(pEmitter, lifetime, age, position, velocity, color),
    m_InitialRadius(radius),
    m_Radius(radius),
    m_InitialRotation(rotation),
    m_Rotation(rotation)
{
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

TexturedParticle::~TexturedParticle()
{
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	color			Color at birth.
//! @param	radius			Radius at birth.
//! @param	rotation		Rotation at birth.

void TexturedParticle::Initialize(float               lifetime,
                                  float               age,
                                  D3DXVECTOR3 const & position,
                                  D3DXVECTOR3 const & velocity,
                                  D3DXCOLOR const &   color,
                                  float               radius,
                                  float               rotation)
{
    Particle::Initialize(lifetime, age, position, velocity, color);

    m_InitialRadius   = radius;
    m_InitialRotation = rotation;

    m_Radius   = radius;
    m_Rotation = rotation;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//
// @param	dt	The amount of time that has passed since the last update

bool TexturedParticle::Update(float dt)
{
    // Update base class

    bool const reborn = Particle::Update(dt);

    if (reborn)
    {
        m_Radius   = m_InitialRadius;
        m_Rotation = m_InitialRotation;
        dt         = m_Age;
    }

    Appearance const * const pA = m_pEmitter->GetAppearance();

    // Update size and rotation

    m_Radius   += dt * pA->GetRadiusRate();
    m_Rotation += dt * pA->GetRadialVelocity();

    return reborn;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void TexturedParticle::Draw(IDirect3DDevice9 * pD3dDevice) const
{
    // Nothing to do because the particle is drawn by the emitter. This function should not be called.
    assert(false);
}
} // namespace Confetti
