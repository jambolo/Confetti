#pragma once

#if !defined(CONFETTI_PARTICLE_H)
#define CONFETTI_PARTICLE_H

namespace Vkx
{
    class Device;
}

namespace Confetti
{
class BasicEmitter;

//! A particle base class.
//!
//! @ingroup	Particles
//!
//! A Particle is a point with a lifetime, age, position, and velocity. It has no size, shape, orientation, color,
//! or texture.
//!
//! @note This is an abstract base class. You must derive from this class in order to use it.

class Particle
{
public:

    //! Constructor.
    Particle() = default;

    //! Constructor.
    Particle(BasicEmitter const * pEmitter,
             float                lifetime,
             float                age,
             glm::vec3 const &    position,
             glm::vec3 const &    velocity,
             glm::vec4 const &    color);

    //! Destructor.
    virtual ~Particle() = default;

    //! Updates the particle. Returns true if the particle was reborn.
    virtual bool Update(float dt);

    //! Draws the particle.
    //!
    //!
    //! @note	This method must be overridden.
    virtual void Draw(std::shared_ptr<Vkx::Device> device) const = 0;

    //! Binds to an emitter.
    void Bind(BasicEmitter * pEmitter) { pEmitter_ = pEmitter; }

    //! Returns the age of the particle.
    float GetAge() const { return age_; }

    //! Returns the particle's current position.
    glm::vec3 GetPosition() const { return position_; }

    //! Returns the particle's current velocity.
    glm::vec3 GetVelocity() const { return velocity_; }

    //! Returns the particle's current velocity.
    glm::vec4 GetColor() const { return color_; }

protected:

    //! Initializes after using the default constructor.
    void Initialize(float             lifetime,
                    float             age,
                    glm::vec3 const & position,
                    glm::vec3 const & velocity,
                    glm::vec4 const & color);

    BasicEmitter const * pEmitter_;            //!< This particle's emitter

    // Age data

    float lifetime_;    //!< Max age
    float age_;         //!< Current age

    // Motion data

    glm::vec3 initialPosition_; //!< Position at birth relative to emitter
    glm::vec3 initialVelocity_; //!< Velocity at birth relative to emitter

    glm::vec3 position_;        //!< Current position
    glm::vec3 velocity_;        //!< Current velocity

    // Appearance data

    glm::vec4 initialColor_;    //!< Color at birth
    glm::vec4 color_;           //!< Current color
};
} // namespace Confetti

#endif // !defined(CONFETTI_PARTICLE_H)
