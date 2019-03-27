#pragma once

#if !defined(CONFETTI_BUILDER_H)
#define CONFETTI_BUILDER_H

#include <Confetti/Configuration.h>
// #include <Confetti/Environment.h>
#include <memory>
#include <random>
#include <Vkx/Random.h>

namespace Vkx
{
    class Camera;
    class Texture;
    class Material;
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

//! A class that builds and maintains Confetti objects.

class Builder
{
public:

    //! Constructor.
    Builder(std::minstd_rand & rng);

    //! Destructor.
    virtual ~Builder();

    //! Returns a new particle system build using the supplied configuration
    std::unique_ptr<ParticleSystem> buildParticleSystem(Configuration const & configuration,
                                                        Vkx::Camera const *   pCamera);

    //! Builds an emitter.
    BasicEmitter * buildEmitter(Configuration::Emitter const & configuration);

    //! Builds an appearance.
    Appearance * buildAppearance(Configuration::Appearance const & configuration,
                                 Vkx::Camera const *               pCamera);

    //! Builds an emitter volume.
    EmitterVolume * buildEmitterVolume(Configuration::EmitterVolume const & configuration);

    //! Builds an environment.
    Environment * buildEnvironment(Configuration::Environment const & configuration);

    //! Builds an bounce plane list.
    Environment::BouncePlaneList * buildBouncePlaneList(Configuration::BouncePlaneList const & configuration);

    //! Builds a clip plane list.
    Environment::ClipPlaneList * buildClipPlaneList(Configuration::ClipPlaneList const & configuration);

    //! Builds the particles for a point emitter.
    std::vector<PointParticle> Builder::buildPointParticles(
        int                                            n,
        Configuration::Emitter::ParticleVector const & configurations,
        Configuration::Emitter const &                 emitterConfiguration,
        EmitterVolume const &                          volume,
        Environment const &                            environment,
        Appearance const &                             appearance);

    //! Builds the particles for a streak emitter.
    std::vector<StreakParticle> Builder::buildStreakParticles(
        int                                            n,
        Configuration::Emitter::ParticleVector const & configurations,
        Configuration::Emitter const &                 emitterConfiguration,
        EmitterVolume const &                          volume,
        Environment const &                            environment,
        Appearance const &                             appearance);

    //! Builds the particles for a textured emitter.
    std::vector<TexturedParticle> Builder::buildTexturedParticles(
        int                                            n,
        Configuration::Emitter::ParticleVector const & configurations,
        Configuration::Emitter const &                 emitterConfiguration,
        EmitterVolume const &                          volume,
        Environment const &                            environment,
        Appearance const &                             appearance);

    //! Builds the particles for a sphere emitter.
    std::vector<SphereParticle> Builder::buildSphereParticles(
        int                                            n,
        Configuration::Emitter::ParticleVector const & configurations,
        Configuration::Emitter const &                 emitterConfiguration,
        EmitterVolume const &                          volume,
        Environment const &                            environment,
        Appearance const &                             appearance);

//! Builds the particles for an emitter emitter
    std::vector<EmitterParticle> Builder::buildEmitterParticles(
        int                                            n,
        Configuration::Emitter::ParticleVector const & configurations,
        Configuration::Emitter const &                 emitterConfiguration,
        EmitterVolume const &                          volume,
        Environment const &                            environment,
        Appearance const &                             appearance);

    //! Returns the named emitter or nullptr if not found.
    BasicEmitter * findEmitter(std::string const & name);

    //! Returns the named emitter volume or nullptr if not found.
    EmitterVolume * findEmitterVolume(std::string const & name);

    //! Returns the named environment or nullptr if not found.
    Environment * findEnvironment(std::string const & name);

    //! Returns the named appearance or nullptr if not found.
    Appearance * findAppearance(std::string const & name);

    //! Returns the named bound plane list or nullptr if not found.
    Environment::BouncePlaneList * findBouncePlaneList(std::string const & name);

    //! Returns the named clip plane list or nullptr if not found.
    Environment::ClipPlaneList * findClipPlaneList(std::string const & name);

    //! Returns the named material or nullptr if not found.
    Vkx::Material * findMaterial(std::string const & name);

    //! Returns the named texture or nullptr if not found.
    std::shared_ptr<Vkx::Texture> findTexture(std::string const & name);

private:

    using EmitterMap         = std::map<std::string, BasicEmitter *>;
    using EmitterVolumeMap   = std::map<std::string, EmitterVolume *>;
    using EnvironmentMap     = std::map<std::string, Environment *>;
    using AppearanceMap      = std::map<std::string, Appearance *>;
    using BouncePlaneListMap = std::map<std::string, Environment::BouncePlaneList *>;
    using ClipPlaneListMap   = std::map<std::string, Environment::ClipPlaneList *>;
    using TextureMap         = std::map<std::string, std::shared_ptr<Vkx::Texture>>;
    using MaterialMap        = std::map<std::string, Vkx::Material *>;

    EmitterMap::iterator         addEmitter(std::string const & name, BasicEmitter * pEmitter);
    EmitterVolumeMap::iterator   addEmitterVolume(std::string const & name, EmitterVolume * pVolume);
    EnvironmentMap::iterator     addEnvironment(std::string const & name, Environment * pEnvironment);
    AppearanceMap::iterator      addAppearance(std::string const & name, Appearance * pAppearance);
    BouncePlaneListMap::iterator addBouncePlaneList(std::string const & name, Environment::BouncePlaneList * pList);
    ClipPlaneListMap::iterator   addClipPlaneList(std::string const & name, Environment::ClipPlaneList * pList);
    MaterialMap::iterator        addMaterial(std::string const & name, Vkx::Material * pMaterial);
    TextureMap::iterator         addTexture(std::string const & name, std::shared_ptr<Vkx::Texture> pTexture);

    EmitterMap emitters_;                      //!< Active emitters
    EmitterVolumeMap emitterVolumes_;          //!< Active emitter volumes
    EnvironmentMap environments_;              //!< Active environments
    AppearanceMap appearances_;                //!< Active appearances
    BouncePlaneListMap bouncePlaneLists_;      //!< Active bounce plane lists
    ClipPlaneListMap clipPlaneLists_;          //!< Active clip plane lists
    TextureMap textures_;                      //!< Active textures
    MaterialMap materials_;                    //!< Active materials

    std::minstd_rand & rng_;
};
} // namespace Confetti

#endif // !defined(CONFETTI_BUILDER_H)
