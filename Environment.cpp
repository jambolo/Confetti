/** @file *//********************************************************************************************************

                                                   Environment.cpp

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Environment.cpp#10 $

    $NoKeywords: $

********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "Environment.h"

#include "Math/Constants.h"
#include "Math/FastMath.h"
#include "Misc/Random.h"

namespace Confetti
{
/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

RandomFloat Environment::m_Rng(-1);

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Environment::Environment(D3DXVECTOR3 const &     gravity /*			= Dxx::Vector3Origin()*/,
                         D3DXVECTOR3 const &     windVelocity /*	= Dxx::Vector3Origin()*/,
                         float                   airFriction /*					= 0.0f*/,
                         D3DXVECTOR3 const &     gustiness /*		= Dxx::Vector3Origin()*/,
                         BouncePlaneList const * pBPL /*		= 0*/,
                         ClipPlaneList const *   pCPL /*			= 0*/)
    : m_Gravity(gravity),
    m_WindVelocity(windVelocity),
    m_AirFriction(airFriction),
    m_Gustiness(gustiness),
    m_BouncePlanes(pBPL),
    m_ClipPlanes(pCPL),
    m_Gust(Dxx::Vector3Origin()),
    m_CurrentWindVelocity(windVelocity)
{
    m_Rng.SetState((uint32)this);   // Reinintialize the RNG to a random starting point
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Environment::~Environment()
{
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//!
//! @param	dt		Amount of time passed since the last update.

void Environment::Update(float dt)
{
    m_CurrentWindVelocity = m_WindVelocity;

    // If a gustiness value is specified, then randomly adjust the wind velocity

    if (D3DXVec3LengthSq(&m_Gustiness) != 0.0f)
    {
        m_Gust.x += m_Rng.Get(float(Math::TWO_PI)) * dt;
        m_Gust.y += m_Rng.Get(float(Math::TWO_PI)) * dt;

        float cx, sx, cy, sy;

        Math::fsincos(m_Gust.x, &sx, &cx);
        Math::fsincos(m_Gust.y, &sy, &cy);

        m_CurrentWindVelocity += D3DXVECTOR3(cx * sy * m_Gustiness.x,
                                             cy      * m_Gustiness.y,
                                             sx * sy * m_Gustiness.z);
    }

    // If an air friction value is specified, then compute the terminal velocity and some intermediate values.

    if (m_AirFriction != 0.0f)
    {
        m_TerminalVelocity = m_CurrentWindVelocity + m_Gravity / m_AirFriction;
        m_TerminalDistance = m_TerminalVelocity * dt;
        m_Ect1 = 1.0f - expf(-m_AirFriction * dt);
    }
    else
    {
        m_TerminalVelocity = Dxx::Vector3Origin();
        m_TerminalDistance = Dxx::Vector3Origin();
        m_Ect1 = 0.0f;
    }
}
} // namespace Confetti
