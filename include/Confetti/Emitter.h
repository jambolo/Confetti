#pragma once

#if !defined(CONFETTI_EMITTER_H)
#define CONFETTI_EMITTER_H

#include "Misc/Assertx.h"
#include "PointParticle.h"
#include "SphereParticle.h"
#include "StreakParticle.h"
#include "TexturedParticle.h"
#include <DirectXMath.h>
#include <memory>

struct ID3D11Device;
struct ID3D11Buffer;
struct ID3D11Buffer;
struct ID3DX11Effect;
struct IDirect3DVertexDeclaration11;

namespace Confetti
{
class EmitterVolume;
class Appearance;
class Environment;

//! A particle emitter.
//!
//! @ingroup	Emitters
//!
//! @note This class is an abstract base class and must be derived from in order to be used.

class BasicEmitter
{
public:

    //! Constructor
    BasicEmitter(ID3D11Device *        pD3dDevice,
                 Particle *            paParticles,
                 int                   size,
                 EmitterVolume const * pVol,
                 Environment const *   pEnv,
                 Appearance const *    pApp,
                 int                   n,
                 bool                  sorted);

    // Destructor
    virtual ~BasicEmitter();

    //! Returns a pointer to the array of particles.
    Particle * particles() { return paParticles_; }

    //! Returns a pointer to the array of particles.
    Particle const * particles() const { return paParticles_; }

    //! Returns the emitter volume.
    EmitterVolume const * emitterVolume() const { return pEmitterVolume_; }

    //! Returns the appearance.
    Appearance const * appearance() const { return pAppearance_; }

    //! Returns the environment.
    Environment const * environment() const { return pEnvironment_; }

    //! Returns the number of particles.
    int size() const { return numParticles_; }

    //! Returns the current position.
    DirectX::XMFLOAT3 const & currentPosition() const { return position_; }

    //! Returns the current velocity.
    DirectX::XMFLOAT3 const & currentVelocity() const { return velocity_; }

    //! Returns true if the emitter is enabled.
    bool enabled() const { return enabled_; }

    //! Returns true if the particles are sorted.
    bool sorted() const { return sorted_; }

    //! Enables/Disables the emitter. Returns the previous state.
    bool enable(bool enable = true);

    //! Sets the emitter's position and velocity
    void update(DirectX::XMFLOAT3 const & position, DirectX::XMFLOAT3 const & velocity);

    //! Updates the emitter and its particles.
    void update(float dt, DirectX::XMFLOAT3 const & position, DirectX::XMFLOAT3 const & velocity);

    //! Updates the emitter's particles.
    //!
    //! @note	This method must be overridden.
    virtual void update(float dt) = 0;

    //! Draws all the emitter's particles
    //!
    //! @note	This method must be overridden.
    virtual void draw() const = 0;

    //! Returns a reference to an individual particle
    //!
    //! @note	This method must be overridden.
    virtual Particle * particle(int i) = 0;

    //! Returns a reference to an individual particle
    //!
    //! @note	This method must be overridden.
    virtual Particle const * particle(int i) const = 0;

protected:

    // D3D Stuff

    ID3D11Device * pD3dDevice_;                         //!< D3D device
    ID3D11Buffer * pVB_;                                //!< Vertex buffer for particles
    ID3DBlob * pEffect_;                                //!< The effect for rendering this emitter's particles
    IDirect3DVertexDeclaration11 * pVertexDeclaration_; //!< The vertex buffer format
    bool alphaTestAvailable_;                           //!< true if the device supports alpha test
    int maxPrimitives_;                                 //!< Maximum number of primitives in a DrawPrimitive call
    int maxVertexIndex_;                                //!< Highest possible index value

private:

    // non-copyable
    BasicEmitter(BasicEmitter const &) = delete;
    BasicEmitter & operator =(BasicEmitter const &) = delete;

    // Particle data

    Particle * paParticles_;                // Particle array
    EmitterVolume const * pEmitterVolume_;  // Emitter volume
    Appearance const * pAppearance_;        // Common appearance parameters
    Environment const * pEnvironment_;      // Common environment parameters
    int numParticles_;                      // Number of particles
    bool sorted_;                           // Should the emitter sort the particles back to front?

    // Emitter state

    bool enabled_;                  // Enabled or not
    DirectX::XMFLOAT3 position_;    // Current position
    DirectX::XMFLOAT3 velocity_;    // Current velocity
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
    PointEmitter(ID3D11Device *        pD3dDevice,
                 EmitterVolume const * pVol,
                 Environment const *   pEnv,
                 Appearance const *    pApp,
                 int                   n,
                 bool                  sorted);

    //! Constructor
    PointEmitter(ID3D11Device *                 pD3dDevice,
                 std::unique_ptr<PointParticle> qaParticles,
                 EmitterVolume const *          pVol,
                 Environment const *            pEnv,
                 Appearance const *             pApp,
                 int                            n,
                 bool                           sorted);

    virtual ~PointEmitter() override;

    //! Initializes the emitter
    void initialize();

    //! Uninitializes the emitter
    void uninitialize();

    //! @name Overrides BasicEmitter
    //@{
    PointParticle * particles()
    {
        return static_cast<PointParticle *>(BasicEmitter::particles());
    }
    PointParticle const * particles() const
    {
        return static_cast<PointParticle const *>(BasicEmitter::particles());
    }
    virtual PointParticle * particle(int i) override
    {
        assert_limits(0, i, size() - 1); return particles() + i;
    }
    virtual PointParticle const * particle(int i) const override
    {
        assert_limits(0, i, size() - 1); return particles() + i;
    }
    virtual void update(float dt) override;
    virtual void draw() const override;
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
    StreakEmitter(ID3D11Device *        pD3dDevice,
                  EmitterVolume const * pVol,
                  Environment const *   pEnv,
                  Appearance const *    pApp,
                  int                   n,
                  bool                  sorted);

    //! Constructor
    StreakEmitter(ID3D11Device *                  pD3dDevice,
                  std::unique_ptr<StreakParticle> qaParticles,
                  EmitterVolume const *           pVol,
                  Environment const *             pEnv,
                  Appearance const *              pApp,
                  int                             n,
                  bool                            sorted);

    virtual ~StreakEmitter() override;

    //! Initializes the emitter
    void initialize();

    //! Uninitializes the emitter
    void uninitialize();

    //! @name Overrides BasicEmitter
    //@{
    StreakParticle * particles()
    {
        return static_cast<StreakParticle *>(BasicEmitter::particles());
    }
    StreakParticle const * particles() const
    {
        return static_cast<StreakParticle const *>(BasicEmitter::particles());
    }
    virtual StreakParticle * particle(int i) override
    {
        assert_limits(0, i, size() - 1); return particles() + i;
    }
    virtual StreakParticle const * particle(int i) const override
    {
        assert_limits(0, i, size() - 1); return particles() + i;
    }
    virtual void update(float dt) override;
    virtual void draw() const override;
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
    TexturedEmitter(ID3D11Device *        pD3dDevice,
                    EmitterVolume const * pVol,
                    Environment const *   pEnv,
                    Appearance const *    pApp,
                    int                   n,
                    bool                  sorted);

    //! Constructor
    TexturedEmitter(ID3D11Device *                    pD3dDevice,
                    std::unique_ptr<TexturedParticle> qaParticles,
                    EmitterVolume const *             pVol,
                    Environment const *               pEnv,
                    Appearance const *                pApp,
                    int                               n,
                    bool                              sorted);

    virtual ~TexturedEmitter() override;

    //! Initializes the emitter
    void initialize();

    //! Uninitializes the emitter
    void uninitialize();

    //! @name Overrides BasicEmitter
    //@{
    TexturedParticle * particles()
    {
        return static_cast<TexturedParticle *>(BasicEmitter::particles());
    }
    TexturedParticle const * particles() const
    {
        return static_cast<TexturedParticle const *>(BasicEmitter::particles());
    }
    virtual TexturedParticle * particle(int i) override
    {
        assert_limits(0, i, size() - 1); return particles() + i;
    }
    virtual TexturedParticle const * particle(int i) const override
    {
        assert_limits(0, i, size() - 1); return particles() + i;
    }
    virtual void update(float dt) override;
    virtual void draw() const override;
    //@}

private:

    ID3D11Buffer * pIB_;                      // Index buffer for particles
};

//! An Emitter that emits SphereParticles
//
//! @ingroup	Emitters
//!

class SphereEmitter : public BasicEmitter
{
public:

    //! Constructor
    SphereEmitter(ID3D11Device *        pD3dDevice,
                  EmitterVolume const * pVol,
                  Environment const *   pEnv,
                  Appearance const *    pApp,
                  int                   n,
                  bool                  sorted);

    //! Constructor
    SphereEmitter(ID3D11Device *                  pD3dDevice,
                  std::unique_ptr<SphereParticle> qaParticles,
                  EmitterVolume const *           pVol,
                  Environment const *             pEnv,
                  Appearance const *              pApp,
                  int                             n,
                  bool                            sorted);

    virtual ~SphereEmitter() override;

    //! Initializes the emitter
    void initialize();

    //! Uninitializes the emitter
    void uninitialize();

    //! @name Overrides BasicEmitter
    //@{
    SphereParticle * particles()
    {
        return static_cast<SphereParticle *>(BasicEmitter::particles());
    }
    SphereParticle const * particles() const
    {
        return static_cast<SphereParticle const *>(BasicEmitter::particles());
    }
    virtual SphereParticle * particle(int i) override
    {
        assert_limits(0, i, size() - 1); return particles() + i;
    }
    virtual SphereParticle const * particle(int i) const override
    {
        assert_limits(0, i, size() - 1); return particles() + i;
    }
    virtual void update(float dt) override;
    virtual void draw() const override;
    //@}
};
} // namespace Confetti

#endif // !defined(CONFETTI_EMITTER_H)
