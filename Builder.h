/** @file *//********************************************************************************************************

                                                      Builder.h

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Builder.h#5 $

    $NoKeywords: $

********************************************************************************************************************/

#pragma once

#include "Configuration.h"
#include "Environment.h"
#include <Dxx/Random.h>
#include <memory>

struct IDirect3DDevice9;
struct IDirect3DTexture9;
struct _D3DMATERIAL9;
namespace Dxx
{
    class Camera;
}

namespace Confetti
{
class ParticleSystem;
class BasicEmitter;
class Appearance;
class EmitterVolume;
class PointParticle;
class StreakParticle;
class TexturedParticle;
class SphereParticle;
class EmitterParticle;

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

//! A class that builds and maintains Confetti objects

class Builder
{
public:

    //! Constructor
    Builder(uint32 seed);

    //! Destructor
    virtual ~Builder();

    //! Returns a new particle system build using the supplied configuration
    std::auto_ptr<ParticleSystem> BuildParticleSystem(Configuration const & configuration,
                                                      IDirect3DDevice9 *    pD3dDevice,
                                                      Dxx::Camera const *   pCamera);

    //! Builds an emitter and returns a reference
    BasicEmitter * BuildEmitter(Configuration::Emitter const & configuration,
                                IDirect3DDevice9 *             pD3dDevice);

    //! Builds an appearance and returns a reference
    Appearance * BuildAppearance(Configuration::Appearance const & configuration,
                                 IDirect3DDevice9 *                pD3dDevice,
                                 Dxx::Camera const *               pCamera);

    //! Builds an emitter volume and returns a reference
    EmitterVolume * BuildEmitterVolume(Configuration::EmitterVolume const & configuration);

    //! Builds an environment and returns a reference
    Environment * BuildEnvironment(Configuration::Environment const & configuration);

    //! Builds an bounce plane list and returns a reference
    Environment::BouncePlaneList * BuildBouncePlaneList(Configuration::BouncePlaneList const & configuration);

    //! Builds clip plane list and returns a reference
    Environment::ClipPlaneList * BuildClipPlaneList(Configuration::ClipPlaneList const & configuration);

    //! Builds the particles for a point emitter
    std::auto_ptr<PointParticle> Builder::BuildPointParticles(
        int                                            n,
        Configuration::Emitter::ParticleVector const & configurations,
        Configuration::Emitter const &                 emitterConfiguration,
        EmitterVolume const &                          volume,
        Environment const &                            environment,
        Appearance const &                             appearance);

    //! Builds the particles for a streak emitter
    std::auto_ptr<StreakParticle> Builder::BuildStreakParticles(
        int                                            n,
        Configuration::Emitter::ParticleVector const & configurations,
        Configuration::Emitter const &                 emitterConfiguration,
        EmitterVolume const &                          volume,
        Environment const &                            environment,
        Appearance const &                             appearance);

    //! Builds the particles for a textured emitter
    std::auto_ptr<TexturedParticle> Builder::BuildTexturedParticles(
        int                                            n,
        Configuration::Emitter::ParticleVector const & configurations,
        Configuration::Emitter const &                 emitterConfiguration,
        EmitterVolume const &                          volume,
        Environment const &                            environment,
        Appearance const &                             appearance);

    //! Builds the particles for a sphere emitter
    std::auto_ptr<SphereParticle> Builder::BuildSphereParticles(
        int                                            n,
        Configuration::Emitter::ParticleVector const & configurations,
        Configuration::Emitter const &                 emitterConfiguration,
        EmitterVolume const &                          volume,
        Environment const &                            environment,
        Appearance const &                             appearance);

//	//! Builds the particles for an emitter emitter
//	std::auto_ptr< EmitterParticle > Builder::BuildEmitterParticles(
//											int n,
//											Configuration::Emitter::ParticleVector const & configurations,
//											Configuration::Emitter const & emitterConfiguration,
//											EmitterVolume const & volume,
//											Environment const & environment,
//											Appearance const & appearance );

    //! Returns the named emitter or 0 if not found
    BasicEmitter * FindEmitter(std::string const & name);

    //! Returns the named emitter volume or 0 if not found
    EmitterVolume * FindEmitterVolume(std::string const & name);

    //! Returns the named environment or 0 if not found
    Environment * FindEnvironment(std::string const & name);

    //! Returns the named appearance or 0 if not found
    Appearance * FindAppearance(std::string const & name);

    //! Returns the named bound plane list or 0 if not found
    Environment::BouncePlaneList * FindBouncePlaneList(std::string const & name);

    //! Returns the named clip plane list or 0 if not found
    Environment::ClipPlaneList * FindClipPlaneList(std::string const & name);

    //! Returns the named material or 0 if not found
    _D3DMATERIAL9 * FindMaterial(std::string const & name);

    //! Returns the named texture or 0 if not found
    IDirect3DTexture9 * FindTexture(std::string const & name);

private:

    typedef std::map<std::string, BasicEmitter *>                 EmitterMap;
    typedef std::map<std::string, EmitterVolume *>                EmitterVolumeMap;
    typedef std::map<std::string, Environment *>                  EnvironmentMap;
    typedef std::map<std::string, Appearance *>                   AppearanceMap;
    typedef std::map<std::string, Environment::BouncePlaneList *> BouncePlaneListMap;
    typedef std::map<std::string, Environment::ClipPlaneList *>   ClipPlaneListMap;
    typedef std::map<std::string, IDirect3DTexture9 *>            TextureMap;
    typedef std::map<std::string, _D3DMATERIAL9 *>                MaterialMap;

    //! Adds the named emitter to the emitter map and returns a reference to its entry
    EmitterMap::iterator AddEmitter(std::string const & name, BasicEmitter * pEmitter);

    //! Adds the named emitter volume to the emitter volume map and returns a reference to its entry
    EmitterVolumeMap::iterator AddEmitterVolume(std::string const & name, EmitterVolume * pVolume);

    //! Adds the named environment to the environment map and returns a reference to its entry
    EnvironmentMap::iterator AddEnvironment(std::string const & name, Environment * pEnvironment);

    //! Adds the named appearance to the appearance map and returns a reference to its entry
    AppearanceMap::iterator AddAppearance(std::string const & name, Appearance * pAppearance);

    //! Adds the named bounce plane list to the bounce plane list map and returns a reference to its entry
    BouncePlaneListMap::iterator AddBouncePlaneList(std::string const & name, Environment::BouncePlaneList * pList);

    //! Adds the named clip plane list to the clip plane list map and returns a reference to its entry
    ClipPlaneListMap::iterator AddClipPlaneList(std::string const & name, Environment::ClipPlaneList * pList);

    //! Adds the named material to the material map and returns a reference to its entry
    MaterialMap::iterator AddMaterial(std::string const & name, _D3DMATERIAL9 * pMaterial);

    //! Adds the named texture to the texture map and returns a reference to its entry
    TextureMap::iterator AddTexture(std::string const & name, IDirect3DTexture9 * pTexture);

    EmitterMap m_Emitters;                      //!< Active emitters
    EmitterVolumeMap m_EmitterVolumes;          //!< Active emitter volumes
    EnvironmentMap m_Environments;              //!< Active environments
    AppearanceMap m_Appearances;                //!< Active appearances
    BouncePlaneListMap m_BouncePlaneLists;      //!< Active bounce plane lists
    ClipPlaneListMap m_ClipPlaneLists;          //!< Active clip plane lists
    TextureMap m_Textures;                      //!< Active textures
    MaterialMap m_Materials;                    //!< Active materials

    // Random number generators

    Random m_Rng;
    RandomFloat m_FRng;
    Dxx::RandomDirection m_DRng;
    Dxx::RandomOrientation m_ORng;
};
} // namespace Confetti
