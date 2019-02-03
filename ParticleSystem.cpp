#include "ParticleSystem.h"

#include "Appearance.h"
#include "Configuration.h"
#include "Emitter.h"
#include "EmitterVolume.h"
#include "Environment.h"

#include "Wx/Wx.h"

#include <d3d11.h>

namespace Confetti
{
ParticleSystem::ParticleSystem(ID3D11Device * pD3dDevice)
    : pD3dDevice_(pD3dDevice)
{
    pD3dDevice_->AddRef();
}

ParticleSystem::~ParticleSystem()
{
    Wx::SafeRelease(pD3dDevice_);
}

//! @param	pEmitter	The Emitter to register.
//!
//! @note	The particle system does not "own" the Emitter.

void ParticleSystem::Register(BasicEmitter * pEmitter)
{
    emitters_.push_back(pEmitter);
}

//!
//! @param	pEmitter	The Emitter to unregister.

bool ParticleSystem::Unregister(BasicEmitter * pEmitter)
{
    EmitterList::iterator const i = std::find(emitters_.begin(), emitters_.end(), pEmitter);

    if (i != emitters_.end())
        emitters_.erase(i);

    return i != emitters_.end();
}

//! @param	pAppearance		The Appearance to register.
//!
//! @note	The particle system does not "own" the Appearance.

void ParticleSystem::Register(Appearance * pAppearance)
{
    appearances_.push_back(pAppearance);
}

//!
//! @param	pAppearance		The Appearance to unregister.

bool ParticleSystem::Unregister(Appearance * pAppearance)
{
    AppearanceList::iterator const i = std::find(appearances_.begin(), appearances_.end(), pAppearance);

    if (i != appearances_.end())
        appearances_.erase(i);

    return i != appearances_.end();
}

//! @param	pEnvironment	The Environment to register.
//!
//! @note	The particle system does not "own" the Environment.

void ParticleSystem::Register(Environment * pEnvironment)
{
    environments_.push_back(pEnvironment);
}

//!
//! @param	pEnvironment	The Environment to unregister.

bool ParticleSystem::Unregister(Environment * pEnvironment)
{
    EnvironmentList::iterator const i = std::find(environments_.begin(), environments_.end(), pEnvironment);

    if (i != environments_.end())
        environments_.erase(i);

    return i != environments_.end();
}

//!
//! @param	dt		Amount of time that has passed since the last update.

void ParticleSystem::Update(float dt)
{
    // Update all the appearances

    for (Appearance * pAppearance : appearances_)
    {
        pAppearance->update(dt);
    }

    // Update all the environments

    for (Environment * pEnvironment : environments_)
    {
        pEnvironment->Update(dt);
    }

    // Update all the emitters

    for (BasicEmitter * pEmitter : emitters_)
    {
        if (pEmitter && pEmitter->enabled())
            pEmitter->update(dt);
    }
}

void ParticleSystem::Draw() const
{
    // For each emitter, draw all its particles

    for (BasicEmitter * pEmitter : emitters_)
    {
        if (pEmitter && pEmitter->enabled())
            pEmitter->draw();
    }
}
} // namespace Confetti
