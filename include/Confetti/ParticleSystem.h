#pragma once

#if !defined(CONFETTI_PARTICLESYSTEM_H)
#define CONFETTI_PARTICLESYSTEM_H

#include <vector>

struct ID3D11Device;

namespace Confetti
{
class BasicEmitter;
class Appearance;
class EmitterVolume;
class Environment;

//! The particle system
//!
//! This class updates and draws particles associated with a set of emitters using a set of appearances
//! and environments.

class ParticleSystem
{
public:

    //! Constructor.
    ParticleSystem(ID3D11Device * pD3dDevice);

    //! Destructor.
    ~ParticleSystem();

    //! Registers/unregisters an emitter (Unregister returns true if the emitter was found)
    void Register(BasicEmitter * pEmitter);
    bool Unregister(BasicEmitter * pEmitter);

    //! Registers/unregisters an appearance (Unregister returns true if the appearance was found)
    void Register(Appearance * pAppearance);
    bool Unregister(Appearance * pAppearance);

    //! Registers/unregisters an environment (Unregister returns true if the environment was found)
    void Register(Environment * pEnvironment);
    bool Unregister(Environment * pEnvironment);

    //! Updates the system
    void Update(float dt);

    //! Draws all particles for all the emitters
    void Draw() const;

private:

    using EmitterList     = std::vector<BasicEmitter *>;
    using EnvironmentList = std::vector<Environment *>;
    using AppearanceList  = std::vector<Appearance *>;

    ID3D11Device * pD3dDevice_;

    EmitterList emitters_;                                     // Active emitters
    EnvironmentList environments_;                             // Active environments
    AppearanceList appearances_;                               // Active appearances
};
} // namespace Confetti

#endif // !defined(CONFETTI_PARTICLESYSTEM_H)
