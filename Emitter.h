/** @file *//********************************************************************************************************

                                                      Emitter.h

                                            Copyright 2002, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Emitter.h#17 $

    $NoKeywords: $

 *********************************************************************************************************************/

#pragma once

#include "Misc/Assert.h"
#include "Misc/Types.h"
#include "PointParticle.h"
#include "SphereParticle.h"
#include "StreakParticle.h"
#include "TexturedParticle.h"
#include <boost/noncopyable.hpp>
#include <d3dx9math.h>

struct IDirect3DDevice9;
struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;
struct ID3DXEffect;
struct IDirect3DVertexDeclaration9;

namespace Confetti
{
class EmitterVolume;
class Appearance;
class Environment;

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! A particle emitter.
//
//! @ingroup	Emitters
//!
//! @note This class is an abstract base class and must be derived from in order to be used.

class BasicEmitter : public boost::noncopyable
{
public:

    //! Constructor
    BasicEmitter(IDirect3DDevice9 * pD3dDevice,
                 Particle * paParticles,
                 int size, uint32 usage, D3DPOOL pool,
                 EmitterVolume const * pVol,
                 Environment const * pEnv,
                 Appearance const * pApp,
                 int n,
                 bool sorted);

    // Destructor
    virtual ~BasicEmitter();

    //! Returns a pointer to the array of particles.
    Particle * GetParticles() { return m_paParticles; }

    //! Returns a pointer to the array of particles.
    Particle const * GetParticles()             const { return m_paParticles; }

    //! Returns the emitter volume.
    EmitterVolume const * GetEmitterVolume()    const { return m_pEmitterVolume; }

    //! Returns the appearance.
    Appearance const * GetAppearance()          const { return m_pAppearance; }

    //! Returns the environment.
    Environment const * GetEnvironment()        const { return m_pEnvironment; }

    //! Returns the number of particles.
    int GetNumParticles()                       const { return m_NumParticles; }

    //! Returns the current position.
    D3DXVECTOR3 const & GetCurrentPosition()    const { return m_Position; }

    //! Returns the current velocity.
    D3DXVECTOR3 const & GetCurrentVelocity()    const { return m_Velocity; }

    //! Returns true if the emitter is enabled.
    bool IsEnabled()                            const { return m_Enabled; }

    //! Returns true if the particles are sorted.
    bool IsSorted()                             const { return m_Sorted; }

    //! Enables/Disables the emitter. Returns the previous state.
    bool Enable(bool enable);

    //! Sets the emitter's position and velocity
    void Update(D3DXVECTOR3 const & position, D3DXVECTOR3 const & velocity);

    //! Updates the emitter and its particles.
    void Update(float dt, D3DXVECTOR3 const & position, D3DXVECTOR3 const & velocity);

    //! Updates the emitter's particles.
    //
    //!
    //! @note	This method must be overridden.
    virtual void Update(float dt) = 0;

    //! Draws all the emitter's particles
    //
    //!
    //! @note	This method must be overridden.
    virtual void Draw() const = 0;

    //! Returns a reference to an individual particle
    //
    //!
    //! @note	This method must be overridden.
    virtual Particle * GetParticle(int i) = 0;

    //! Returns a reference to an individual particle
    //
    //!
    //! @note	This method must be overridden.
    virtual Particle const * GetParticle(int i) const = 0;

protected:

    // D3D Stuff

    IDirect3DDevice9 * m_pD3dDevice;                        //!< D3D device
    IDirect3DVertexBuffer9 * m_pVB;                         //!< Vertex buffer for particles
    ID3DXEffect * m_pEffect;                                //!< The effect for rendering this emitter's particles
    IDirect3DVertexDeclaration9 * m_pVertexDeclaration;     //!< The vertex buffer format
    bool m_AlphaTestAvailable;                              //!< @c true if the device supports alpha test
    int m_MaxPrimitives;                                    //!< Maximum number of primitives in a DrawPrimitive call
    int m_MaxVertexIndex;                                   //!< Highest possible index value

private:

    // Particle data

    Particle * m_paParticles;                           // Particle array
    EmitterVolume const * m_pEmitterVolume;             // Emitter volume
    Appearance const * m_pAppearance;                   // Common appearance parameters
    Environment const * m_pEnvironment;                 // Common environment parameters
    int m_NumParticles;                                 // Number of particles
    bool m_Sorted;                                      // Should the emitter sort the particles back to front?

    // Emitter state

    bool m_Enabled;                                     // Enabled or not
    D3DXVECTOR3 m_Position;                             // Current position
    D3DXVECTOR3 m_Velocity;                             // Current velocity
};

//! @name Basic Emitters
//! @ingroup	Emitters
//@{

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! An Emitter that emits PointParticles
//
//! @ingroup	Emitters
//!

class PointEmitter : public BasicEmitter
{
public:

    //! Constructor
    PointEmitter(IDirect3DDevice9 *    pD3dDevice,
                 EmitterVolume const * pVol,
                 Environment const *   pEnv,
                 Appearance const *    pApp,
                 int                   n,
                 bool                  sorted);

    //! Constructor
    PointEmitter(IDirect3DDevice9 *           pD3dDevice,
                 std::auto_ptr<PointParticle> qaParticles,
                 EmitterVolume const *        pVol,
                 Environment const *          pEnv,
                 Appearance const *           pApp,
                 int                          n,
                 bool                         sorted);

    virtual ~PointEmitter();

    //! Initializes the emitter
    void Initialize();

    //! Uninitializes the emitter
    void Uninitialize();

    //! @name Overrides BasicEmitter
    //@{
    PointParticle * GetParticles()                            {
        return static_cast<PointParticle *>(BasicEmitter::GetParticles());
    }
    PointParticle const * GetParticles()                const {
        return static_cast<PointParticle const *>(BasicEmitter::GetParticles());
    }
    virtual PointParticle * GetParticle(int i)              {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual PointParticle const * GetParticle(int i)  const {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual void Update(float dt);
    virtual void Draw() const;
    //@}
};

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! An Emitter that emits StreakParticles
//
//! @ingroup	Emitters
//!

class StreakEmitter : public BasicEmitter
{
public:

    //! Constructor
    StreakEmitter(IDirect3DDevice9 *    pD3dDevice,
                  EmitterVolume const * pVol,
                  Environment const *   pEnv,
                  Appearance const *    pApp,
                  int                   n,
                  bool                  sorted);

    //! Constructor
    StreakEmitter(IDirect3DDevice9 *            pD3dDevice,
                  std::auto_ptr<StreakParticle> qaParticles,
                  EmitterVolume const *         pVol,
                  Environment const *           pEnv,
                  Appearance const *            pApp,
                  int                           n,
                  bool                          sorted);

    virtual ~StreakEmitter();

    //! Initializes the emitter
    void Initialize();

    //! Uninitializes the emitter
    void Uninitialize();

    //! @name Overrides BasicEmitter
    //@{
    StreakParticle * GetParticles()                           {
        return static_cast<StreakParticle *>(BasicEmitter::GetParticles());
    }
    StreakParticle const * GetParticles()               const {
        return static_cast<StreakParticle const *>(BasicEmitter::GetParticles());
    }
    virtual StreakParticle * GetParticle(int i)             {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual StreakParticle const * GetParticle(int i) const {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual void Update(float dt);
    virtual void Draw() const;
    //@}
};

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! An Emitter that emits TexturedParticles
//
//! @ingroup	Emitters
//!

class TexturedEmitter : public BasicEmitter
{
public:

    //! Constructor
    TexturedEmitter(IDirect3DDevice9 *    pD3dDevice,
                    EmitterVolume const * pVol,
                    Environment const *   pEnv,
                    Appearance const *    pApp,
                    int                   n,
                    bool                  sorted);

    //! Constructor
    TexturedEmitter(IDirect3DDevice9 *              pD3dDevice,
                    std::auto_ptr<TexturedParticle> qaParticles,
                    EmitterVolume const *           pVol,
                    Environment const *             pEnv,
                    Appearance const *              pApp,
                    int                             n,
                    bool                            sorted);

    virtual ~TexturedEmitter();

    //! Initializes the emitter
    void Initialize();

    //! Uninitializes the emitter
    void Uninitialize();

    //! @name Overrides BasicEmitter
    //@{
    TexturedParticle * GetParticles()                             {
        return static_cast<TexturedParticle *>(BasicEmitter::GetParticles());
    }
    TexturedParticle const * GetParticles()                 const {
        return static_cast<TexturedParticle const *>(BasicEmitter::GetParticles());
    }
    virtual TexturedParticle * GetParticle(int i)               {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual TexturedParticle const * GetParticle(int i)   const {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual void Update(float dt);
    virtual void Draw() const;
    //@}

private:

    IDirect3DIndexBuffer9 * m_pIB;                      // Index buffer for particles
};

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! An Emitter that emits SphereParticles
//
//! @ingroup	Emitters
//!

class SphereEmitter : public BasicEmitter
{
public:

    //! Constructor
    SphereEmitter(IDirect3DDevice9 *    pD3dDevice,
                  EmitterVolume const * pVol,
                  Environment const *   pEnv,
                  Appearance const *    pApp,
                  int                   n,
                  bool                  sorted);

    //! Constructor
    SphereEmitter(IDirect3DDevice9 *            pD3dDevice,
                  std::auto_ptr<SphereParticle> qaParticles,
                  EmitterVolume const *         pVol,
                  Environment const *           pEnv,
                  Appearance const *            pApp,
                  int                           n,
                  bool                          sorted);

    virtual ~SphereEmitter();

    //! Initializes the emitter
    void Initialize();

    //! Uninitializes the emitter
    void Uninitialize();

    //! @name Overrides BasicEmitter
    //@{
    SphereParticle * GetParticles()                           {
        return static_cast<SphereParticle *>(BasicEmitter::GetParticles());
    }
    SphereParticle const * GetParticles()               const {
        return static_cast<SphereParticle const *>(BasicEmitter::GetParticles());
    }
    virtual SphereParticle * GetParticle(int i)             {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual SphereParticle const * GetParticle(int i) const {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual void Update(float dt);
    virtual void Draw() const;
    //@}
};
} // namespace Confetti

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

// Inline functions

#include "Emitter.inl"
