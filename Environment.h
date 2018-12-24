/** @file *//********************************************************************************************************

                                                     Environment.h

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Environment.h#9 $

    $NoKeywords: $

********************************************************************************************************************/

#pragma once

#include "Dxx/d3d.h"
#include "Misc/Random.h"
#include <d3dx9math.h>
#include <vector>

namespace Confetti
{
/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! An external (to any particle or emitter) set of factors that control a particle's position and velocity.
//
//! @ingroup	Controls
//!

class Environment
{
public:

    //! A plane that the particles bounce against.
    class BouncePlane
    {
public:

        BouncePlane()
            : m_Plane(1.0f, 0.0f, 0.0f, 1.0f), m_Dampening(1.0f)
        {
        }

        //! Constructor
        BouncePlane(D3DXPLANE const & plane, float dampening)
            : m_Plane(plane), m_Dampening(dampening)
        {
        }
        D3DXPLANE m_Plane;          //!< The plane
        float m_Dampening;          //!< The ratio of the post-bounce velocity to the pre-bounce velocity.
    };

    //! A plane that clips particles.
    typedef D3DXPLANE ClipPlane;

    //! A list of BouncePlanes.
    typedef std::vector<BouncePlane>  BouncePlaneList;

    //! A list of ClipPlanes.
    typedef std::vector<ClipPlane>    ClipPlaneList;

    //! Constructor
    Environment(D3DXVECTOR3 const &     gravity      = Dxx::Vector3Origin(),
                D3DXVECTOR3 const &     windVelocity = Dxx::Vector3Origin(),
                float                   airFriction = 0.0f,
                D3DXVECTOR3 const &     gustiness = Dxx::Vector3Origin(),
                BouncePlaneList const * pBPL      = 0,
                ClipPlaneList const *   pCPL      = 0);

    // Destructor
    virtual ~Environment();

    //! Sets/returns gravity.
    void                SetGravity(D3DXVECTOR3 const & gravity) { m_Gravity = gravity; }
    D3DXVECTOR3 const & GetGravity() const                      { return m_Gravity; }

    //! Sets/returns wind velocity
    void                SetWindVelocity(D3DXVECTOR3 const & wind) { m_WindVelocity = wind; }
    D3DXVECTOR3 const & GetWindVelocity() const                   { return m_CurrentWindVelocity; }

    //! Sets/returns air friction
    void  SetAirFriction(float airFriction) { m_AirFriction = airFriction; }
    float GetAirFriction() const            { return m_AirFriction; }

    //! Sets/returns gustiness
    void                SetGustiness(D3DXVECTOR3 const & gustiness) { m_Gustiness = gustiness; }
    D3DXVECTOR3 const & GetGustiness() const                        { return m_Gustiness; }

    //! Returns the terminal velocity
    D3DXVECTOR3 const & GetTerminalVelocity() const { return m_TerminalVelocity; }

    //! Returns the distance traveled by a particle at terminal velocity during the last update.
    D3DXVECTOR3 const & GetTerminalDistance() const { return m_TerminalDistance; }

    //! Returns the value 1.0f - exp( -m_AirFriction * dt ) calculated during the last update.
    float GetEct1() const { return m_Ect1; }

    //! Sets/returns the list of bounce plane
    void                    SetBouncePlanes(BouncePlaneList const * pBPL) { m_BouncePlanes = pBPL; }
    BouncePlaneList const * GetBouncePlanes() const                       { return m_BouncePlanes; }

    //! Sets/returns the list of clip planes
    void                  SetClipPlanes(ClipPlaneList const * pCPL) { m_ClipPlanes = pCPL; }
    ClipPlaneList const * GetClipPlanes() const                     { return m_ClipPlanes; }

    //! Updates the environment.
    void Update(float dt);

private:

    D3DXVECTOR3 m_Gravity;                          // Gravity.
    D3DXVECTOR3 m_WindVelocity;                     // Constant wind velocity component of the current
                                                    // wind velocity.
    D3DXVECTOR3 m_Gustiness;                        // Gustiness factor.
    D3DXVECTOR3 m_Gust;                             // Gust component of the current wind velocity.
    D3DXVECTOR3 m_CurrentWindVelocity;              // Current wind velocity.
    D3DXVECTOR3 m_TerminalVelocity;                 // Terminal velocity.
    D3DXVECTOR3 m_TerminalDistance;                 // Movement of a particle traveling at terminal velocity.
    float m_AirFriction;                            // Air friction factor.
    float m_Ect1;                                   // The value 1.0f - exp( -m_AirFriction * dt )
                                                    // calculated during the last update.
    BouncePlaneList const * m_BouncePlanes;         // A list of planes that the particles bounce against.
    ClipPlaneList const * m_ClipPlanes;             // A list of planes that clip the particles.

    static RandomFloat m_Rng;                       // The RNG for environments
};
} // namespace Confetti
