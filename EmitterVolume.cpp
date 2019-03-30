#include "EmitterVolume.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <random>
#include<glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Confetti
{
//! @param  rng     Random number generator
//! @param	size	Length of the line segment.

EmitterLine::EmitterLine(float size)
    : randomX(-0.5f * size, 0.5f * size)
{
}

glm::vec3 EmitterLine::operator ()(std::minstd_rand & rng) const
{
    return glm::vec3(randomX(rng), 0.0f, 0.0f);
}

//! @param	w	    Size of the rectangle along the X axis.
//! @param	h       Size of the rectangle along the Z axis.

EmitterRectangle::EmitterRectangle(float w, float h)
    : randomX_(-0.5f * w, 0.5f * w)
    , randomZ_(-0.5f * h, 0.5f * h)

{
}

glm::vec3 EmitterRectangle::operator ()(std::minstd_rand & rng) const
{
    return glm::vec3(randomX_(rng), 0.0f, randomZ_(rng));
}

//! @param	radius	Radius of the circle.

EmitterCircle::EmitterCircle(float radius)
    : randomAngle_(0.0f, glm::two_pi<float>())
    , randomR_(0.0f, radius * radius)
{
}

glm::vec3 EmitterCircle::operator ()(std::minstd_rand & rng) const
{
    // Source: http://mathworld.wolfram.com/DiskPointPicking.html

    float a = randomAngle_(rng);
    float c = cos(a);
    float s = sqrt(1.0f - c * c);
    float r = sqrt(randomR_(rng));
    return glm::vec3(c * r, s * r, 0.0f);
}

//! @param	rng     Random number generator.
//! @param	radius	radius of the sphere.

EmitterSphere::EmitterSphere(float radius)
    : randomR_(0.0f, radius * radius * radius)
    , randomAngle_(0.0f, glm::two_pi<float>())
    , randomCos_(-1.0f, 1.0f)

{
}

glm::vec3 EmitterSphere::operator ()(std::minstd_rand & rng) const
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

    float t  = randomAngle_(rng);
    float ct = cos(t);
    float st = sqrt(1.0f - ct * ct);
    float cf = randomCos_(rng);
    float sf = sqrt(1.0f - cf * cf);
    float r  = pow(randomR_(rng), 1.0f / 3.0f);

    return glm::vec3(cf * ct * r, sf * r, cf * st * r);
}

//! @param	rng     Random number generator.
//! @param	size	Width, height, and depth of the box.

EmitterBox::EmitterBox(glm::vec3 const & size)
    : randomX_(-0.5f * size.x, 0.5f * size.x)
    , randomY_(-0.5f * size.y, 0.5f * size.y)
    , randomZ_(-0.5f * size.z, 0.5f * size.z)

{
}

glm::vec3 EmitterBox::operator ()(std::minstd_rand & rng) const
{
    return glm::vec3(randomX_(rng), randomY_(rng), randomZ_(rng));
}

//! @param	rng     Random number generator.
//! @param	radius	Radius of the cylinder.
//! @param	height	Height of the cylinder.

EmitterCylinder::EmitterCylinder(float radius, float height)
    : radius_(radius)
    , randomAngle_(0.0f, glm::two_pi<float>())
    , randomR_(0.0f, radius_ * radius_)
    , randomZ_(-0.5f * height, 0.5f * height)

{
}

glm::vec3 EmitterCylinder::operator ()(std::minstd_rand & rng) const
{
    float a = randomAngle_(rng);
    float c = cos(a);
    float s = sqrt(1.0f - c * c);
    float z = randomZ_(rng);
    float r = sqrt(randomR_(rng));

    return glm::vec3(c * r, s * r, z);
}

//! @param	rng     Random number generator.
//! @param	radius	Radius of the cone at the base.
//! @param	height	Height of the cone.

EmitterCone::EmitterCone(float radius, float height)
    : randomAngle_(0.0f, glm::two_pi<float>())
    , randomZ_(0.0f, height * height * height)
    , randomR_(0.0f, 1.0f / radius / radius)

{
}

glm::vec3 EmitterCone::operator ()(std::minstd_rand & rng) const
{
    float a = randomAngle_(rng);
    float c = cos(a);
    float s = sqrt(1.0f - c * c);
    float z = pow(randomZ_(rng), 1.0f / 3.0f);
    float r = sqrt(randomR_(rng)) * z;

    return glm::vec3(c * r, s * r, z);
}
} // namespace Confetti
