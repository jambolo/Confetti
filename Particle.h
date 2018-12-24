/** @file *//********************************************************************************************************

                                                     Particle.h

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Particle.h#8 $

    $NoKeywords: $

********************************************************************************************************************/

#pragma once

#include <d3dx9math.h>

struct IDirect3DDevice9;

namespace Confetti
{
class BasicEmitter;

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! A particle base class.
//
//! @ingroup	Particles
//!
//! A Particle is a point with a lifetime, age, position, and velocity. It has no size, shape, orientation, color,
//! or texture.
//!
//! @note This is an abstract base class. You must derive from this class in order to use it.

class Particle
{
public:

    //! Constructor
    Particle() {}

    //! Constructor
    Particle(BasicEmitter const * pEmitter,
             float                lifetime,
             float                age,
             D3DXVECTOR3 const &  position,
             D3DXVECTOR3 const &  velocity,
             D3DXCOLOR const &    color);

    // Destructor
    virtual ~Particle();

    //! Updates the particle. Returns true if the particle was reborn.
    virtual bool Update(float dt);

    //! Draws the particle.
    //
    //!
    //! @note	This method must be overridden.
    virtual void Draw(IDirect3DDevice9 * pD3dDevice) const = 0;

    //! Binds to an emitter.
    void Bind(BasicEmitter * pEmitter) { m_pEmitter = pEmitter; }

    //! Returns the age of the particle.
    float GetAge() const { return m_Age; }

    //! Returns the particle's current position.
    D3DXVECTOR3 GetPosition() const { return m_Position; }

    //! Returns the particle's current velocity.
    D3DXVECTOR3 GetVelocity() const { return m_Velocity; }

    //! Returns the particle's current velocity.
    D3DXCOLOR GetColor() const { return m_Color; }

protected:

    //! Initializes after using the default constructor.
    void Initialize(float               lifetime,
                    float               age,
                    D3DXVECTOR3 const & position,
                    D3DXVECTOR3 const & velocity,
                    D3DXCOLOR const &   color);

    BasicEmitter const * m_pEmitter;            //!< This particle's emitter

    // Age data

    float m_Lifetime;                           //!< Max age
    float m_Age;                                //!< Current age

    // Motion data

    D3DXVECTOR3 m_InitialPosition;              //!< Position at birth relative to emitter
    D3DXVECTOR3 m_InitialVelocity;              //!< Velocity at birth relative to emitter

    D3DXVECTOR3 m_Position;                     //!< Current position
    D3DXVECTOR3 m_Velocity;                     //!< Current velocity

    // Appearance data

    D3DXCOLOR m_InitialColor;                   //!< Color at birth
    D3DXCOLOR m_Color;                          //!< Current color
};
} // namespace Confetti
