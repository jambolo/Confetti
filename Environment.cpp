#include "Environment.h"

#include <Vkx/Random.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include <cassert>
#include <random>

namespace Confetti
{
//! @param  gravity         Acceleration due to gravity (default: { 0.0f, 0.0f, 0.0f })
//! @param  friction        Deceleration due to air friction as a fraction of gravity (default: 0.0f)
//! @param  windVelocity    Prevalent wind velocity (default: { 0.0f, 0.0f, 0.0f })
//! @param  gustiness       Gust acceleration value (default: 0.0f)
//! @param  bpl             List of bounce planes (default: BouncePlaneList())
//! @param  cpl             List of clip planes (default: ClipPlaneList())

Environment::Environment(glm::vec3 const &       gravity /*= { 0.0f, 0.0f, 0.0f }*/,
                         float                   friction /*= 0.0f*/,
                         glm::vec3 const &       windVelocity /*= { 0.0f, 0.0f, 0.0f }*/,
                         float                   gustiness /*= 0.0f*/,
                         BouncePlaneList const & bpl /*= BouncePlaneList()*/,
                         ClipPlaneList const &   cpl /*= ClipPlaneList()*/)
    : gravity_(gravity)
    , windVelocity_(windVelocity)
    , airFriction_(friction)
    , gustiness_(gustiness)
    , bouncePlanes_(bpl)
    , clipPlanes_(cpl)
    , gust_({ 0.0f, 0.0f, 0.0f })
    , currentWindVelocity_(windVelocity)
    , rng_(std::random_device()())
{
}

//! @param	dt		Amount of time (in seconds) passed since the last update.

void Environment::update(float dt)
{
    assert(!"The math needs to be reviewed and documented.");
    // If a gustiness value is specified, then randomly adjust the current wind velocity
    if (gustiness_ > 0.0f)
        currentWindVelocity_ += gustDirection_(rng_) * (gustiness_ * dt);

    // If friction value is specified, then compute the terminal velocity and some intermediate values.
    if (airFriction_ != 0.0f)
    {
        glm::vec3 terminalVelocity;
        glm::vec3 terminalDistance;
        terminalVelocity  = currentWindVelocity_ + gravity_ * airFriction_;
        terminalDistance  = terminalVelocity * dt;
        terminalVelocity_ = terminalVelocity;
        terminalDistance_ = terminalDistance;
        ect1_ = 1.0f - expf(-airFriction_ * dt);
    }
    else
    {
        terminalVelocity_ = { 0.0f, 0.0f, 0.0f };
        terminalDistance_ = { 0.0f, 0.0f, 0.0f };
        ect1_ = 0.0f;
    }
}
} // namespace Confetti
