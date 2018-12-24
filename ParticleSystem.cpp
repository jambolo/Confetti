/** @file *//********************************************************************************************************

                                                  ParticleSystem.cpp

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/ParticleSystem.cpp#11 $

    $NoKeywords: $

********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "ParticleSystem.h"

#include "Appearance.h"
#include "Configuration.h"
#include "Emitter.h"
#include "EmitterVolume.h"
#include "Environment.h"

namespace Confetti
{
/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

ParticleSystem::ParticleSystem(IDirect3DDevice9 * pD3dDevice)
    : m_pD3dDevice(pD3dDevice)
{
    m_pD3dDevice->AddRef();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

ParticleSystem::~ParticleSystem()
{
    Wx::SafeRelease(m_pD3dDevice);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	pEmitter	The Emitter to register.
//!
//! @note	The particle system does not "own" the Emitter.

void ParticleSystem::Register(BasicEmitter * pEmitter)
{
    m_Emitters.push_back(pEmitter);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//!
//! @param	pEmitter	The Emitter to unregister.

bool ParticleSystem::Unregister(BasicEmitter * pEmitter)
{
    EmitterList::iterator const i = std::find(m_Emitters.begin(), m_Emitters.end(), pEmitter);

    if (i != m_Emitters.end())
        m_Emitters.erase(i);

    return i != m_Emitters.end();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	pAppearance		The Appearance to register.
//!
//! @note	The particle system does not "own" the Appearance.

void ParticleSystem::Register(Appearance * pAppearance)
{
    m_Appearances.push_back(pAppearance);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//!
//! @param	pAppearance		The Appearance to unregister.

bool ParticleSystem::Unregister(Appearance * pAppearance)
{
    AppearanceList::iterator const i = std::find(m_Appearances.begin(), m_Appearances.end(), pAppearance);

    if (i != m_Appearances.end())
        m_Appearances.erase(i);

    return i != m_Appearances.end();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	pEnvironment	The Environment to register.
//!
//! @note	The particle system does not "own" the Environment.

void ParticleSystem::Register(Environment * pEnvironment)
{
    m_Environments.push_back(pEnvironment);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//!
//! @param	pEnvironment	The Environment to unregister.

bool ParticleSystem::Unregister(Environment * pEnvironment)
{
    EnvironmentList::iterator const i = std::find(m_Environments.begin(), m_Environments.end(), pEnvironment);

    if (i != m_Environments.end())
        m_Environments.erase(i);

    return i != m_Environments.end();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//!
//! @param	dt		Amount of time that has passed since the last update.

void ParticleSystem::Update(float dt)
{
    // Update all the appearances

    for (AppearanceList::iterator ppApp = m_Appearances.begin(); ppApp != m_Appearances.end(); ++ppApp)
    {
        Appearance * const pAppearance = *ppApp;    // Convenience

        pAppearance->Update(dt);
    }

    // Update all the environments

    for (EnvironmentList::iterator ppEnv = m_Environments.begin(); ppEnv != m_Environments.end(); ++ppEnv)
    {
        Environment * const pEnvironment = *ppEnv;      // Convenience

        pEnvironment->Update(dt);
    }

    // Update all the emitters

    for (EmitterList::iterator ppEmitter = m_Emitters.begin(); ppEmitter != m_Emitters.end(); ++ppEmitter)
    {
        BasicEmitter * const pEmitter = *ppEmitter;         // Convenience

        if (pEmitter && pEmitter->IsEnabled())
            pEmitter->Update(dt);
    }
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void ParticleSystem::Draw() const
{
    // For each emitter, draw all its particles

    for (EmitterList::const_iterator ppE = m_Emitters.begin(); ppE != m_Emitters.end(); ++ppE)
    {
        BasicEmitter * const pEmitter = *ppE;       // Convenience

        if (pEmitter && pEmitter->IsEnabled())
            pEmitter->Draw();
    }
}
} // namespace Confetti
