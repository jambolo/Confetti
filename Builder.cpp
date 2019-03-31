#include "Builder.h"

#include "Appearance.h"
#include "Configuration.h"
#include "Emitter.h"
// #include "EmitterParticle.h"
#include "EmitterVolume.h"
#include "Environment.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "PointParticle.h"
#include "SphereParticle.h"
#include "StreakParticle.h"
#include "TexturedParticle.h"

#include <Vkx/Random.h>

#include <glm/glm.hpp>

#include <memory>
#include <random>

namespace Confetti
{
Builder::Builder(std::minstd_rand & rng)
    : rng_(rng)
{
    // Nothing to do
}

std::shared_ptr<ParticleSystem> Builder::buildParticleSystem(Configuration const &        configuration,
                                                             std::shared_ptr<Vkx::Device> device,
                                                             Vkx::Camera const *          pCamera)
{
    // Build the surface lists used by the emitters

    for (auto const & p : configuration.surfaceLists_)
    {
        buildSurfaceList(p.second);
    }

    // Build the clip plane lists used by the emitters

    for (auto const & p : configuration.clipperLists_)
    {
        buildClipperList(p.second);
    }

    // Build the emitter volumes used by the emitters

    for (auto const & v : configuration.emitterVolumes_)
    {
        buildEmitterVolume(v.second);
    }

    std::shared_ptr<ParticleSystem> system = std::make_shared<ParticleSystem>(device);

    // Build the environments

    for (auto const & e : configuration.environments_)
    {
        std::shared_ptr<Environment> environment = buildEnvironment(e.second);
        if (environment)
            system->add(environment);
    }

    // Build the appearances

    for (auto const & a : configuration.appearances_)
    {
        std::shared_ptr<Appearance> appearance = buildAppearance(a.second, pCamera);
        if (appearance)
            system->add(appearance);
    }

    // Build the emitters

    for (auto const & e : configuration.emitters_)
    {
        std::shared_ptr<BasicEmitter> emitter = buildEmitter(e.second, device);
        if (emitter)
            system->add(emitter);
    }

    return system;
}

std::shared_ptr<BasicEmitter> Builder::buildEmitter(Configuration::Emitter const & configuration,
                                                    std::shared_ptr<Vkx::Device>   device)
{
    // Prevent duplicate entries

    if (findEmitter(configuration.name_))
        return std::shared_ptr<BasicEmitter>();

    // class Configuration::Emitter
    // {
    // public:
    //
    //	typedef std::vector< Particle > ParticleVector;
    //
    //	std::string		name_;
    //	std::string		type_;
    //	std::string		volume_;
    //	std::string		environment_;
    //	std::string		appearance_;
    //	glm::vec4		position_;
    //	glm::vec4	orientation_;
    //	glm::vec4		velocity_;
    //	float			minSpeed_;
    //	float			maxSpeed_;
    //	int				count_;
    //	float			lifetime_;
    //	float			spread_;
    //	DWORD			color_;
    //	float			radius_;
    //	bool			sorted_;
    //	ParticleVector	particleVector_;
    // };

    std::shared_ptr<EmitterVolume> volume      = findEmitterVolume(configuration.volume_);
    std::shared_ptr<Environment>   environment = findEnvironment(configuration.environment_);
    std::shared_ptr<Appearance>    appearance  = findAppearance(configuration.appearance_);

    std::shared_ptr<BasicEmitter> emitter;

    if (configuration.type_ == "point")
    {
        std::vector<PointParticle> particles;
        if (!configuration.particleVector_.empty())
        {
            particles = buildPointParticles(configuration.particleVector_);
        }
        else
        {
            particles = buildPointParticles(configuration.count_,
                                            configuration,
                                            *volume,
                                            *environment,
                                            *appearance);
        }

        emitter = std::make_shared<PointEmitter>(device,
                                                 std::move(particles),
                                                 volume,
                                                 environment,
                                                 appearance,
                                                 configuration.sorted_);
    }
    else if (configuration.type_ == "streak")
    {
        std::vector<StreakParticle> particles;
        if (!configuration.particleVector_.empty())
        {
            particles = buildStreakParticles(configuration.particleVector_);
        }
        else
        {
            particles = buildStreakParticles(configuration.count_,
                                             configuration,
                                             *volume,
                                             *environment,
                                             *appearance);
        }
        emitter = std::make_shared<StreakEmitter>(device,
                                                  std::move(particles),
                                                  volume,
                                                  environment,
                                                  appearance,
                                                  configuration.sorted_);
    }
    else if (configuration.type_ == "textured")
    {
        std::vector<TexturedParticle> particles;
        if (!configuration.particleVector_.empty())
        {
            particles = buildTexturedParticles(configuration.particleVector_);
        }
        else
        {
            particles = buildTexturedParticles(configuration.count_,
                                               configuration,
                                               *volume,
                                               *environment,
                                               *appearance);
        }
        emitter = std::make_shared<TexturedEmitter>(device,
                                                    std::move(particles),
                                                    volume,
                                                    environment,
                                                    appearance,
                                                    configuration.sorted_);
    }
    else if (configuration.type_ == "sphere")
    {
        std::vector<SphereParticle> particles;
        if (!configuration.particleVector_.empty())
        {
            particles = buildSphereParticles(configuration.particleVector_);
        }
        else
        {
            particles = buildSphereParticles(configuration.count_,
                                             configuration,
                                             *volume,
                                             *environment,
                                             *appearance);
        }
        emitter = std::make_shared<SphereEmitter>(device,
                                                  std::move(particles),
                                                  volume,
                                                  environment,
                                                  appearance,
                                                  configuration.sorted_);
    }
//	else if ( configuration.type_ == "emitter" )
//	{
//        std::vector<EmitterParticle> particles;
//        if (!configuration.particleVector_.empty())
//        {
//            particles = buildEmitterParticles(configuration.particleVector_);
//        }
//        else
//        {
//            particles = buildEmitterParticles(configuration.count_,
//                                            configuration,
//                                              *volume,
//                                              *environment,
//                                              *appearance );
//        }
//        emitter = std::make_shared<EmitterEmitter>( device,
//                                   std::move(particles),
//                                                    pVolume,
//                                                    pEnvironment,
//                                                    pAppearance,
//                                                    configuration.sorted_ );
//    }

    // Manage the emitter

    if (emitter)
        emitters_.emplace(configuration.name_, emitter);

    return emitter;
}

std::shared_ptr<Appearance> Builder::buildAppearance(Configuration::Appearance const & configuration,
                                                     Vkx::Camera const *               camera)
{
    // Prevent duplicate entries

    if (findAppearance(configuration.name_))
        return nullptr;

    //	class Configuration::Appearance
    //	{
    //	public:
    //		std::string	name_;
    //		glm::vec4	colorChange_;
    //		float		radiusChange_;
    //		float		radialVelocity_;
    //		std::string	texture_;
    //		float		size_;
    //	};

    std::shared_ptr<Vkx::Texture>  texture;
    std::shared_ptr<Vkx::Material> material;

    // Create the texture (if specified)

    if (!configuration.texture_.empty())
    {
        // If the texture is not already created, then create it. Otherwise, use the existing one.

        texture = findTexture(configuration.texture_);
        if (!texture)
        {
            // Not working for now
            assert(false);
//            HRESULT hr;
//            hr = D3DXCreateTextureFromFile(device, configuration.texture_.c_str(), &pTexture);
//            assert_succeeded(hr);

            textures_.emplace(configuration.texture_, texture);
        }
    }

//     // Create the material (if specified)
//
//     if (!configuration.material_.empty())
//     {
//         // If the material is not already created, then create it. Otherwise, use the existing one.
//
//         material = findMaterial(configuration.material_);
//         if (!material)
//         {
//             // Not working for now
//             assert(false);
//             materials_.emplace(configuration.material_, material);
//         }
//     }

    std::shared_ptr<Appearance> appearance(new Appearance{ camera,
                                                           texture,
                                                           configuration.colorChange_,
                                                           configuration.radiusChange_,
                                                           configuration.radialVelocity_,
                                                           configuration.size_ });

    appearances_.emplace(configuration.name_, appearance);

    return appearance;
}

std::shared_ptr<Environment> Builder::buildEnvironment(Configuration::Environment const & configuration)
{
    // Prevent duplicate entries

    if (findEnvironment(configuration.name_))
        return nullptr;

    //	class Configuration::Environment
    //	{
    //	public:
    //		std::string	name_;
    //		glm::vec4	gravity_;
    //		glm::vec4	windVelocity_;
    //		float       gustiness_;
    //		float		airFriction_;
    //		std::string	surface_;
    //		std::string	clip_;
    //	};

    std::shared_ptr<Environment::SurfaceList> surfaceList = findSurfaceList(configuration.surface_);
    std::shared_ptr<Environment::ClipperList> clipperList = findClipperList(configuration.clip_);

    auto environment = std::make_shared<Environment>(configuration.gravity_,
                                                     configuration.airFriction_,
                                                     configuration.windVelocity_,
                                                     configuration.gustiness_,
                                                     *surfaceList,
                                                     *clipperList);
    environments_.emplace(configuration.name_, environment);
    return environment;
}

std::shared_ptr<Environment::SurfaceList> Builder::buildSurfaceList(Configuration::SurfaceList const & configuration)
{
//     // Prevent duplicate entries
//
//     if (findSurfaceList(configuration.name_))
//         return Environment::SurfaceList();

    //	class Configuration::SurfaceList
    //	{
    //	public:
    //		std::string					name_;
    //		std::vector< Surface >	surfaces_;
    //	};
    //
    //	class Configuration::Surface
    //	{
    //	public:
    //		glm::vec4	plane_;
    //		float		dampening_;
    //	};

    std::shared_ptr<Environment::SurfaceList> list(new Environment::SurfaceList);
    list->reserve(configuration.planes_.size());
    for (auto const & config : configuration.planes_)
    {
        list->emplace_back(config.plane_, config.dampening_);
    }

    surfaceLists_.emplace(configuration.name_, list);

    return list;
}

std::shared_ptr<Environment::ClipperList> Builder::buildClipperList(Configuration::ClipperList const & configuration)
{
    // Prevent duplicate entries

    if (findClipperList(configuration.name_))
        return nullptr;

    //	class Configuration::ClipperList
    //	{
    //	public:
    //		std::string					name_;
    //		std::vector< glm::vec4 >	clippers_;
    //	};

    auto list = std::make_shared<Environment::ClipperList>(configuration.planes_);
    clipperLists_.emplace(configuration.name_, list);

    return list;
}

std::shared_ptr<EmitterVolume> Builder::buildEmitterVolume(Configuration::EmitterVolume const & configuration)
{
    // Prevent duplicate entries

    if (findEmitterVolume(configuration.name_))
        return nullptr;

    //	class Configuration::EmitterVolume
    //	{
    //	public:
    //
    //		std::string	name_;
    //		std::string	type_;
    //		float		length_;
    //		float		width_;
    //		float		height_;
    //		float		depth_;
    //		float		radius_;
    //	};

    std::shared_ptr<EmitterVolume> pVolume;

    if (configuration.type_ == "point")
    {
        pVolume = std::make_shared<Confetti::EmitterPoint>();
    }
    else if (configuration.type_ == "line")
    {
        pVolume = std::make_shared<Confetti::EmitterLine>(configuration.length_);
    }
    else if (configuration.type_ == "rectangle")
    {
        pVolume = std::make_shared<Confetti::EmitterRectangle>(configuration.width_, configuration.height_);
    }
    else if (configuration.type_ == "circle")
    {
        pVolume = std::make_shared<Confetti::EmitterCircle>(configuration.radius_);
    }
    else if (configuration.type_ == "sphere")
    {
        pVolume = std::make_shared<Confetti::EmitterSphere>(configuration.radius_);
    }
    else if (configuration.type_ == "box")
    {
        pVolume =
            std::make_shared<Confetti::EmitterBox>(glm::vec3(configuration.width_,
                                                             configuration.height_,
                                                             configuration.depth_));
    }
    else if (configuration.type_ == "cylinder")
    {
        pVolume = std::make_shared<Confetti::EmitterCylinder>(configuration.radius_, configuration.height_);
    }
    else if (configuration.type_ == "cone")
    {
        pVolume = std::make_shared<Confetti::EmitterCone>(configuration.radius_, configuration.height_);
    }

    if (pVolume)
        emitterVolumes_.emplace(configuration.name_, pVolume);

    return pVolume;
}

std::vector<PointParticle> Builder::buildPointParticles(int                            n,
                                                        Configuration::Emitter const & emitterConfiguration,
                                                        EmitterVolume const &          randomPosition,
                                                        Environment const &            environment,
                                                        Appearance const &             appearance)
{
    std::vector<PointParticle> particles;
    particles.reserve(n);

    // Generate the particles' characteristics from the emitter configuration.

    Vkx::RandomDirection randomDirection(emitterConfiguration.spread_);
    std::uniform_real_distribution<float> randomSpeed(emitterConfiguration.minSpeed_, emitterConfiguration.maxSpeed_);
    std::uniform_real_distribution<float> randomAge(0.0f, emitterConfiguration.lifetime_);

    for (int i = 0; i < n; i++)
    {
        glm::vec3 direction = randomDirection(rng_);
        float     speed     = randomSpeed(rng_);
        float     age       = randomAge(rng_);
        // Note: RandomDirection returns a direction near the X axis, but the emitter points down the Z axis.
        // The direction returned by RandomDirection must be rotated -90 degrees around the Y axis.
        glm::vec3 const velocity = glm::vec3(-direction.z * speed, direction.y * speed, direction.x * speed);

        particles.emplace_back(emitterConfiguration.lifetime_,
                               age,
                               randomPosition(rng_),
                               velocity,
                               emitterConfiguration.color_);
    }

    return particles;
}

std::vector<PointParticle> Builder::buildPointParticles(Configuration::Emitter::ParticleVector const & configurations)
{
    std::vector<PointParticle> particles;
    particles.reserve(configurations.size());

    for (auto const & c : configurations)
    {
        particles.emplace_back(c.lifetime_, c.age_, c.position_, c.velocity_, c.color_);
    }

    return particles;
}

std::vector<StreakParticle> Builder::buildStreakParticles(int                            n,
                                                          Configuration::Emitter const & emitterConfiguration,
                                                          EmitterVolume const &          randomPosition,
                                                          Environment const &            environment,
                                                          Appearance const &             appearance)
{
    std::vector<StreakParticle> particles;
    particles.reserve(n);

    // Generate the particles' characteristics from the emitter configuration.

    Vkx::RandomDirection randomDirection(emitterConfiguration.spread_);
    std::uniform_real_distribution<float> randomSpeed(emitterConfiguration.minSpeed_, emitterConfiguration.maxSpeed_);
    std::uniform_real_distribution<float> randomAge(0.0f, emitterConfiguration.lifetime_);

    for (int i = 0; i < n; i++)
    {
        glm::vec3 direction = randomDirection(rng_);
        float     speed     = randomSpeed(rng_);
        float     age       = randomAge(rng_);
        glm::vec3 velocity  = glm::vec3(-direction.z * speed, direction.y * speed, direction.x * speed);

        particles.emplace_back(emitterConfiguration.lifetime_,
                               age,
                               randomPosition(rng_),
                               velocity,
                               emitterConfiguration.color_);
    }

    return particles;
}

std::vector<StreakParticle> Builder::buildStreakParticles(Configuration::Emitter::ParticleVector const & configurations)
{
    std::vector<StreakParticle> particles;
    particles.reserve(configurations.size());

    for (auto const & c : configurations)
    {
        particles.emplace_back(c.lifetime_, c.age_, c.position_, c.velocity_, c.color_);
    }

    return particles;
}

std::vector<TexturedParticle> Builder::buildTexturedParticles(int                            n,
                                                              Configuration::Emitter const & emitterConfiguration,
                                                              EmitterVolume const &          randomPosition,
                                                              Environment const &            environment,
                                                              Appearance const &             appearance)
{
    std::vector<TexturedParticle> particles;
    particles.reserve(n);

    // Generate the particles' characteristics from the emitter configuration.

    Vkx::RandomDirection randomDirection(emitterConfiguration.spread_);
    std::uniform_real_distribution<float> randomSpeed(emitterConfiguration.minSpeed_, emitterConfiguration.maxSpeed_);
    std::uniform_real_distribution<float> randomAge(0.0f, emitterConfiguration.lifetime_);
    std::uniform_real_distribution<float> randomRotation(0.0f, glm::two_pi<float>());

    for (int i = 0; i < n; i++)
    {
        glm::vec3 direction = randomDirection(rng_);
        float     speed     = randomSpeed(rng_);
        float     age       = randomAge(rng_);
        glm::vec3 velocity  = glm::vec3(-direction.z * speed, direction.y * speed, direction.x * speed);
        float     rotation  = randomRotation(rng_);

        particles.emplace_back(emitterConfiguration.lifetime_,
                               age,
                               randomPosition(rng_),
                               velocity,
                               emitterConfiguration.color_,
                               emitterConfiguration.radius_,
                               rotation);
    }

    return particles;
}

std::vector<TexturedParticle> Builder::buildTexturedParticles(Configuration::Emitter::ParticleVector const & configurations)
{
    std::vector<TexturedParticle> particles;
    particles.reserve(configurations.size());

    for (auto const & c : configurations)
    {
        particles.emplace_back(c.lifetime_, c.age_, c.position_, c.velocity_, c.color_, c.radius_, c.rotation_);
    }

    return particles;
}

std::vector<SphereParticle> Builder::buildSphereParticles(int                            n,
                                                          Configuration::Emitter const & emitterConfiguration,
                                                          EmitterVolume const &          randomPosition,
                                                          Environment const &            environment,
                                                          Appearance const &             appearance)
{
    std::vector<SphereParticle> particles;
    particles.reserve(n);

    // Generate the particles' characteristics from the emitter configuration.

    Vkx::RandomDirection randomDirection(emitterConfiguration.spread_);
    std::uniform_real_distribution<float> randomSpeed(emitterConfiguration.minSpeed_, emitterConfiguration.maxSpeed_);
    std::uniform_real_distribution<float> randomAge(0.0f, emitterConfiguration.lifetime_);

    for (int i = 0; i < n; i++)
    {
        glm::vec3 direction = randomDirection(rng_);
        float     speed     = randomSpeed(rng_);
        float     age       = randomAge(rng_);
        glm::vec3 velocity  = glm::vec3(-direction.z * speed, direction.y * speed, direction.x * speed);

        particles.emplace_back(emitterConfiguration.lifetime_,
                               age,
                               randomPosition(rng_),
                               velocity,
                               emitterConfiguration.color_,
                               emitterConfiguration.radius_);
    }

    return particles;
}

std::vector<SphereParticle> Builder::buildSphereParticles(Configuration::Emitter::ParticleVector const & configurations)
{
    std::vector<SphereParticle> particles;
    particles.reserve(configurations.size());

    for (auto const & c : configurations)
    {
        particles.emplace_back(c.lifetime_, c.age_, c.position_, c.velocity_, c.color_, c.radius_);
    }

    return particles;
}

// std::vector<EmitterParticle> Builder::buildEmitterParticles(int                            n,
//                                                             Configuration::Emitter const & emitterConfiguration,
//                                                             EmitterVolume const &          randomPosition,
//                                                             Environment const &            environment,
//                                                             Appearance const &             appearance)
// {
//     std::vector<EmitterParticle> particles;
//     particles.reserve(n);
//
//     // Generate the particles' characteristics from the emitter configuration.
//
//     Vkx::RandomDirection randomDirection(emitterConfiguration.spread_);
//     std::uniform_real_distribution<float> randomSpeed(emitterConfiguration.minSpeed_, emitterConfiguration.maxSpeed_);
//     std::uniform_real_distribution<float> randomAge(0.0f, emitterConfiguration.lifetime_);
//
//     for (int i = 0; i < n; i++)
//     {
//         glm::vec3 direction = randomDirection(rng_);
//         float     speed     = randomSpeed(rng_);
//         float     age       = randomAge(rng_);
//         glm::vec3 velocity  = glm::vec3(-direction.z * speed, direction.y * speed, direction.x * speed);
//
//         particles.emplace_back(emitterConfiguration.lifetime_,
//                                age,
//                                randomPosition(rng_),
//                                velocity,
//                                emitterConfiguration.color_,
//                                emitterConfiguration.radius_);
//     }
//
//     return particles;
// }
//
// std::vector<EmitterParticle> Builder::buildEmitterParticles(Configuration::Emitter::ParticleVector const & configurations)
// {
//     std::vector<EmitterParticle> particles;
//     particles.reserve(configurations.size());
//
//     for (auto const & c : configurations)
//     {
//         particles.emplace_back(c.lifetime_, c.age_, c.position_, c.velocity_, c.color_, c.radius_);
//     }
//
//     return particles;
// }

std::shared_ptr<BasicEmitter> Builder::findEmitter(std::string const & name)
{
    EmitterMap::iterator          entry = emitters_.find(name);
    std::shared_ptr<BasicEmitter> emitter;
    if (entry != emitters_.end())
        emitter = entry->second;
    return emitter;
}

std::shared_ptr<Appearance> Builder::findAppearance(std::string const & name)
{
    AppearanceMap::iterator     entry = appearances_.find(name);
    std::shared_ptr<Appearance> appearance;
    if (entry != appearances_.end())
        appearance = entry->second;
    return appearance;
}

std::shared_ptr<Environment> Builder::findEnvironment(std::string const & name)
{
    EnvironmentMap::iterator     entry = environments_.find(name);
    std::shared_ptr<Environment> environment;
    if (entry != environments_.end())
        environment = entry->second;
    return environment;
}

std::shared_ptr<EmitterVolume> Builder::findEmitterVolume(std::string const & name)
{
    EmitterVolumeMap::iterator     entry = emitterVolumes_.find(name);
    std::shared_ptr<EmitterVolume> emitterVolume;
    if (entry != emitterVolumes_.end())
        emitterVolume = entry->second;
    return emitterVolume;
}

std::shared_ptr<Environment::SurfaceList> Builder::findSurfaceList(std::string const & name)
{
    SurfaceListMap::iterator entry = surfaceLists_.find(name);
    std::shared_ptr<Environment::SurfaceList> surfaceList;
    if (entry != surfaceLists_.end())
        surfaceList = entry->second;
    return surfaceList;
}

std::shared_ptr<Environment::ClipperList> Builder::findClipperList(std::string const & name)
{
    ClipperListMap::iterator entry = clipperLists_.find(name);
    std::shared_ptr<Environment::ClipperList> clipperList;
    if (entry != clipperLists_.end())
        clipperList = entry->second;
    return clipperList;
}

std::shared_ptr<Vkx::Material> Builder::findMaterial(std::string const & name)
{
    MaterialMap::iterator          entry = materials_.find(name);
    std::shared_ptr<Vkx::Material> material;
    if (entry != materials_.end())
        material = entry->second;
    return material;
}

std::shared_ptr<Vkx::Texture> Builder::findTexture(std::string const & name)
{
    TextureMap::iterator          entry = textures_.find(name);
    std::shared_ptr<Vkx::Texture> texture;
    if (entry != textures_.end())
        texture = entry->second;
    return texture;
}
} // namespace Confetti
