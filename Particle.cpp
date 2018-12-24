/** @file *//********************************************************************************************************

                                                     Particle.cpp

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Particle.cpp#10 $

    $NoKeywords: $

********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "Particle.h"

#include "Appearance.h"
#include "Emitter.h"
#include "Environment.h"

#include "Math/FastMath.h"
#include "Misc/Max.h"

namespace Confetti
{
/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	pEmitter		The emitter that controls this particle
//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.

Particle::Particle(BasicEmitter const * pEmitter,
                   float                lifetime,
                   float                age,
                   D3DXVECTOR3 const &  position,
                   D3DXVECTOR3 const &  velocity,
                   D3DXCOLOR const &    color)
    : m_pEmitter(pEmitter),
    m_Lifetime(lifetime),
    m_Age(age),
    m_InitialPosition(position),
    m_InitialVelocity(velocity),
    m_Position(position),
    m_Velocity(velocity),
    m_InitialColor(color),
    m_Color(color)
{
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Particle::~Particle()
{
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	lifetime		How long the particle lives.
//! @param	age				Initial age.
//! @param	position		Position at birth.
//! @param	velocity		Velocity at birth.
//! @param	color			Color at birth

void Particle::Initialize(float               lifetime,
                          float               age,
                          D3DXVECTOR3 const & position,
                          D3DXVECTOR3 const & velocity,
                          D3DXCOLOR const &   color)
{
    m_Lifetime        = lifetime;
    m_Age             = age;
    m_InitialPosition = position;
    m_InitialVelocity = velocity;
    m_InitialColor    = color;

    m_Position = position;
    m_Velocity = velocity;
    m_Color    = color;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	dt	The amount of time that has passed since the last update.
//!
//! @note	Methods overriding this method must call this first in order to determine the particle's age and if it
//!			has been reborn or not.

bool Particle::Update(float dt)
{
    bool reborn;

    m_Age += dt;

    // If the particle has not been born yet then do nothing

    if (m_Age < 0.0f)
        return false;

    // See if the particle is (re)born this frame

    if (m_Age < dt)
    {
        reborn = true;
    }
    else if (m_Age >= m_Lifetime)
    {
        m_Age -= m_Lifetime;
        reborn = true;
    }
    else
    {
        reborn = false;
    }

    // If (re)born, then reset to initial values and adjust dt

    if (reborn)
    {
        m_Velocity = m_pEmitter->GetCurrentVelocity() + m_InitialVelocity;
        m_Position = m_pEmitter->GetCurrentPosition() + m_InitialPosition;
        m_Color    = m_InitialColor;
        dt         = m_Age;
    }

    Environment const * const pE = m_pEmitter->GetEnvironment();
    Appearance const * const  pA = m_pEmitter->GetAppearance();

    // Update velocity and position

    D3DXVECTOR3         dv;
    D3DXVECTOR3         ds;
    float const         c = pE->GetAirFriction();
    D3DXVECTOR3 const & g = pE->GetGravity();

    if (c != 0.0f)
    {
        D3DXVECTOR3 const & terminalVelocity = pE->GetTerminalVelocity();
        D3DXVECTOR3 const & terminalDistance = pE->GetTerminalDistance();
        float const         ect1 = pE->GetEct1();

        dv = (terminalVelocity - m_Velocity) * ect1;
        ds = terminalDistance - dv / c;
    }
    else
    {
        dv = g * dt;
        ds = (m_Velocity + dv * 0.5f) * dt;
    }

    m_Velocity += dv;
    m_Position += ds;

    // Check for collision with clip planes

    Environment::ClipPlaneList const * pClipPlanes = pE->GetClipPlanes();

    if (pClipPlanes != 0)
    {
        for (Environment::ClipPlaneList::const_iterator pC = pClipPlanes->begin(); pC != pClipPlanes->end(); ++pC)
        {
            D3DXPLANE const & plane = *pC;

            if (D3DXPlaneDotCoord(&plane, &m_Position) < 0.0f)
            {
                m_Age -= m_Lifetime;
                return reborn;
            }
        }
    }

    // Check for collision with bounce planes

    Environment::BouncePlaneList const * pBouncePlanes = pE->GetBouncePlanes();

    if (pBouncePlanes != 0)
    {
        for (Environment::BouncePlaneList::const_iterator pB = pBouncePlanes->begin(); pB != pBouncePlanes->end(); ++pB)
        {
            Environment::BouncePlane const & plane = *pB;

            if (D3DXPlaneDotCoord(&plane.m_Plane, &m_Position) < 0.0f)
            {
                float const f = 1.0f + plane.m_Dampening;
                m_Velocity -=  D3DXVECTOR3(plane.m_Plane) * (f * D3DXPlaneDotNormal(&plane.m_Plane, &m_Velocity));
                m_Position -=  D3DXVECTOR3(plane.m_Plane) * (f * D3DXPlaneDotCoord(&plane.m_Plane, &m_Position));
            }
        }
    }

    // Update color

    m_Color  += pA->GetColorRate() * dt;
    m_Color.r = limit(0.0f, m_Color.r, 1.0f);
    m_Color.g = limit(0.0f, m_Color.g, 1.0f);
    m_Color.b = limit(0.0f, m_Color.b, 1.0f);
    m_Color.a = limit(0.0f, m_Color.a, 1.0f);

    return reborn;
}

/// ********************************************************************************************************************/
/// *																													*/
/// ********************************************************************************************************************/
//
// D3DXCOLOR Particle::GetColor() const
// {
//	float const	r	= limit( 0., m_InitialColor.m_R + m_ColorDelta.m_R * m_Age, 1. );
//	float const	g	= limit( 0., m_InitialColor.m_G + m_ColorDelta.m_G * m_Age, 1. );
//	float const	b	= limit( 0., m_InitialColor.m_B + m_ColorDelta.m_B * m_Age, 1. );
//	float const	a	= limit( 0., m_InitialColor.m_A + m_ColorDelta.m_A * m_Age, 1. );
//
//	return D3DXCOLOR( r, g, b, a );
// }
//
//
/// ********************************************************************************************************************/
/// *																													*/
/// ********************************************************************************************************************/
//
// D3DXVECTOR3 Particle::GetPosition() const
// {
//	Environment const &	e	= *m_pEmitter->GetEnvironment();
//
//	if ( Math::IsCloseToZero( e.GetAirFriction() ) )
//	{
//		D3DXVECTOR3	position	= e.GetGravity();
//
//		position *= ( 0.5f * m_Age );
//		position += m_AbsoluteInitialVelocity;
//		position *= m_Age;
//		position += m_AbsoluteInitialPosition;
//
//		return position;
//
////		return m_AbsoluteInitialPosition
////			   + ( m_AbsoluteInitialVelocity
////				   + e.GetGravity() * ( 0.5f * m_Age )
////				 ) * m_Age;
//	}
//	else
//	{
//		// How this was derived:
//		// Acceleration due to air friction = -c * ( v - w ), so
//		//
//		//		ds2/dt2 = dv/dt = g - c * ( v - w )
//		//
//		// also note that "terminal velocity" vT = g/c + w:
//		//
//		//		ds2/dt2 = dv/dt = c * vT - cv
//		//
//		//		ds/dt = v = vT - e**( -c * t ) * e**( -c * D ) / c
//		//
//		// Assuming v = v0 at t = 0 ==> e**( -c * D ) / c = ( vT - v0 ):
//		//
//		//		ds/dt = v = vT - ( vT - v0 ) * e**( -c * t )
//		//
//		//		s = vT * t + ( vT - v0 ) / c * e**( -c * t ) + E
//		//
//		// Assuming s = s0 at t = 0 ==> E = s0 - ( vT - v0 ) / c:
//		//
//		//		s = s0 + vT * t - ( vT - v0 ) * ( 1 - e**( -c * t ) ) / c
//		//
//		// c <== e.GetAirFriction(), w <== e.GetWindVelocity(), t <== m_Age,
//		// g <== e.GetGravity(), v0 <== m_AbsoluteInitialVelocity,
//		// s0 <== m_AbsoluteInitialPosition
//		//
//
//		double const	ect1c	= ( 1. - exp( -e.GetAirFriction() * m_Age ) ) / e.GetAirFriction();
//
//		return m_AbsoluteInitialPosition
//			   + m_TerminalVelocity * m_Age
//			   - ( m_TerminalVelocity - m_AbsoluteInitialVelocity ) * ect1c;
//
//	}
// }
//
/// ********************************************************************************************************************/
/// *																													*/
/// ********************************************************************************************************************/
//
// D3DXVECTOR3 Particle::GetVelocity() const
// {
//	Environment const &	e	= *m_pEmitter->GetEnvironment();
//
//	if ( Math::IsCloseToZero( e.GetAirFriction() ) )
//	{
//		return m_AbsoluteInitialVelocity + e.GetGravity() * m_Age;
//	}
//	else
//	{
//		// How this was derived:
//		// Acceleration due to air friction = -c * ( v - w ), so
//		//
//		//		dv/dt = g - c * ( v - w )
//		//
//		// also note that "terminal velocity" vT = g/c + w:
//		//
//		//		dv/dt = c * vT - cv
//		//
//		//		v = vT - e**( -c * t ) * e**( -c * D ) / c
//		//
//		// Assuming v = v0 at t = 0, e**( -c * D ) / c = ( vT - v0 ):
//		//
//		//		v = vT - ( vT - v0 ) * e**( -c * t )
//		//
//		// c <== e.GetAirFriction(), w <== e.GetWindVelocity(), t <== m_Age,
//		// g <== e.GetGravity(), v0 <== m_AbsoluteInitialVelocity,
//
//		float const		ect		= exp( -e.GetAirFriction() * m_Age );
//
//		return m_TerminalVelocity - ( m_TerminalVelocity - m_AbsoluteInitialVelocity ) * ect;
//	}
// }
} // namespace Confetti
