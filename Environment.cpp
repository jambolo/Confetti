#include "Environment.h"

#include "Misc/Random.h"
#include "MyMath/Constants.h"
#include "MyMath/FastMath.h"

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

namespace Confetti
{
RandomFloat Environment::rng_(-1);

Environment::Environment(DirectX::XMFLOAT3 const & gravity /* = { 0.0f, 0.0f, 0.0f }*/,
                         DirectX::XMFLOAT3 const & windVelocity /* = { 0.0f, 0.0f, 0.0f }*/,
                         float                     airFriction /* = 0.0f*/,
                         DirectX::XMFLOAT3 const & gustiness /* = { 0.0f, 0.0f, 0.0f }*/,
                         BouncePlaneList const *   pBPL /* = nullptr*/,
                         ClipPlaneList const *     pCPL /* = nullptr*/)
    : gravity_(gravity)
    , windVelocity_(windVelocity)
    , airFriction_(airFriction)
    , gustiness_(gustiness)
    , bouncePlanes_(pBPL)
    , clipPlanes_(pCPL)
    , gust_({ 0.0f, 0.0f, 0.0f })
    , currentWindVelocity_(windVelocity)
{
    rng_.setState((uint32_t)reinterpret_cast<uintptr_t>(this));   // Reinitialize the RNG to a random starting point
}

//!
//! @param	dt		Amount of time passed since the last update.

void Environment::Update(float dt)
{
    DirectX::XMVECTOR currentWindVelocity_simd(XMLoadFloat3(&windVelocity_);

    // If a gustiness value is specified, then randomly adjust the wind velocity
    DirectX::XMVECTOR gustiness_simd(XMLoadFloat3(&gustiness_));
    if (!XMVector3InBounds(gustiness_simd, DirectX::XMVectorZero())
    {
        gust_.x += rng_(DirectX::XM_2PI) * dt;
        gust_.y += rng_(DirectX::XM_2PI) * dt;

        float cx, sx, cy, sy;
        MyMath::fsincos(gust_.x, &sx, &cx);
        MyMath::fsincos(gust_.y, &sy, &cy);
        currentWindVelocity_simd += gustiness_simd * FXMVECTOR(cx * sy, cy, sx * sy);

        DirectX::XMStoreFloat3(&currentWindVelocity_, currentWindVelocity_simd);
    }

    // If an air friction value is specified, then compute the terminal velocity and some intermediate values.
    if (airFriction_ != 0.0f)
    {
        DirectX::XMVECTOR gravity_simd(XMLoadFloat3(&gravity_));
        DirectX::XMVECTOR terminalVelocity_simd;
        DirectX::XMVECTOR terminalDistance_simd;
        terminalVelocity_simd = currentWindVelocity_simd + gravity_simd / airFriction_;
        terminalDistance_simd = terminalVelocity_simd * dt;
        DirectX::XMStoreFloat3(&terminalVelocity_, terminalVelocity_simd);
        DirectX::XMStoreFloat3(&terminalDistance_, terminalDistance_simd);
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
