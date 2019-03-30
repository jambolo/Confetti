#pragma once

#if !defined(CONFETTI_BUILDER_H)
#define CONFETTI_BUILDER_H

#include <Confetti/Configuration.h>
#include <Confetti/Environment.h>
#include <memory>
#include <random>
#include <Vkx/Random.h>

namespace Vkx
{
    class Camera;
    class Device;
    class Material;
    class Texture;
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

    //! Returns a new particle system build using the supplied configuration
    std::shared_ptr<ParticleSystem> buildParticleSystem(Configuration const &        configuration,
                                                        std::shared_ptr<Vkx::Device> device,
                                                        Vkx::Camera const *          camera);

    //! Builds an emitter.
    std::shared_ptr<BasicEmitter> buildEmitter(Configuration::Emitter const & configuration,
                                               std::shared_ptr<Vkx::Device>   device);

    //! Builds an appearance.
    std::shared_ptr<Appearance> buildAppearance(Configuration::Appearance const & configuration,
                                                Vkx::Camera const *               pCamera);

    //! Builds an environment.
    std::shared_ptr<Environment> buildEnvironment(Configuration::Environment const & configuration);

    //! Builds an bounce plane list.
    Environment::BouncePlaneList buildBouncePlaneList(Configuration::BouncePlaneList const & configuration);

    //! Builds a clip plane list.
    Environment::ClipPlaneList buildClipPlaneList(Configuration::ClipPlaneList const & configuration);

    //! Builds an emitter volume.
    std::shared_ptr<EmitterVolume> buildEmitterVolume(Configuration::EmitterVolume const & configuration);

    //! Builds the particles for a point emitter.
    std::vector<PointParticle> buildPointParticles(int                            n,
                                                   Configuration::Emitter const & emitterConfiguration,
                                                   EmitterVolume const &          volume,
                                                   Environment const &            environment,
                                                   Appearance const &             appearance);

    //! Builds the particles for a point emitter.
    std::vector<PointParticle> buildPointParticles(Configuration::Emitter::ParticleVector const & configurations);

    //! Builds the particles for a streak emitter.
    std::vector<StreakParticle> buildStreakParticles(int                            n,
                                                     Configuration::Emitter const & emitterConfiguration,
                                                     EmitterVolume const &          volume,
                                                     Environment const &            environment,
                                                     Appearance const &             appearance);

    //! Builds the particles for a streak emitter.
    std::vector<StreakParticle> buildStreakParticles(Configuration::Emitter::ParticleVector const & configurations);

    //! Builds the particles for a textured emitter.
    std::vector<TexturedParticle> buildTexturedParticles(int                            n,
                                                         Configuration::Emitter const & emitterConfiguration,
                                                         EmitterVolume const &          volume,
                                                         Environment const &            environment,
                                                         Appearance const &             appearance);

    //! Builds the particles for a textured emitter.
    std::vector<TexturedParticle> buildTexturedParticles(Configuration::Emitter::ParticleVector const & configurations);

    //! Builds the particles for a sphere emitter.
    std::vector<SphereParticle> buildSphereParticles(int                            n,
                                                     Configuration::Emitter const & emitterConfiguration,
                                                     EmitterVolume const &          volume,
                                                     Environment const &            environment,
                                                     Appearance const &             appearance);

    //! Builds the particles for a sphere emitter.
    std::vector<SphereParticle> buildSphereParticles(Configuration::Emitter::ParticleVector const & configurations);

    //! Builds the particles for an emitter emitter
    std::vector<EmitterParticle> buildEmitterParticles(int                            n,
                                                       Configuration::Emitter const & emitterConfiguration,
                                                       EmitterVolume const &          volume,
                                                       Environment const &            environment,
                                                       Appearance const &             appearance);

    //! Builds the particles for an emitter emitter
    std::vector<EmitterParticle> buildEmitterParticles(Configuration::Emitter::ParticleVector const & configurations);

    //! Returns the named emitter or nullptr if not found.
    std::shared_ptr<BasicEmitter> findEmitter(std::string const & name);

    //! Returns the named emitter volume or nullptr if not found.
    std::shared_ptr<EmitterVolume> findEmitterVolume(std::string const & name);

    //! Returns the named environment or nullptr if not found.
    std::shared_ptr<Environment> findEnvironment(std::string const & name);

    //! Returns the named appearance or nullptr if not found.
    std::shared_ptr<Appearance> findAppearance(std::string const & name);

    //! Returns the named bound plane list or nullptr if not found.
    std::shared_ptr<Environment::BouncePlaneList> findBouncePlaneList(std::string const & name);

    //! Returns the named clip plane list or nullptr if not found.
    std::shared_ptr<Environment::ClipPlaneList> findClipPlaneList(std::string const & name);

    //! Returns the named material or nullptr if not found.
    std::shared_ptr<Vkx::Material> findMaterial(std::string const & name);

    //! Returns the named texture or nullptr if not found.
    std::shared_ptr<Vkx::Texture> findTexture(std::string const & name);

private:

    using EmitterMap         = std::map<std::string, std::shared_ptr<BasicEmitter>>;
    using EmitterVolumeMap   = std::map<std::string, std::shared_ptr<EmitterVolume>>;
    using EnvironmentMap     = std::map<std::string, std::shared_ptr<Environment>>;
    using AppearanceMap      = std::map<std::string, std::shared_ptr<Appearance>>;
    using BouncePlaneListMap = std::map<std::string, std::shared_ptr<Environment::BouncePlaneList>>;
    using ClipPlaneListMap   = std::map<std::string, std::shared_ptr<Environment::ClipPlaneList>>;
    using TextureMap         = std::map<std::string, std::shared_ptr<Vkx::Texture>>;
    using MaterialMap        = std::map<std::string, std::shared_ptr<Vkx::Material>>;

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
