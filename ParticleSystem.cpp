#include "ParticleSystem.h"

#include "Appearance.h"
#include "Configuration.h"
#include "Emitter.h"
#include "EmitterVolume.h"
#include "Environment.h"

#include <Vkx/Device.h>

#include <algorithm>

template <class List, typename T>
bool removeFromList(List & list, T * value)
{
    typename List::iterator new_end = std::remove_if(list.begin(),
                                            list.end(),
                                            [value] (std::shared_ptr<T> & entry) {
                                                return entry.get() == value;
                                            });
    bool found = new_end != list.end();
    if (found)
        list.erase(new_end, list.end());

    return found;
}

namespace Confetti
{
//! @param  device  Device to draw the particle system on

ParticleSystem::ParticleSystem(std::shared_ptr<Vkx::Device> device)
    : device_(device)
{
}

//! @param	emitter	The emitter to register.

void ParticleSystem::add(std::shared_ptr<BasicEmitter> emitter)
{
    emitters_.push_back(emitter);
}

//! @param	appearance		The appearance to register.

void ParticleSystem::add(std::shared_ptr<Appearance> appearance)
{
    appearances_.push_back(appearance);
}

//! @param	environment	The environment to register.

void ParticleSystem::add(std::shared_ptr<Environment> environment)
{
    environments_.push_back(environment);
}

//! @param	emitter	The emitter to unregister.
//!
//! @return     false, if the emitter was not registered

bool ParticleSystem::remove(BasicEmitter * emitter)
{
    return removeFromList(emitters_, emitter);
}

//! @param	appearance		The appearance to unregister.
//!
//! @return     false, if the appearance was not registered

bool ParticleSystem::remove(Appearance * appearance)
{
    return removeFromList(appearances_, appearance);
}

//! @param	environment	The environment to unregister.
//!
//! @return     false, if the appearance was not registered

bool ParticleSystem::remove(Environment * environment)
{
    return removeFromList(environments_, environment);
}

//! @param	dt		Amount of time (seconds) that has passed since the last update.

void ParticleSystem::update(float dt)
{
    // Update all the appearances

    for (std::shared_ptr<Appearance> appearance : appearances_)
    {
        appearance->update(dt);
    }

    // Update all the environments

    for (auto const & environment : environments_)
    {
        environment->update(dt);
    }

    // Update all the emitters

    for (auto const & emitter : emitters_)
    {
        if (emitter->enabled())
            emitter->update(dt);
    }
}

void ParticleSystem::draw() const
{
    // For each emitter, draw all its particles

    for (auto const & emitter : emitters_)
    {
        if (emitter->enabled())
            emitter->draw();
    }
}
} // namespace Confetti
