#pragma once

#if !defined(CONFETTI_BUILDER_H)
#define CONFETTI_BUILDER_H

#include "Configuration.h"
#include "Environment.h"
#include <Dxx/Random.h>
#include <memory>
#include <random>

struct ID3D11Device;
struct ID3D11Texture2D;
struct _D3DMATERIAL11;
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

//! A class that builds and maintains Confetti objects

class Builder
{
public:

    //! Constructor.
    Builder(std::minstd_rand & rng);

    //! Destructor
    virtual ~Builder();

    //! Returns a new particle system build using the supplied configuration
    std::unique_ptr<ParticleSystem> BuildParticleSystem(Configuration const & configuration,
                                                      ID3D11Device *        pD3dDevice,
        ID3D11DeviceContext *             pD3dContext,
        Dxx::Camera const *   pCamera);

    //! Builds an emitter and returns a reference
    BasicEmitter * BuildEmitter(Configuration::Emitter const & configuration,
                                ID3D11Device *                 pD3dDevice);

    //! Builds an appearance and returns a reference
    Appearance * BuildAppearance(Configuration::Appearance const & configuration,
                                 ID3D11Device *                    pD3dDevice,
                                 ID3D11DeviceContext *             pD3dContext,
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
    std::unique_ptr<PointParticle> Builder::BuildPointParticles(
        int                                            n,
        Configuration::Emitter::ParticleVector const & configurations,
        Configuration::Emitter const &                 emitterConfiguration,
        EmitterVolume const &                          volume,
        Environment const &                            environment,
        Appearance const &                             appearance);

    //! Builds the particles for a streak emitter
    std::unique_ptr<StreakParticle> Builder::BuildStreakParticles(
        int                                            n,
        Configuration::Emitter::ParticleVector const & configurations,
        Configuration::Emitter const &                 emitterConfiguration,
        EmitterVolume const &                          volume,
        Environment const &                            environment,
        Appearance const &                             appearance);

    //! Builds the particles for a textured emitter
    std::unique_ptr<TexturedParticle> Builder::BuildTexturedParticles(
        int                                            n,
        Configuration::Emitter::ParticleVector const & configurations,
        Configuration::Emitter const &                 emitterConfiguration,
        EmitterVolume const &                          volume,
        Environment const &                            environment,
        Appearance const &                             appearance);

    //! Builds the particles for a sphere emitter
    std::unique_ptr<SphereParticle> Builder::BuildSphereParticles(
        int                                            n,
        Configuration::Emitter::ParticleVector const & configurations,
        Configuration::Emitter const &                 emitterConfiguration,
        EmitterVolume const &                          volume,
        Environment const &                            environment,
        Appearance const &                             appearance);

//	//! Builds the particles for an emitter emitter
//	std::unique_ptr< EmitterParticle > Builder::BuildEmitterParticles(
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

//     //! Returns the named material or 0 if not found
//     _D3DMATERIAL11 * FindMaterial(std::string const & name);
// 
    //! Returns the named texture or 0 if not found
    ID3D11Texture2D * FindTexture(std::string const & name);

private:

    using EmitterMap         = std::map<std::string, BasicEmitter *>;
    using EmitterVolumeMap   = std::map<std::string, EmitterVolume *>;
    using EnvironmentMap     = std::map<std::string, Environment *>;
    using AppearanceMap      = std::map<std::string, Appearance *>;
    using BouncePlaneListMap = std::map<std::string, Environment::BouncePlaneList *>;
    using ClipPlaneListMap   = std::map<std::string, Environment::ClipPlaneList *>;
    using TextureMap         = std::map<std::string, ID3D11Texture2D *>;
//    using MaterialMap        = std::map<std::string, _D3DMATERIAL11 *>;

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

//     //! Adds the named material to the material map and returns a reference to its entry
//     MaterialMap::iterator AddMaterial(std::string const & name, _D3DMATERIAL11 * pMaterial);
// 
    //! Adds the named texture to the texture map and returns a reference to its entry
    TextureMap::iterator AddTexture(std::string const & name, ID3D11Texture2D * pTexture);

    EmitterMap emitters_;                      //!< Active emitters
    EmitterVolumeMap emitterVolumes_;          //!< Active emitter volumes
    EnvironmentMap environments_;              //!< Active environments
    AppearanceMap appearances_;                //!< Active appearances
    BouncePlaneListMap bouncePlaneLists_;      //!< Active bounce plane lists
    ClipPlaneListMap clipPlaneLists_;          //!< Active clip plane lists
    TextureMap textures_;                      //!< Active textures
//    MaterialMap materials_;                    //!< Active materials

    // Random number generators

    std::minstd_rand & rng_;
};
} // namespace Confetti

#endif // !defined(CONFETTI_BUILDER_H)
