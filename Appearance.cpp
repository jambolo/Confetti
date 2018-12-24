/** @file *//********************************************************************************************************

                                                     Appearance.cpp

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Appearance.cpp#11 $

    $NoKeywords: $

********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "Appearance.h"

namespace Confetti
{
/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	colorRate		Color rate of change
//! @param	radiusRate		Radius rate of change
//! @param	angularVelocity	Angular velocity
//! @param	pMaterial		The particles' material.
//! @param	pTexture		The particles' texture.
//! @param	size			Size of the particles (line width or radius)

Appearance::Appearance(Dxx::Camera const *  pCamera,
                       D3DXCOLOR const &    colorRate /*			= D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f )*/,
                       float                radiusRate /*		= 0.0f*/,
                       float                angularVelocity /*	= 0.0f*/,
                       D3DMATERIAL9 const * pMaterial /*			= 0*/,
                       IDirect3DTexture9 *  pTexture /*			= 0*/,
                       float                size /*				= 1.0f*/)
    : m_pCamera(pCamera),
    m_ColorRate(colorRate),
    m_RadiusRate(radiusRate),
    m_AngularVelocity(angularVelocity),
    m_pMaterial(pMaterial),
    m_pTexture(pTexture),
    m_Size(size)
{
    assert(pCamera != 0);
    assert(size > 0.0f);

    if (m_pTexture != 0)
        m_pTexture->AddRef();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Appearance::~Appearance()
{
    Wx::SafeRelease(m_pTexture);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Appearance::Appearance(Appearance const & src)
    : m_pMaterial(src.m_pMaterial),
    m_pTexture(src.m_pTexture),
    m_ColorRate(src.m_ColorRate),
    m_RadiusRate(src.m_RadiusRate),
    m_AngularVelocity(src.m_AngularVelocity),
    m_pCamera(src.m_pCamera),
    m_Size(src.m_Size)
{
    if (m_pTexture != 0)
        m_pTexture->AddRef();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Appearance & Appearance::operator =(Appearance const & rhs)
{
    if (this != &rhs)
    {
        Wx::SafeRelease(m_pTexture);

        m_pMaterial = rhs.m_pMaterial;
        m_pTexture  = rhs.m_pTexture;
        if (m_pTexture != 0)
            m_pTexture->AddRef();
        m_ColorRate       = rhs.m_ColorRate;
        m_RadiusRate      = rhs.m_RadiusRate;
        m_AngularVelocity = rhs.m_AngularVelocity;
        m_pCamera         = rhs.m_pCamera;
        m_Size = rhs.m_Size;
    }

    return *this;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//!
//! @param	dt		Amount of time elapsed since the last update

void Appearance::Update(float dt)
{
    // Nothing to update
}
} // namespace Confetti
