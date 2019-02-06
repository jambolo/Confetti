#pragma once

#if !defined(CONFETTI_ENVIRONMENT_H)
#define CONFETTI_ENVIRONMENT_H

#include "Dxx/D3dx.h"
#include <DirectXMath.h>
#include <vector>
#include <random>

namespace Confetti
{
//! An external (to any particle or emitter) set of factors that control a particle's position and velocity.
//!
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
            : plane_(1.0f, 0.0f, 0.0f, 1.0f)
            , dampening_(1.0f)
        {
        }

        //! Constructor.
        BouncePlane(DirectX::XMFLOAT4 const & plane, float dampening)
            : plane_(plane)
            , dampening_(dampening)
        {
        }
        DirectX::XMFLOAT4 plane_;   //!< The plane
        float dampening_;           //!< The ratio of the post-bounce velocity to the pre-bounce velocity.
    };

    //! A plane that clips particles.
    using ClipPlane = DirectX::XMFLOAT4;

    //! A list of BouncePlanes.
    using BouncePlaneList = std::vector<BouncePlane>;

    //! A list of ClipPlanes.
    using ClipPlaneList = std::vector<ClipPlane>;

    //! Constructor.
    explicit Environment(DirectX::XMFLOAT3 const & gravity      = { 0.0f, 0.0f, 0.0f },
                         DirectX::XMFLOAT3 const & windVelocity = { 0.0f, 0.0f, 0.0f },
                         float airFriction = 0.0f,
                         DirectX::XMFLOAT3 const & gustiness = { 0.0f, 0.0f, 0.0f },
                         BouncePlaneList const *   pBPL      = nullptr,
                         ClipPlaneList const *     pCPL      = nullptr);

    //! Destructor.
    virtual ~Environment() = default;

    //! Sets/returns gravity.
    void                      SetGravity(DirectX::XMFLOAT3 const & gravity) { gravity_ = gravity; }
    DirectX::XMFLOAT3 const & GetGravity() const                            { return gravity_; }

    //! Sets/returns wind velocity
    void                      SetWindVelocity(DirectX::XMFLOAT3 const & wind) { windVelocity_ = wind; }
    DirectX::XMFLOAT3 const & GetWindVelocity() const                         { return currentWindVelocity_; }

    //! Sets/returns air friction
    void  SetAirFriction(float airFriction) { airFriction_ = airFriction; }
    float GetAirFriction() const            { return airFriction_; }

    //! Sets/returns gustiness
    void                      SetGustiness(DirectX::XMFLOAT3 const & gustiness) { gustiness_ = gustiness; }
    DirectX::XMFLOAT3 const & GetGustiness() const                              { return gustiness_; }

    //! Returns the terminal velocity
    DirectX::XMFLOAT3 const & GetTerminalVelocity() const { return terminalVelocity_; }

    //! Returns the distance traveled by a particle at terminal velocity during the last update.
    DirectX::XMFLOAT3 const & GetTerminalDistance() const { return terminalDistance_; }

    //! Returns the value 1.0f - exp( -airFriction_ * dt ) calculated during the last update.
    float GetEct1() const { return ect1_; }

    //! Sets/returns the list of bounce plane
    void                    SetBouncePlanes(BouncePlaneList const * pBPL) { bouncePlanes_ = pBPL; }
    BouncePlaneList const * GetBouncePlanes() const                       { return bouncePlanes_; }

    //! Sets/returns the list of clip planes
    void                  SetClipPlanes(ClipPlaneList const * pCPL) { clipPlanes_ = pCPL; }
    ClipPlaneList const * GetClipPlanes() const                     { return clipPlanes_; }

    //! Updates the environment.
    void Update(float dt);

private:

    DirectX::XMFLOAT3 gravity_;             // Gravity.
    DirectX::XMFLOAT3 windVelocity_;        // Constant wind velocity component of the current
                                            // wind velocity.
    DirectX::XMFLOAT3 gustiness_;           // Gustiness factor.
    DirectX::XMFLOAT3 gust_;                // Gust component of the current wind velocity.
    DirectX::XMFLOAT3 currentWindVelocity_; // Current wind velocity.
    DirectX::XMFLOAT3 terminalVelocity_;    // Terminal velocity.
    DirectX::XMFLOAT3 terminalDistance_;    // Movement of a particle traveling at terminal velocity.
    float airFriction_;                     // Air friction factor.
    float ect1_;                            // The value 1.0f - exp( -airFriction_ * dt )
                                            // calculated during the last update.
    BouncePlaneList const * bouncePlanes_;  // A list of planes that the particles bounce against.
    ClipPlaneList const * clipPlanes_;      // A list of planes that clip the particles.

    std::minstd_rand rng_;                // The RNG for environments
};
} // namespace Confetti

#endif // !defined(CONFETTI_ENVIRONMENT_H)
