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
#include <d3dx9math.h>

namespace Confetti
{
/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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
    virtual ~EmitterVolume();

    //! Returns a value used to specify a particle's point of emission.
    //
    //!
    //! @note	This method must be overridden.
    virtual D3DXVECTOR3 Next() const = 0;

protected:

    mutable RandomFloat m_Rng;      // A random number generator.
};

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! An EmitterVolume that emits particles from the point <tt>[0,0,0]</tt>.

class EmitterPoint : public EmitterVolume
{
public:

    //! Constructor
    EmitterPoint(unsigned int seed);

    //! Destructor
    virtual ~EmitterPoint();

    //! @name Overrides EmitterVolume
    //@{
    D3DXVECTOR3 Next() const { return Dxx::Vector3Origin(); }
    //@}
};

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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
    virtual ~EmitterLine();

    //! @name Overrides EmitterVolume
    //@{
    D3DXVECTOR3 Next() const;
    //@}

private:

    float m_Size;       // The length of the line segment.
};

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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
    EmitterRectangle(unsigned int seed, D3DXVECTOR2 const & size);

    //! Destructor
    virtual ~EmitterRectangle();

    //! @name Overrides EmitterVolume
    //@{
    D3DXVECTOR3 Next() const;
    //@}

private:

    D3DXVECTOR2 m_Size;     // The width and height of the rectangle.
};

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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
    virtual ~EmitterCircle();

    //! @name Overrides EmitterVolume
    //@{
    D3DXVECTOR3 Next() const;
    //@}

private:

    float m_Radius;         // Thre radius of the circle.
};

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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
    virtual ~EmitterSphere();

    //! @name Overrides EmitterVolume
    //@{
    D3DXVECTOR3 Next() const;
    //@}

private:

    float m_Radius;         // The radius of the sphere.
};

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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
    EmitterBox(unsigned int seed, D3DXVECTOR3 const & size);

    //! Destructor
    virtual ~EmitterBox();

    //! @name Overrides EmitterVolume
    //@{
    D3DXVECTOR3 Next() const;
    //@}

private:

    D3DXVECTOR3 m_Size;     // The width, height, and depth of the box.
};

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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
    virtual ~EmitterCylinder();

    //! @name Overrides EmitterVolume
    //@{
    D3DXVECTOR3 Next() const;
    //@}

private:

    float m_Radius;             // The radius of the cylinder.
    float m_Height;             // The height of the cylinder.
};

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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
    virtual ~EmitterCone();

    //! @name Overrides EmitterVolume
    //@{
    D3DXVECTOR3 Next() const;
    //@}

private:

    float m_Radius;             // The radius of the cone at the base.
    float m_Height;             // The height of the cone.
};
} // namespace Confetti
