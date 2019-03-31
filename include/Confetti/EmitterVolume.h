#if !defined(CONFETTI_EMITTERVOLUME_H)
#define CONFETTI_EMITTERVOLUME_H

#pragma once

#include <glm/glm.hpp>
#include <random>

namespace Confetti
{
//! This class generates random points in 3D whose locations are uniformly distributed in a specific volume.
//!
//! @note	This is an abstract base class, so it must be derived from to be used.
//!

class EmitterVolume
{
public:
    //! Destructor.
    virtual ~EmitterVolume() = default;

    //! Returns a value used to specify a particle's point of emission.
    //!
    //! @param	rng     Random number generator.
    //!
    //! @note	This method must be overridden.
    virtual glm::vec3 operator ()(std::minstd_rand & rng) const = 0;
};

//! An EmitterVolume that emits particles from the point <tt>[0,0,0]</tt>.

class EmitterPoint : public EmitterVolume
{
public:
    //! Destructor.
    virtual ~EmitterPoint() override = default;

    //! @name Overrides EmitterVolume
    //@{
    glm::vec3 operator ()(std::minstd_rand &) const override { return { 0.0f, 0.0f, 0.0f }; }
    //@}
};

//! An EmitterVolume that emits particles from a line segment.
//!
//! The points are distributed uniformly in the volume using this function:
//!	<pre>
//!		Given the value a and the random value <tt>t:[0,1)</tt>,
//!		<tt>
//!			x = a * t - a/2
//!			y = 0
//!			z = 0
//!		</tt>
//! </pre>

class EmitterLine : public EmitterVolume
{
public:

    //! Constructor.
    EmitterLine(float size);

    //! Destructor.
    virtual ~EmitterLine() override = default;

    //! @name Overrides EmitterVolume
    //@{
    glm::vec3 operator ()(std::minstd_rand & rng) const override;
    //@}

private:
    std::uniform_real_distribution<float> randomX;
};

//! An EmitterVolume that emits particles from the interior of a rectangle.
//!
//! The points are distributed uniformly in the volume using this function:
//!	<pre>
//!		Given the value <tt>[a,b]</tt> and the random values <tt>t:[0,1)</tt> and <tt>u:[0,1)</tt>,
//!		<tt>
//!			x = a * t - a/2
//!			y = b * u - b/2
//!			z = 0
//!		</tt>
//!	</pre>

class EmitterRectangle : public EmitterVolume
{
public:

    //! Constructor.
    EmitterRectangle(float w, float h);

    //! Destructor.
    virtual ~EmitterRectangle() override = default;

    //! @name Overrides EmitterVolume
    //@{
    glm::vec3 operator ()(std::minstd_rand & rng) const override;
    //@}

private:
    std::uniform_real_distribution<float> randomX_;
    std::uniform_real_distribution<float> randomZ_;
};

//! An EmitterVolume that emits particles from the interior of a circle.
//!
//! The points are distributed uniformly in the volume using this function:
//!	<pre>
//!		Given the radius r and the random values <tt>t:[0,1)</tt> and <tt>u:[0,1)</tt>,
//!		<tt>
//!			x = t * r * cos( u * TWO_PI )
//!			y = t * r * sin( u * TWO_PI )
//!			z = 0
//!		</tt>
//!	</pre>

class EmitterCircle : public EmitterVolume
{
public:

    //! Constructor.
    EmitterCircle(float radius);

    //! Destructor.
    virtual ~EmitterCircle() override = default;

    //! @name Overrides EmitterVolume
    //@{
    glm::vec3 operator ()(std::minstd_rand & rng) const override;
    //@}

private:
    std::uniform_real_distribution<float> randomAngle_;
    std::uniform_real_distribution<float> randomR_;
};

//! An EmitterVolume that emits particles from the interior of a sphere.
//!
//! The points are distributed uniformly in the volume using this function:
//!	<pre>
//!		Given the radius r and the random values <tt>t:[0,1)</tt>, <tt>u:[0,1)</tt>, and <tt>v:[0,1)</tt>,
//!		<tt>
//!			x = t * r * sin( u * PI ) * cos( v * TWO_PI )
//!			y = t * r * sin( u * PI ) * sin( v * TWO_PI )
//!			z = t * r * cos( u * PI )
//!		</tt>
//!	</pre>

class EmitterSphere : public EmitterVolume
{
public:

    //! Constructor.
    EmitterSphere(float radius);

    //! Destructor.
    virtual ~EmitterSphere() override = default;

    //! @name Overrides EmitterVolume
    //@{
    glm::vec3 operator ()(std::minstd_rand & rng) const override;
    //@}

private:
    std::uniform_real_distribution<float> randomR_;
    std::uniform_real_distribution<float> randomAngle_;
    std::uniform_real_distribution<float> randomCos_;
};

//! An EmitterVolume that emits particles from the interior of a box.
//!
//! The points are distributed uniformly in the volume using this function:
//!	<pre>
//!		Given the value <tt>[a,b,c]</tt> and the random values <tt>t:[0,1)</tt>, <tt>u:[0,1)</tt>, and
//!		<tt>v:[0,1)</tt>,
//!		<tt>
//!			x = a * t - a/2
//!			y = b * u - b/2
//!			z = c * v - c/2
//!		</tt>
//!	</pre>

class EmitterBox : public EmitterVolume
{
public:

    //! Constructor.
    EmitterBox(glm::vec3 const & size);

    //! Destructor.
    virtual ~EmitterBox() override = default;

    //! @name Overrides EmitterVolume
    //@{
    glm::vec3 operator ()(std::minstd_rand & rng) const override;
    //@}

private:
    std::uniform_real_distribution<float> randomX_;
    std::uniform_real_distribution<float> randomY_;
    std::uniform_real_distribution<float> randomZ_;
};

//! An EmitterVolume that emits particles from the interior of a cylinder.
//!
//! The points are distributed uniformly in the volume using this function:
//!	<pre>
//!		Given the values r and h, and the random values <tt>t:[0,1)</tt>, <tt>u:[0,1)</tt>, and
//!		<tt>v:[0,1)</tt>,
//!		<tt>
//!			x = t * r * cos( u * TWO_PI )
//!			y = t * r * sin( u * TWO_PI )
//!			z = h * v
//!		</tt>
//!	</pre>

class EmitterCylinder : public EmitterVolume
{
public:

    //! Constructor.
    EmitterCylinder(float radius, float height);

    //! Destructor.
    virtual ~EmitterCylinder() override = default;

    //! @name Overrides EmitterVolume
    //@{
    glm::vec3 operator ()(std::minstd_rand & rng) const override;
    //@}

private:
    std::uniform_real_distribution<float> randomAngle_;
    std::uniform_real_distribution<float> randomR_;
    std::uniform_real_distribution<float> randomZ_;
};

//! An EmitterVolume that emits particles from the interior of a cone.
//!
//! The points are distributed uniformly in the volume using this function:
//!	<pre>
//!		Given the values r and h, and the random values <tt>t:[0,1)</tt>, <tt>u:[0,1)</tt>, and
//!		<tt>v:[0,1)</tt>,
//!		<tt>
//!			x = sqrt( t ) / r * v**(1/3) * h * cos( u * TWO_PI )
//!			y = sqrt( t ) / r * v**(1/3) * h * sin( u * TWO_PI )
//!			z = v**(1/3) * h
//!		</tt>
//!	</pre>

class EmitterCone : public EmitterVolume
{
public:

    //! Constructor.
    EmitterCone(float radius, float height);

    //! Destructor.
    virtual ~EmitterCone() override = default;

    //! @name Overrides EmitterVolume
    //@{
    glm::vec3 operator ()(std::minstd_rand & rng) const override;
    //@}

private:
    std::uniform_real_distribution<float> randomAngle_;
    std::uniform_real_distribution<float> randomZ_;
    std::uniform_real_distribution<float> randomR_;
};
} // namespace Confetti

#endif // !defined(CONFETTI_EMITTERVOLUME_H)
