/** @file *//********************************************************************************************************

                                                     Appearance.cpp

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Appearance.cpp#11 $

    $NoKeywords: $

********************************************************************************************************************/

#include "Appearance.h"

#include <Wx/Wx.h>

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

namespace Confetti
{
//! @param	colorRate		Color rate of change
//! @param	radiusRate		Radius rate of change
//! @param	angularVelocity	Angular velocity
//! @param	pMaterial		The particles' material.
//! @param	pTexture		The particles' texture.
//! @param	size			Size of the particles (line width or radius)

Appearance::Appearance(Dxx::Camera const *       pCamera,
                       DirectX::XMFLOAT4 const & colorRate /* = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f )*/,
                       float                     radiusRate /* = 0.0f*/,
                       float                     angularVelocity /* = 0.0f*/,
                       D3DMATERIAL9 const *      pMaterial /* = nullptr*/,
                       IDirect3DTexture11 *      pTexture /* = nullptr*/,
                       float                     size /* = 1.0f*/)
    : pCamera_(pCamera)
    , colorRate_(colorRate)
    , radiusRate_(radiusRate)
    , angularVelocity_(angularVelocity)
    , pMaterial_(pMaterial)
    , pTexture_(pTexture)
    , size_(size)
{
    assert(pCamera);
    assert(size > 0.0f);

    if (pTexture_)
        pTexture_->AddRef();
}

Appearance::~Appearance()
{
    Wx::SafeRelease(pTexture_);
}

Appearance::Appearance(Appearance const & src)
    : pMaterial_(src.pMaterial_)
    , pTexture_(src.pTexture_)
    , colorRate_(src.colorRate_)
    , radiusRate_(src.radiusRate_)
    , angularVelocity_(src.angularVelocity_)
    , pCamera_(src.pCamera_)
    , size_(src.size_)
{
    if (pTexture_)
        pTexture_->AddRef();
}

Appearance & Appearance::operator =(Appearance const & rhs)
{
    if (this != &rhs)
    {
        Wx::SafeRelease(pTexture_);

        pMaterial_ = rhs.pMaterial_;
        pTexture_  = rhs.pTexture_;
        if (pTexture_)
            pTexture_->AddRef();
        colorRate_       = rhs.colorRate_;
        radiusRate_      = rhs.radiusRate_;
        angularVelocity_ = rhs.angularVelocity_;
        pCamera_         = rhs.pCamera_;
        size_ = rhs.size_;
    }

    return *this;
}

//!
//! @param	dt		Amount of time elapsed since the last update

void Appearance::Update(float dt)
{
    // Nothing to update
}
} // namespace Confetti
