#include "EmitterVolume.h"

#include "MyMath/FastMath.h"

namespace Confetti
{
//!
//! @param	seed	Initial seed

EmitterVolume::EmitterVolume(unsigned int seed)
    : rng_(seed)
{
}

//!
//! @param	seed	Initial seed (not used)

EmitterPoint::EmitterPoint(unsigned int seed)
    : EmitterVolume(seed)
{
}

//! @param	seed	Initial seed.
//! @param	size	Length of the line segment.

EmitterLine::EmitterLine(unsigned int seed, float size)
    : EmitterVolume(seed)
    , size_(size)
{
}

DirectX::XMFLOAT3 EmitterLine::next() const
{
    float x = rng_(size_) - size_ * 0.5f;

    return DirectX::XMFLOAT3(x, 0.0f, 0.0f);
}

//! @param	seed	Initial seed.
//! @param	w	    Size of the rectangle along the X axis.
//! @param	h       Size of the rectangle along the Z axis.

EmitterRectangle::EmitterRectangle(unsigned int seed, float w, float h)
    : EmitterVolume(seed)
    , width_(w)
    , height_(h)
{
}

DirectX::XMFLOAT3 EmitterRectangle::next() const
{
    float x = width_  * (rng_(1.0f) - 0.5f);
    float z = height_ * (rng_(1.0f) - 0.5f);

    return DirectX::XMFLOAT3(x, 0.0f, z);
}

//! @param	seed	Initial seed.
//! @param	radius	Radius of the circle.

EmitterCircle::EmitterCircle(unsigned int seed, float radius)
    : EmitterVolume(seed)
    , radius_(radius)
{
}

DirectX::XMFLOAT3 EmitterCircle::next() const
{
    // Source: http://mathworld.wolfram.com/DiskPointPicking.html

    float a = rng_(float(DirectX::XM_2PI));
    float r = radius_ * sqrtf(rng_());
    float       c, s;

    MyMath::fsincos(a, &s, &c);

    return DirectX::XMFLOAT3(c * r, s * r, 0.0f);
}

//! @param	seed	Initial seed.
//! @param	radius	radius of the sphere.

EmitterSphere::EmitterSphere(unsigned int seed, float radius)
    : EmitterVolume(seed)
    , radius_(radius)
{
}

DirectX::XMFLOAT3 EmitterSphere::next() const
{
    // Source: http://mathworld.wolfram.com/SpherePointPicking.html
    //
    // u = R(-1,1)
    // v = R(-1,1)
    // t = a * u
    // f = acos( b / (PI/2) * v )
    // v = [ cos(f) * cos(t), sin(f), cos(f) * sin(t) ]
    // v = [ cos(f) * cos(t), sqrt(1.-cos(f)*cos(f)), cos(f) * sin(t) ]
    // ct = cos(t)
    // st = sin(t)
    // cf = cos(f) = b / (PI/2) * v
    // sf = sin(f) = sqrt( 1. - cf*cf )
    // v = [ cf * ct, sf, cf * st ]

    float t = rng_(float(-DirectX::XM_PI), float(DirectX::XM_PI));
    float cf = rng_(-1, 1);
    float sf = sqrtf(1.0f - cf * cf);
    float r = radius_ * powf(rng_(), 1.0f / 3.0f);
    float st, ct;

    MyMath::fsincos(t, &st, &ct);

    return DirectX::XMFLOAT3(cf * ct * r, sf * r, cf * st * r);
}

//! @param	seed	Initial seed.
//! @param	size	Width, height, and depth of the box.

EmitterBox::EmitterBox(unsigned int seed, DirectX::XMFLOAT3 const & size)
    : EmitterVolume(seed)
    , size_(size)
{
}

DirectX::XMFLOAT3 EmitterBox::next() const
{
    float x = rng_(size_.x) - size_.x * 0.5f;
    float y = rng_(size_.y) - size_.y * 0.5f;
    float z = rng_(size_.z) - size_.z * 0.5f;

    return DirectX::XMFLOAT3(x, y, z);
}

//! @param	seed	Initial seed.
//! @param	radius	Radius of the cylinder.
//! @param	height	Height of the cylinder.

EmitterCylinder::EmitterCylinder(unsigned int seed, float radius, float height)
    : EmitterVolume(seed)
    , radius_(radius)
    , height_(height)
{
}

DirectX::XMFLOAT3 EmitterCylinder::next() const
{
    float a = rng_(float(DirectX::XM_2PI));
    float h = rng_(height_);
    float r = radius_ * sqrtf(rng_());
    float       c, s;

    MyMath::fsincos(a, &s, &c);

    return DirectX::XMFLOAT3(c * r, s * r, h);
}

//! @param	seed	Initial seed.
//! @param	radius	Radius of the cone at the base.
//! @param	height	Height of the cone.

EmitterCone::EmitterCone(unsigned int seed, float radius, float height)
    : EmitterVolume(seed)
    , radius_(radius)
    , height_(height)
{
}

DirectX::XMFLOAT3 EmitterCone::next() const
{
    float a = rng_(float(DirectX::XM_2PI));
    float       h = rng_();
    float       r = rng_();
    float       c, s;

    MyMath::fsincos(a, &s, &c);

    h = powf(h, 1.0f / 3.0f) * height_;
    r = sqrtf(r) / radius_ * h;

    return DirectX::XMFLOAT3(c * r, s * r, h);
}
} // namespace Confetti
