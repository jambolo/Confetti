#pragma once

#if !defined(CONFETTI_PARTICLESYSTEM_H)
#define CONFETTI_PARTICLESYSTEM_H

#include <vector>

namespace Vkx { class Device; }

namespace Confetti
{
class BasicEmitter;
class Appearance;
class EmitterVolume;
class Environment;

//! The particle system.
//!
//! This class updates and draws particles associated with a set of emitters using a set of appearances and environments.

class ParticleSystem
{
public:

    //! Constructor.
    ParticleSystem(std::shared_ptr<Vkx::Device> device);

    //@{
    //! Registers a component.
    void add(std::shared_ptr<BasicEmitter> pemitter);
    void add(std::shared_ptr<Appearance> appearance);
    void add(std::shared_ptr<Environment> environment);
    //@}

    //@{
    //! Unregisters a component.
    bool remove(BasicEmitter * emitter);
    bool remove(Appearance * appearance);
    bool remove(Environment * environment);
    //@}

    //! Updates the system.
    void update(float dt);

    //! Draws all particles for all the emitters.
    void draw() const;

private:

    using EmitterList     = std::vector<std::shared_ptr<BasicEmitter>>;
    using EnvironmentList = std::vector<std::shared_ptr<Environment>>;
    using AppearanceList  = std::vector<std::shared_ptr<Appearance>>;

    std::shared_ptr<Vkx::Device> device_;
    EmitterList emitters_;                  // Active emitters
    EnvironmentList environments_;          // Active environments
    AppearanceList appearances_;            // Active appearances
};
} // namespace Confetti

#endif // !defined(CONFETTI_PARTICLESYSTEM_H)
