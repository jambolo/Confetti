/** @file *//********************************************************************************************************

                                                     Emitter.inl

                                            Copyright 2002, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Emitter.inl#12 $

    $NoKeywords: $

 *********************************************************************************************************************/

#pragma once

#include "Emitter.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <d3dx9.h>
#include <windows.h>

namespace Confetti
{
/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//!
//! @param	enabled		If true, the emitter is enabled, otherwise the emitter is disabled.

inline bool BasicEmitter::Enable(bool enabled)
{
    bool const oldState = m_Enabled;

    m_Enabled = enabled;

    return oldState;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	position	The new position of the emitter.
//! @param	velocity	The new velocity of the emitter.

inline void BasicEmitter::Update(D3DXVECTOR3 const & position, D3DXVECTOR3 const & velocity)
{
    m_Position = position;
    m_Velocity = velocity;
}
} // namespace Confetti
