/** @file *//********************************************************************************************************

                                                     EmitterVolume.h

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/EmitterVolume.h#8 $

    $NoKeywords: $

********************************************************************************************************************/

#pragma once

#include "Dxx/d3d.h"
#include "Misc/Random.h"
#include <DirectXMath.h>

namespace Confetti
{
//! This class generates random points in 3D whose locations are uniformly distributed in a specific volume.
//
//! @note	This is an abstract base class, so it must be derived from to be used.
//!

class EmitterVolume
{
public:

    //! Constructor
    EmitterVolume(unsigned int seed);

    // Destructor
    virtual ~EmitterVolume() = default;

    //! Returns a value used to specify a particle's point of emission.
    //
    //!
    //! @note	This method must be overridden.
    virtual DirectX::XMFLOAT4 next() const = 0;

protected:

    mutable RandomFloat rng_;      // A random number generator.
};

//! An EmitterVolume that emits particles from the point <tt>[0,0,0]</tt>.

class EmitterPoint : public EmitterVolume
{
public:

    //! Constructor
    EmitterPoint(unsigned int seed);

    //! Destructor
    virtual ~EmitterPoint() override = default;

    //! @name Overrides EmitterVolume
    //@{
    DirectX::XMFLOAT4 next() const override { return DirectX::XMVectorZero(); }
    //@}
};

//! An EmitterVolume that emits particles from a line segment.
//
//! The points are distributed uniformly in the volume using this function:
//!	<pre>
//!		Given the value @c a and the random value <tt>t:[0,1)</tt>,
//!		<tt>
//!			x = a * t - a/2
//!			y = 0
//!			z = 0
//!		</tt>
//! </pre>

class EmitterLine : public EmitterVolume
{
public:

    //! Constructor
    EmitterLine(unsigned int seed, float size);

    //! Destructor
    virtual ~EmitterLine() override = default;

    //! @name Overrides EmitterVolume
    //@{
    DirectX::XMFLOAT4 next() const override;
    //@}

private:

    float size_;       // The length of the line segment.
};

//! An EmitterVolume that emits particles from the interior of a rectangle.
//
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

    //! Constructor
    EmitterRectangle(unsigned int seed, float w, float h);

    //! Destructor
    virtual ~EmitterRectangle() override = default;

    //! @name Overrides EmitterVolume
    //@{
    DirectX::XMFLOAT4 next() const override;
    //@}

private:

    float width_, height_;     // The width and height of the rectangle.
};

//! An EmitterVolume that emits particles from the interior of a circle.
//
//! The points are distributed uniformly in the volume using this function:
//!	<pre>
//!		Given the radius @c r and the random values <tt>t:[0,1)</tt> and <tt>u:[0,1)</tt>,
//!		<tt>
//!			x = t * r * cos( u * TWO_PI )
//!			y = t * r * sin( u * TWO_PI )
//!			z = 0
//!		</tt>
//!	</pre>

class EmitterCircle : public EmitterVolume
{
public:

    //! Constructor
    EmitterCircle(unsigned int seed, float radius);

    //! Destructor
    virtual ~EmitterCircle() override = default;

    //! @name Overrides EmitterVolume
    //@{
    DirectX::XMFLOAT4 next() const override;
    //@}

private:

    float radius_;         // The radius of the circle.
};

//! An EmitterVolume that emits particles from the interior of a sphere.
//
//! The points are distributed uniformly in the volume using this function:
//!	<pre>
//!		Given the radius @c r and the random values <tt>t:[0,1)</tt>, <tt>u:[0,1)</tt>, and <tt>v:[0,1)</tt>,
//!		<tt>
//!			x = t * r * sin( u * PI ) * cos( v * TWO_PI )
//!			y = t * r * sin( u * PI ) * sin( v * TWO_PI )
//!			z = t * r * cos( u * PI )
//!		</tt>
//!	</pre>

class EmitterSphere : public EmitterVolume
{
public:

    //! Constructor
    EmitterSphere(unsigned int seed, float radius);

    //! Destructor
    virtual ~EmitterSphere() override = default;

    //! @name Overrides EmitterVolume
    //@{
    DirectX::XMFLOAT4 next() const override;
    //@}

private:

    float radius_;         // The radius of the sphere.
};

//! An EmitterVolume that emits particles from the interior of a box.
//
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

    //! Constructor
    EmitterBox(unsigned int seed, DirectX::XMFLOAT4 const & size);

    //! Destructor
    virtual ~EmitterBox() override = default;

    //! @name Overrides EmitterVolume
    //@{
    DirectX::XMFLOAT4 next() const override;
    //@}

private:

    DirectX::XMFLOAT4 size_;     // The width, height, and depth of the box.
};

//! An EmitterVolume that emits particles from the interior of a cylinder.
//
//! The points are distributed uniformly in the volume using this function:
//!	<pre>
//!		Given the values @c r and @c h, and the random values <tt>t:[0,1)</tt>, <tt>u:[0,1)</tt>, and
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

    //! Constructor
    EmitterCylinder(unsigned int seed, float radius, float height);

    //! Destructor
    virtual ~EmitterCylinder() override = default;

    //! @name Overrides EmitterVolume
    //@{
    DirectX::XMFLOAT4 next() const override;
    //@}

private:

    float radius_;             // The radius of the cylinder.
    float height_;             // The height of the cylinder.
};

//! An EmitterVolume that emits particles from the interior of a cone.
//
//! The points are distributed uniformly in the volume using this function:
//!	<pre>
//!		Given the values @c r and @c h, and the random values <tt>t:[0,1)</tt>, <tt>u:[0,1)</tt>, and
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

    //! Constructor
    EmitterCone(unsigned int seed, float radius, float height);

    //! Destructor
    virtual ~EmitterCone() override = default;

    //! @name Overrides EmitterVolume
    //@{
    DirectX::XMFLOAT4 next() const override;
    //@}

private:

    float radius_;             // The radius of the cone at the base.
    float height_;             // The height of the cone.
};
} // namespace Confetti
