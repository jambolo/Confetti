/** @file *//********************************************************************************************************

                                                   ParticleSystem.h

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/ParticleSystem.h#10 $

    $NoKeywords: $

********************************************************************************************************************/

#pragma once

#include <vector>

struct IDirect3DDevice9;

namespace Confetti
{
class BasicEmitter;
class Appearance;
class EmitterVolume;
class Environment;

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! The particle system
//
//! This class updates and draws particles associated with a set of emitters using a set of appearances
//! and environments.

class ParticleSystem
{
public:

    //! Default constructor
    ParticleSystem(IDirect3DDevice9 * pD3dDevice);

    //! Destructor
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

    typedef std::vector<BasicEmitter *>   EmitterList;
    typedef std::vector<Environment *>    EnvironmentList;
    typedef std::vector<Appearance *>     AppearanceList;

    IDirect3DDevice9 * m_pD3dDevice;

    EmitterList m_Emitters;                                     // Active emitters
    EnvironmentList m_Environments;                             // Active environments
    AppearanceList m_Appearances;                               // Active appearances
};
} // namespace Confetti
