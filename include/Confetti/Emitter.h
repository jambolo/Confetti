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
#include <DirectXMath.h>
#include <memory>

struct IDirect3DDevice11;
struct IDirect3DVertexBuffer11;
struct IDirect3DIndexBuffer11;
struct ID3DX11Effect;
struct IDirect3DVertexDeclaration11;

namespace Confetti
{
class EmitterVolume;
class Appearance;
class Environment;

//! A particle emitter.
//
//! @ingroup	Emitters
//!
//! @note This class is an abstract base class and must be derived from in order to be used.

class BasicEmitter
{
public:

    //! Constructor
    BasicEmitter(IDirect3DDevice11 * pD3dDevice,
                 Particle * paParticles,
                 int size, uint32_t usage, D3DPOOL pool,
                 EmitterVolume const * pVol,
                 Environment const * pEnv,
                 Appearance const * pApp,
                 int n,
                 bool sorted);

    // Destructor
    virtual ~BasicEmitter();

    // non-copyable
    BasicEmitter(BasicEmitter const &) = delete;
    BasicEmitter & operator =(BasicEmitter const &) = delete;

    //! Returns a pointer to the array of particles.
    Particle * GetParticles() { return paParticles_; }

    //! Returns a pointer to the array of particles.
    Particle const * GetParticles() const { return paParticles_; }

    //! Returns the emitter volume.
    EmitterVolume const * GetEmitterVolume() const { return pEmitterVolume_; }

    //! Returns the appearance.
    Appearance const * GetAppearance() const { return pAppearance_; }

    //! Returns the environment.
    Environment const * GetEnvironment() const { return pEnvironment_; }

    //! Returns the number of particles.
    int GetNumParticles() const { return numParticles_; }

    //! Returns the current position.
    DirectX::XMFLOAT4 const & GetCurrentPosition() const { return position_; }

    //! Returns the current velocity.
    DirectX::XMFLOAT4 const & GetCurrentVelocity() const { return velocity_; }

    //! Returns true if the emitter is enabled.
    bool IsEnabled() const { return enabled_; }

    //! Returns true if the particles are sorted.
    bool IsSorted() const { return sorted_; }

    //! Enables/Disables the emitter. Returns the previous state.
    bool Enable(bool enable);

    //! Sets the emitter's position and velocity
    void Update(DirectX::XMFLOAT4 const & position, DirectX::XMFLOAT4 const & velocity);

    //! Updates the emitter and its particles.
    void Update(float dt, DirectX::XMFLOAT4 const & position, DirectX::XMFLOAT4 const & velocity);

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

    IDirect3DDevice11 * pD3dDevice_;                    //!< D3D device
    IDirect3DVertexBuffer11 * pVB_;                     //!< Vertex buffer for particles
    ID3DX11Effect * pEffect_;                           //!< The effect for rendering this emitter's particles
    IDirect3DVertexDeclaration11 * pVertexDeclaration_; //!< The vertex buffer format
    bool alphaTestAvailable_;                           //!< @c true if the device supports alpha test
    int maxPrimitives_;                                 //!< Maximum number of primitives in a DrawPrimitive call
    int maxVertexIndex_;                                //!< Highest possible index value

private:

    // Particle data

    Particle * paParticles_;                // Particle array
    EmitterVolume const * pEmitterVolume_;  // Emitter volume
    Appearance const * pAppearance_;        // Common appearance parameters
    Environment const * pEnvironment_;      // Common environment parameters
    int numParticles_;                      // Number of particles
    bool sorted_;                           // Should the emitter sort the particles back to front?

    // Emitter state

    bool enabled_;                  // Enabled or not
    DirectX::XMFLOAT4 position_;    // Current position
    DirectX::XMFLOAT4 velocity_;    // Current velocity
};

//! @name Basic Emitters
//! @ingroup	Emitters
//@{

//! An Emitter that emits PointParticles
//
//! @ingroup	Emitters
//!

class PointEmitter : public BasicEmitter
{
public:

    //! Constructor
    PointEmitter(IDirect3DDevice11 *   pD3dDevice,
                 EmitterVolume const * pVol,
                 Environment const *   pEnv,
                 Appearance const *    pApp,
                 int                   n,
                 bool                  sorted);

    //! Constructor
    PointEmitter(IDirect3DDevice11 *          pD3dDevice,
                 std::auto_ptr<PointParticle> qaParticles,
                 EmitterVolume const *        pVol,
                 Environment const *          pEnv,
                 Appearance const *           pApp,
                 int                          n,
                 bool                         sorted);

    virtual ~PointEmitter() override;

    //! Initializes the emitter
    void Initialize();

    //! Uninitializes the emitter
    void Uninitialize();

    //! @name Overrides BasicEmitter
    //@{
    PointParticle * GetParticles()
    {
        return static_cast<PointParticle *>(BasicEmitter::GetParticles());
    }
    PointParticle const * GetParticles() const override
    {
        return static_cast<PointParticle const *>(BasicEmitter::GetParticles());
    }
    virtual PointParticle * GetParticle(int i) override
    {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual PointParticle const * GetParticle(int i) const override
    {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual void Update(float dt) override;
    virtual void Draw() const override;
    //@}
};

//! An Emitter that emits StreakParticles
//
//! @ingroup	Emitters
//!

class StreakEmitter : public BasicEmitter
{
public:

    //! Constructor
    StreakEmitter(IDirect3DDevice11 *   pD3dDevice,
                  EmitterVolume const * pVol,
                  Environment const *   pEnv,
                  Appearance const *    pApp,
                  int                   n,
                  bool                  sorted);

    //! Constructor
    StreakEmitter(IDirect3DDevice11 *           pD3dDevice,
                  std::auto_ptr<StreakParticle> qaParticles,
                  EmitterVolume const *         pVol,
                  Environment const *           pEnv,
                  Appearance const *            pApp,
                  int                           n,
                  bool                          sorted);

    virtual ~StreakEmitter() override;

    //! Initializes the emitter
    void Initialize();

    //! Uninitializes the emitter
    void Uninitialize();

    //! @name Overrides BasicEmitter
    //@{
    StreakParticle * GetParticles()
    {
        return static_cast<StreakParticle *>(BasicEmitter::GetParticles());
    }
    StreakParticle const * GetParticles() const
    {
        return static_cast<StreakParticle const *>(BasicEmitter::GetParticles());
    }
    virtual StreakParticle * GetParticle(int i) override
    {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual StreakParticle const * GetParticle(int i) const override
    {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual void Update(float dt) override;
    virtual void Draw() const override;
    //@}
};

//! An Emitter that emits TexturedParticles
//
//! @ingroup	Emitters
//!

class TexturedEmitter : public BasicEmitter
{
public:

    //! Constructor
    TexturedEmitter(IDirect3DDevice11 *   pD3dDevice,
                    EmitterVolume const * pVol,
                    Environment const *   pEnv,
                    Appearance const *    pApp,
                    int                   n,
                    bool                  sorted);

    //! Constructor
    TexturedEmitter(IDirect3DDevice11 *             pD3dDevice,
                    std::auto_ptr<TexturedParticle> qaParticles,
                    EmitterVolume const *           pVol,
                    Environment const *             pEnv,
                    Appearance const *              pApp,
                    int                             n,
                    bool                            sorted);

    virtual ~TexturedEmitter() override;

    //! Initializes the emitter
    void Initialize();

    //! Uninitializes the emitter
    void Uninitialize();

    //! @name Overrides BasicEmitter
    //@{
    TexturedParticle * GetParticles()
    {
        return static_cast<TexturedParticle *>(BasicEmitter::GetParticles());
    }
    TexturedParticle const * GetParticles() const
    {
        return static_cast<TexturedParticle const *>(BasicEmitter::GetParticles());
    }
    virtual TexturedParticle * GetParticle(int i) override
    {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual TexturedParticle const * GetParticle(int i) const override
    {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual void Update(float dt) override;
    virtual void Draw() const override;
    //@}

private:

    IDirect3DIndexBuffer11 * pIB_;                      // Index buffer for particles
};

//! An Emitter that emits SphereParticles
//
//! @ingroup	Emitters
//!

class SphereEmitter : public BasicEmitter
{
public:

    //! Constructor
    SphereEmitter(IDirect3DDevice11 *   pD3dDevice,
                  EmitterVolume const * pVol,
                  Environment const *   pEnv,
                  Appearance const *    pApp,
                  int                   n,
                  bool                  sorted);

    //! Constructor
    SphereEmitter(IDirect3DDevice11 *           pD3dDevice,
                  std::auto_ptr<SphereParticle> qaParticles,
                  EmitterVolume const *         pVol,
                  Environment const *           pEnv,
                  Appearance const *            pApp,
                  int                           n,
                  bool                          sorted);

    virtual ~SphereEmitter() override;

    //! Initializes the emitter
    void Initialize();

    //! Uninitializes the emitter
    void Uninitialize();

    //! @name Overrides BasicEmitter
    //@{
    SphereParticle * GetParticles()
    {
        return static_cast<SphereParticle *>(BasicEmitter::GetParticles());
    }
    SphereParticle const * GetParticles() const
    {
        return static_cast<SphereParticle const *>(BasicEmitter::GetParticles());
    }
    virtual SphereParticle * GetParticle(int i) override
    {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual SphereParticle const * GetParticle(int i) const override
    {
        assert_limits(0, i, GetNumParticles() - 1); return GetParticles() + i;
    }
    virtual void Update(float dt) override;
    virtual void Draw() const override;
    //@}
};
} // namespace Confetti

// Inline functions

#include "Emitter.inl"
