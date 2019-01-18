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

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

namespace Confetti
{
RandomFloat Environment::rng_(-1);

Environment::Environment(DirectX::XMFLOAT3 const & gravity /* = DirectX::XMVectorZero()*/,
                         DirectX::XMFLOAT3 const & windVelocity /* = DirectX::XMVectorZero()*/,
                         float                     airFriction /* = 0.0f*/,
                         DirectX::XMFLOAT3 const & gustiness /* = DirectX::XMVectorZero()*/,
                         BouncePlaneList const *   pBPL /* = 0*/,
                         ClipPlaneList const *     pCPL /* = 0*/)
    : gravity_(gravity)
    , windVelocity_(windVelocity)
    , airFriction_(airFriction)
    , gustiness_(gustiness)
    , bouncePlanes_(pBPL)
    , clipPlanes_(pCPL)
    , gust_(DirectX::XMVectorZero())
    , currentWindVelocity_(windVelocity)
{
    rng_.SetState((uint32_t)this);   // Reinitialize the RNG to a random starting point
}

//!
//! @param	dt		Amount of time passed since the last update.

void Environment::Update(float dt)
{
    currentWindVelocity_ = windVelocity_;

    // If a gustiness value is specified, then randomly adjust the wind velocity

    if (XMVector3LengthSq(&gustiness_) != 0.0f)
    {
        gust_.x += rng_.Get(float(Math::TWO_PI)) * dt;
        gust_.y += rng_.Get(float(Math::TWO_PI)) * dt;

        float cx, sx, cy, sy;

        Math::fsincos(gust_.x, &sx, &cx);
        Math::fsincos(gust_.y, &sy, &cy);

        currentWindVelocity_ += DirectX::XMFLOAT3(cx * sy * gustiness_.x,
                                                  cy      * gustiness_.y,
                                                  sx * sy * gustiness_.z);
    }

    // If an air friction value is specified, then compute the terminal velocity and some intermediate values.

    if (airFriction_ != 0.0f)
    {
        terminalVelocity_ = currentWindVelocity_ + gravity_ / airFriction_;
        terminalDistance_ = terminalVelocity_ * dt;
        ect1_ = 1.0f - expf(-airFriction_ * dt);
    }
    else
    {
        terminalVelocity_ = DirectX::XMVectorZero();
        terminalDistance_ = DirectX::XMVectorZero();
        ect1_ = 0.0f;
    }
}
} // namespace Confetti
