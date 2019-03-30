#pragma once

#if !defined(CONFETTI_EMITTER_H)
#define CONFETTI_EMITTER_H

#include <Confetti/PointParticle.h>
#include <Confetti/SphereParticle.h>
#include <Confetti/StreakParticle.h>
#include <Confetti/TexturedParticle.h>
#include <glm/glm.hpp>
#include <memory>
#include <Misc/Assertx.h>
#include <vector>

namespace Vkx
{
    class LocalBuffer;
}

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

    //! Constructor.
    BasicEmitter(std::shared_ptr<Vkx::Device>   device,
                 int                            size,
                 std::shared_ptr<EmitterVolume> volume,
                 std::shared_ptr<Environment>   environment,
                 std::shared_ptr<Appearance>    appearance,
                 bool                           sorted);

    //! Destructor.
    virtual ~BasicEmitter() = default;

    //! Returns the emitter volume.
    std::shared_ptr<EmitterVolume> emitterVolume() const { return volume_; }

    //! Returns the appearance.
    std::shared_ptr<Appearance> appearance() const { return appearance_; }

    //! Returns the environment.
    std::shared_ptr<Environment> environment() const { return environment_; }

    //! Returns the current position.
    glm::vec3 currentPosition() const { return position_; }

    //! Returns the current velocity.
    glm::vec3 currentVelocity() const { return velocity_; }

    //! Returns true if the emitter is enabled.
    bool enabled() const { return enabled_; }

    //! Returns true if the particles are sorted.
    bool sorted() const { return sorted_; }

    //! Enables/Disables the emitter. Returns the previous state.
    bool enable(bool enable = true);

    //! Sets the emitter's position and velocity
    void update(glm::vec3 const & position, glm::vec3 const & velocity);

    //! Updates the emitter and its particles.
    void update(float dt, glm::vec3 const & position, glm::vec3 const & velocity);

    //! Updates the emitter's particles.
    //!
    //! @note	This method must be overridden.
    virtual void update(float dt) = 0;

    //! Draws all the emitter's particles
    //!
    //! @note	This method must be overridden.
    virtual void draw() const = 0;

protected:

//    std::shared_ptr<Vkx::LocalBuffer> pVB_;             //!< Vertex buffer for particles
//    bool alphaTestAvailable_;                           //!< true if the device supports alpha test
//    int maxPrimitives_;                                 //!< Maximum number of primitives in a DrawPrimitive call
//    int maxVertexIndex_;                                //!< Highest possible index value

private:
    // Particle data

    std::shared_ptr<EmitterVolume> volume_;     // Emitter volume
    std::shared_ptr<Appearance> appearance_;    // Common appearance parameters
    std::shared_ptr<Environment> environment_;  // Common environment parameters
    bool sorted_;                               // Should the emitter sort the particles back to front?

    // Emitter state

    bool enabled_;          // Enabled or not
    glm::vec3 position_;    // Current position
    glm::vec3 velocity_;    // Current velocity
};

//! @name Basic Emitters
//! @ingroup	Emitters
//@{

//! An Emitter that emits PointParticles
//!
//! @ingroup	Emitters
//!

class PointEmitter : public BasicEmitter
{
public:

    //! Constructor.
    PointEmitter(std::shared_ptr<Vkx::Device>   device,
                 int                            n,
                 std::shared_ptr<EmitterVolume> volume,
                 std::shared_ptr<Environment>   environment,
                 std::shared_ptr<Appearance>    appearance,
                 bool                           sorted);

    //! Constructor.
    PointEmitter(std::shared_ptr<Vkx::Device>   device,
                 std::vector<PointParticle>     particles,
                 std::shared_ptr<EmitterVolume> volume,
                 std::shared_ptr<Environment>   environment,
                 std::shared_ptr<Appearance>    appearance,
                 bool                           sorted);

    virtual ~PointEmitter() override;

    //! Initializes the emitter
    void initialize();

    //! Uninitializes the emitter
    void uninitialize();

    std::vector<PointParticle> &       particles()       { return particles_; }
    std::vector<PointParticle> const & particles() const { return particles_; }

    //! @name Overrides BasicEmitter
    //@{
    virtual void update(float dt) override;
    virtual void draw() const override;
    //@}

private:
    std::vector<PointParticle> particles_;
};

//! An Emitter that emits StreakParticles
//!
//! @ingroup	Emitters
//!

class StreakEmitter : public BasicEmitter
{
public:

    //! Constructor.
    StreakEmitter(std::shared_ptr<Vkx::Device>   device,
                  int                            n,
                  std::shared_ptr<EmitterVolume> volume,
                  std::shared_ptr<Environment>   environment,
                  std::shared_ptr<Appearance>    appearance,
                  bool                           sorted);

    //! Constructor.
    StreakEmitter(std::shared_ptr<Vkx::Device>   device,
                  std::vector<StreakParticle>    particles,
                  std::shared_ptr<EmitterVolume> volume,
                  std::shared_ptr<Environment>   environment,
                  std::shared_ptr<Appearance>    appearance,
                  bool                           sorted);

    virtual ~StreakEmitter() override;

    //! Initializes the emitter
    void initialize();

    //! Uninitializes the emitter
    void uninitialize();

    std::vector<StreakParticle> &       particles()       { return particles_; }
    std::vector<StreakParticle> const & particles() const { return particles_; }

    //! @name Overrides BasicEmitter
    //@{
    virtual void update(float dt) override;
    virtual void draw() const override;
    //@}

private:
    std::vector<StreakParticle> particles_;
};

//! An Emitter that emits TexturedParticles
//!
//! @ingroup	Emitters
//!

class TexturedEmitter : public BasicEmitter
{
public:

    //! Constructor.
    TexturedEmitter(std::shared_ptr<Vkx::Device>   device,
                    int                            n,
                    std::shared_ptr<EmitterVolume> volume,
                    std::shared_ptr<Environment>   environment,
                    std::shared_ptr<Appearance>    appearance,
                    bool                           sorted);

    //! Constructor.
    TexturedEmitter(std::shared_ptr<Vkx::Device>   device,
                    std::vector<TexturedParticle>  particles,
                    std::shared_ptr<EmitterVolume> volume,
                    std::shared_ptr<Environment>   environment,
                    std::shared_ptr<Appearance>    appearance,
                    bool                           sorted);

    virtual ~TexturedEmitter() override;

    //! Initializes the emitter
    void initialize();

    //! Uninitializes the emitter
    void uninitialize();

    std::vector<TexturedParticle> &       particles()       { return particles_; }
    std::vector<TexturedParticle> const & particles() const { return particles_; }

    //! @name Overrides BasicEmitter
    //@{
    virtual void update(float dt) override;
    virtual void draw() const override;
    //@}

private:
    std::vector<TexturedParticle> particles_;
    std::shared_ptr<Vkx::LocalBuffer> pIB_;                      // Index buffer for particles
};

//! An Emitter that emits SphereParticles
//!
//! @ingroup	Emitters
//!

class SphereEmitter : public BasicEmitter
{
public:

    //! Constructor.
    SphereEmitter(std::shared_ptr<Vkx::Device>   device,
                  int                            n,
                  std::shared_ptr<EmitterVolume> volume,
                  std::shared_ptr<Environment>   environment,
                  std::shared_ptr<Appearance>    appearance,
                  bool                           sorted);

    //! Constructor.
    SphereEmitter(std::shared_ptr<Vkx::Device>   device,
                  std::vector<SphereParticle>    particles,
                  std::shared_ptr<EmitterVolume> volume,
                  std::shared_ptr<Environment>   environment,
                  std::shared_ptr<Appearance>    appearance,
                  bool                           sorted);

    virtual ~SphereEmitter() override;

    //! Initializes the emitter
    void initialize();

    //! Uninitializes the emitter
    void uninitialize();

    std::vector<SphereParticle> &       particles()       { return particles_; }
    std::vector<SphereParticle> const & particles() const { return particles_; }

    //! @name Overrides BasicEmitter
    //@{
    virtual void update(float dt) override;
    virtual void draw() const override;
    //@}

private:
    std::vector<SphereParticle> particles_;
};
} // namespace Confetti

#endif // !defined(CONFETTI_EMITTER_H)
