#include "Environment.h"

#include "Dxx/Random.h"
#include "MyMath/Constants.h"
#include "MyMath/FastMath.h"

#include <d3d11.h>
#include <DirectXMath.h>

#include <random>
using namespace DirectX;

namespace Confetti
{
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
    , rng_(std::random_device()())
{
}

//!
//! @param	dt		Amount of time passed since the last update.

void Environment::Update(float dt)
{
    DirectX::XMVECTOR currentWindVelocity_simd(XMLoadFloat3(&windVelocity_));

    // If a gustiness value is specified, then randomly adjust the wind velocity
    DirectX::XMVECTOR gustiness_simd(XMLoadFloat3(&gustiness_));
    if (!XMVector3InBounds(gustiness_simd, DirectX::XMVectorZero()))
    {
        Dxx::RandomDirection gust;
        DirectX::XMVECTOR gustDirection_simd = DirectX::XMLoadFloat3(&gust(rng_));
        currentWindVelocity_simd += gustDirection_simd * gustiness_simd * dt;

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
