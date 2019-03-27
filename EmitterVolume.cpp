#include "EmitterVolume.h"

#include <random>

namespace Confetti
{
//!
//! @param	rng     Random number generator

EmitterVolume::EmitterVolume(std::minstd_rand & rng)
    : rng_(rng)
{
}

//!
//! @param	rng     Random number generator (not used)

EmitterPoint::EmitterPoint(std::minstd_rand & rng)
    : EmitterVolume(rng)
{
}

//! @param  rng     Random number generator
//! @param	size	Length of the line segment.

EmitterLine::EmitterLine(std::minstd_rand & rng, float size)
    : EmitterVolume(rng)
    , size_(size)
{
}

glm::vec3 EmitterLine::next() const
{
    std::uniform_real_distribution<float> endpoint(-0.5f, 0.5f);
    float x = size_ * endpoint(rng_);
    return glm::vec3(x, 0.0f, 0.0f);
}

//! @param	rng     Random number generator.
//! @param	w	    Size of the rectangle along the X axis.
//! @param	h       Size of the rectangle along the Z axis.

EmitterRectangle::EmitterRectangle(std::minstd_rand & rng, float w, float h)
    : EmitterVolume(rng)
    , width_(w)
    , height_(h)
{
}

glm::vec3 EmitterRectangle::next() const
{
    std::uniform_real_distribution<float> endpoint(-0.5f, 0.5f);
    float x = width_  * endpoint(rng_);
    float z = height_ * endpoint(rng_);
    return glm::vec3(endpoint(rng_), 0.0f, z);
}

//! @param	rng     Random number generator.
//! @param	radius	Radius of the circle.

EmitterCircle::EmitterCircle(std::minstd_rand & rng, float radius)
    : EmitterVolume(rng)
    , radius_(radius)
{
}

glm::vec3 EmitterCircle::next() const
{
    // Source: http://mathworld.wolfram.com/DiskPointPicking.html

    std::uniform_real_distribution<float> randomFloat(0.0f, 1.0f);

    float a = randomFloat(rng_) * glm::two_pi<float>();
    float r = radius_ * sqrt(randomFloat(rng_));
    float c, s;
    glm::fsincos(a, &s, &c);
    return glm::vec3(c * r, s * r, 0.0f);
}

//! @param	rng     Random number generator.
//! @param	radius	radius of the sphere.

EmitterSphere::EmitterSphere(std::minstd_rand & rng, float radius)
    : EmitterVolume(rng)
    , radius_(radius)
{
}

glm::vec3 EmitterSphere::next() const
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

    std::uniform_real_distribution<float> randomFloat(-1.0f, 1.0f);
    float t = randomFloat(rng_) * glm::pi();
    float cf = randomFloat(rng_);
    float sf = sqrt(1.0f - cf * cf);
    float r = radius_ * pow((randomFloat(rng_) + 1.0f) * 0.5f, 1.0f / 3.0f);
    float st, ct;

    glm::fsincos(t, &st, &ct);

    return glm::vec3(cf * ct * r, sf * r, cf * st * r);
}

//! @param	rng     Random number generator.
//! @param	size	Width, height, and depth of the box.

EmitterBox::EmitterBox(std::minstd_rand & rng, glm::vec3 const & size)
    : EmitterVolume(rng)
    , size_(size)
{
}

glm::vec3 EmitterBox::next() const
{
    std::uniform_real_distribution<float> randomFloat(-0.5f, 0.5f);
    float x = randomFloat(rng_) * size_.x;
    float y = randomFloat(rng_) * size_.y;
    float z = randomFloat(rng_) * size_.z;

    return glm::vec3(x, y, z);
}

//! @param	rng     Random number generator.
//! @param	radius	Radius of the cylinder.
//! @param	height	Height of the cylinder.

EmitterCylinder::EmitterCylinder(std::minstd_rand & rng, float radius, float height)
    : EmitterVolume(rng)
    , radius_(radius)
    , height_(height)
{
}

glm::vec3 EmitterCylinder::next() const
{
    std::uniform_real_distribution<float> randomFloat(0.0f, 1.0f);
    float a = randomFloat(rng_) * glm::two_pi<float>();
    float h = randomFloat(rng_) * height_;
    float r = radius_ * sqrt(randomFloat(rng_));
    float c, s;
    glm::fsincos(a, &s, &c);

    return glm::vec3(c * r, s * r, h);
}

//! @param	rng     Random number generator.
//! @param	radius	Radius of the cone at the base.
//! @param	height	Height of the cone.

EmitterCone::EmitterCone(std::minstd_rand & rng, float radius, float height)
    : EmitterVolume(rng)
    , radius_(radius)
    , height_(height)
{
}

glm::vec3 EmitterCone::next() const
{
    std::uniform_real_distribution<float> randomFloat(0.0f, 1.0f);
    float a = randomFloat(rng_) * glm::two_pi<float>();
    float h = randomFloat(rng_);
    float r = randomFloat(rng_);
    float c, s;
    glm::fsincos(a, &s, &c);

    h = pow(h, 1.0f / 3.0f) * height_;
    r = sqrt(r) / radius_ * h;

    return glm::vec3(c * r, s * r, h);
}
} // namespace Confetti
