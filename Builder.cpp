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

std::unique_ptr<ParticleSystem> Builder::buildParticleSystem(Configuration const &        configuration,
                                                             std::shared_ptr<Vkx::Device> device,
                                                             Vkx::Camera const *          pCamera)
{
    // Build the bounce lists used by the emitters

    for (auto const & p : configuration.bouncePlaneLists_)
    {
        buildBouncePlaneList(p.second);
    }

    // Build the clip plane lists used by the emitters

    for (auto const & p : configuration.clipPlaneLists_)
    {
        buildClipPlaneList(p.second);
    }

    // Build the emitter volumes used by the emitters

    for (auto const & v : configuration.emitterVolumes_)
    {
        buildEmitterVolume(v.second);
    }

    std::unique_ptr<ParticleSystem> system = std::make_unique<ParticleSystem>(device);

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
        std::shared_ptr<BasicEmitter> emitter = buildEmitter(e.second);
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

    std::shared_ptr<BasicEmitter> pEmitter;

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

        pEmitter = new PointEmitter(device,
                                    std::move(particles),
                                    volume,
                                    environment,
                                    appearance,
                                    configuration.sorted_);
    }
    else if (configuration.type_ == "streak")
    {
        std::vector<StreakParticle> particles = buildStreakParticles(configuration.count_,
                                                                     configuration.particleVector_,
                                                                     configuration,
                                                                     *volume,
                                                                     *environment,
                                                                     *appearance);

        pEmitter = new StreakEmitter(device,
                                     std::move(particles),
                                     volume,
                                     environment,
                                     appearance,
                                     configuration.count_,
                                     configuration.sorted_);
    }
    else if (configuration.type_ == "textured")
    {
        std::vector<TexturedParticle> particles = buildTexturedParticles(configuration.count_,
                                                                         configuration.particleVector_,
                                                                         configuration,
                                                                         *volume,
                                                                         *environment,
                                                                         *appearance);

        pEmitter = new TexturedEmitter(device,
                                       std::move(particles),
                                       volume,
                                       environment,
                                       appearance,
                                       configuration.count_,
                                       configuration.sorted_);
    }
    else if (configuration.type_ == "sphere")
    {
        std::vector<SphereParticle> particles = buildSphereParticles(configuration.count_,
                                                                     configuration.particleVector_,
                                                                     configuration,
                                                                     *volume,
                                                                     *environment,
                                                                     *appearance);

        pEmitter = new SphereEmitter(device,
                                     std::move(particles),
                                     volume,
                                     environment,
                                     appearance,
                                     configuration.count_,
                                     configuration.sorted_);
    }
//	else if ( configuration.type_ == "emitter" )
//	{
//		std::vector< EmitterParticle >	particles = BuildEmitterParticles( configuration.count_,
//																		         configuration.particleVector_,
//																				 configuration,
//																		         *pVolume,
//																		         *pEnvironment,
//																		         *pAppearance );
//
//		pEmitter = new EmitterEmitter( device,
//                                   std::move(particles),
//									 pVolume,
//									 pEnvironment,
//									 pAppearance,
//									 configuration.count_,
//									 configuration.sorted_ );
//	}
    else
    {
        // Invalid emitter type
        pEmitter = nullptr;
    }

    // Manage the emitter

    if (pEmitter)
        emitters_.emplace(configuration.name_, pEmitter);

    return pEmitter;
}

std::shared_ptr<Appearance> Builder::buildAppearance(Configuration::Appearance const & configuration,
                                                     std::shared_ptr<Vkx::Device>      device,
                                                     Vkx::Camera const *               pCamera)
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

    std::shared_ptr<Vkx::Texture>  pTexture;
    std::shared_ptr<Vkx::Material> pMaterial;

    // Create the texture (if specified)

    if (!configuration.texture_.empty())
    {
        // If the texture is not already created, then create it. Otherwise, use the existing one.

        pTexture = findTexture(configuration.texture_);
        if (!pTexture)
        {
            // Not working for now
            assert(false);
//            HRESULT hr;
//            hr = D3DXCreateTextureFromFile(device, configuration.texture_.c_str(), &pTexture);
//            assert_succeeded(hr);

            addTexture(configuration.texture_, pTexture);
        }
    }
    else
    {
        pTexture = nullptr;   // No texture specified
    }

//	// Create the material (if specified)
//
//	if ( !configuration.material_.empty() )
//	{
//		// If the material is not already created, then create it. Otherwise, use the existing one.
//
//		pMaterial = FindMaterial( configuration.material_ );
//		if ( !pMaterial )
//		{
//		}
//	}
//	else
//	{
    pMaterial = nullptr;      // No material specified
//	}

    std::shared_ptr<Appearance> pAppearance = new Appearance(pCamera,
                                                             configuration.colorChange_,
                                                             configuration.radiusChange_,
                                                             configuration.radialVelocity_,
                                                             pTexture,
                                                             configuration.size_);

    addAppearance(configuration.name_, pAppearance);

    return pAppearance;
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
    //		glm::vec4	gustiness_;
    //		float		airFriction_;
    //		std::string	bounce_;
    //		std::string	clip_;
    //	};

    std::shared_ptr<Environment::BouncePlaneList> pBouncePlaneList = findBouncePlaneList(configuration.bounce_);
    std::shared_ptr<Environment::ClipPlaneList>   pClipPlaneList   = findClipPlaneList(configuration.clip_);

    std::shared_ptr<Environment> pEnvironment = new Environment(configuration.gravity_,
                                                                configuration.windVelocity_,
                                                                configuration.airFriction_,
                                                                configuration.gustiness_,
                                                                pBouncePlaneList, pClipPlaneList);
    addEnvironment(configuration.name_, pEnvironment);
    return pEnvironment;
}

Environment::BouncePlaneList Builder::buildBouncePlaneList(Configuration::BouncePlaneList const & configuration)
{
//     // Prevent duplicate entries
//
//     if (findBouncePlaneList(configuration.name_))
//         return Environment::BouncePlaneList();

    //	class Configuration::BouncePlaneList
    //	{
    //	public:
    //		std::string					name_;
    //		std::vector< BouncePlane >	bouncePlanes_;
    //	};
    //
    //	class Configuration::BouncePlane
    //	{
    //	public:
    //		glm::vec4	plane_;
    //		float		dampening_;
    //	};

    Environment::BouncePlaneList list;
    list.reserve(configuration.bouncePlanes_.size());
    for (auto const & config : configuration.bouncePlanes_)
    {
        list.emplace_back(config.plane_, config.dampening_);
    }

//    addBouncePlaneList(configuration.name_, list);

    return list;
}

Environment::ClipPlaneList Builder::buildClipPlaneList(Configuration::ClipPlaneList const & configuration)
{
//     // Prevent duplicate entries
//
//     if (findClipPlaneList(configuration.name_))
//         return nullptr;

    //	class Configuration::ClipPlaneList
    //	{
    //	public:
    //		std::string					name_;
    //		std::vector< glm::vec4 >	clipPlanes_;
    //	};

    Environment::ClipPlaneList list;
    list.reserve(configuration.clipPlanes_.size());
    for (size_t i = 0; i < configuration.clipPlanes_.size(); i++)
    {
        list[i] = configuration.clipPlanes_[i];
    }

//     addClipPlaneList(configuration.name_, list);

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
        pVolume = std::make_shared<Confetti::EmitterPoint>(rng_);
    }
    else if (configuration.type_ == "line")
    {
        pVolume = std::make_shared<Confetti::EmitterLine>(rng_, configuration.length_);
    }
    else if (configuration.type_ == "rectangle")
    {
        pVolume = std::make_shared<Confetti::EmitterRectangle>(rng_, configuration.width_, configuration.height_);
    }
    else if (configuration.type_ == "circle")
    {
        pVolume = std::make_shared<Confetti::EmitterCircle>(rng_, configuration.radius_);
    }
    else if (configuration.type_ == "sphere")
    {
        pVolume = std::make_shared<Confetti::EmitterSphere>(rng_, configuration.radius_);
    }
    else if (configuration.type_ == "box")
    {
        pVolume =
            std::make_shared<Confetti::EmitterBox>(rng_, glm::vec3(configuration.width_,
                                                                   configuration.height_,
                                                                   configuration.depth_));
    }
    else if (configuration.type_ == "cylinder")
    {
        pVolume = std::make_shared<Confetti::EmitterCylinder>(rng_, configuration.radius_, configuration.height_);
    }
    else if (configuration.type_ == "cone")
    {
        pVolume = std::make_shared<Confetti::EmitterCone>(rng_, configuration.radius_, configuration.height_);
    }

    if (pVolume)
        emitterVolumes_.emplace(configuration.name_, pVolume);

    return pVolume;
}

std::vector<PointParticle> Builder::buildPointParticles(int                            n,
                                                        Configuration::Emitter const & emitterConfiguration,
                                                        EmitterVolume const &          volume,
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
                               volume.next(),
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
                                                          EmitterVolume const &          volume,
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
                               volume.next(),
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
                                                              EmitterVolume const &          volume,
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
                               volume.next(),
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
                                                          EmitterVolume const &          volume,
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
                               volume.next(),
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

std::vector<EmitterParticle> Builder::buildEmitterParticles(int                            n,
                                                            Configuration::Emitter const & emitterConfiguration,
                                                            EmitterVolume const &          volume,
                                                            Environment const &            environment,
                                                            Appearance const &             appearance)
{
    std::vector<EmitterParticle> particles;
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
                               volume.next(),
                               velocity,
                               emitterConfiguration.color_,
                               emitterConfiguration.radius_);
    }

    return particles;
}

std::vector<EmitterParticle> Builder::buildEmitterParticles(Configuration::Emitter::ParticleVector const & configurations)
{
    std::vector<EmitterParticle> particles;
    particles.reserve(configurations.size());

    for (auto const & c : configurations)
    {
        particles.emplace_back(c.lifetime_, c.age_, c.position_, c.velocity_, c.color_, c.radius_);
    }

    return particles;
}

std::shared_ptr<BasicEmitter> Builder::findEmitter(std::string const & name)
{
    EmitterMap::iterator          pEntry = emitters_.find(name);
    std::shared_ptr<BasicEmitter> pEmitter;

    if (pEntry != emitters_.end())
        pEmitter = pEntry->second;
    else
        pEmitter = nullptr;

    return pEmitter;
}

std::shared_ptr<Appearance> Builder::findAppearance(std::string const & name)
{
    AppearanceMap::iterator     pEntry = appearances_.find(name);
    std::shared_ptr<Appearance> pAppearance;

    if (pEntry != appearances_.end())
        pAppearance = pEntry->second;
    else
        pAppearance = nullptr;

    return pAppearance;
}

std::shared_ptr<Environment> Builder::findEnvironment(std::string const & name)
{
    EnvironmentMap::iterator     pEntry = environments_.find(name);
    std::shared_ptr<Environment> pEnvironment;

    if (pEntry != environments_.end())
        pEnvironment = pEntry->second;
    else
        pEnvironment = nullptr;

    return pEnvironment;
}

std::shared_ptr<EmitterVolume> Builder::findEmitterVolume(std::string const & name)
{
    EmitterVolumeMap::iterator     pEntry = emitterVolumes_.find(name);
    std::shared_ptr<EmitterVolume> pEmitterVolume;

    if (pEntry != emitterVolumes_.end())
        pEmitterVolume = pEntry->second;
    else
        pEmitterVolume = nullptr;

    return pEmitterVolume;
}

std::shared_ptr<Environment::BouncePlaneList> Builder::findBouncePlaneList(std::string const & name)
{
    BouncePlaneListMap::iterator pEntry = bouncePlaneLists_.find(name);
    std::shared_ptr<Environment::BouncePlaneList> pBouncePlaneList;

    if (pEntry != bouncePlaneLists_.end())
        pBouncePlaneList = pEntry->second;
    else
        pBouncePlaneList = nullptr;

    return pBouncePlaneList;
}

std::shared_ptr<Environment::ClipPlaneList> Builder::findClipPlaneList(std::string const & name)
{
    ClipPlaneListMap::iterator pEntry = clipPlaneLists_.find(name);
    std::shared_ptr<Environment::ClipPlaneList> pClipPlaneList;

    if (pEntry != clipPlaneLists_.end())
        pClipPlaneList = pEntry->second;
    else
        pClipPlaneList = nullptr;

    return pClipPlaneList;
}

std::shared_ptr<Vkx::Material> Builder::FindMaterial(std::string const & name)
{
    MaterialMap::iterator          pEntry = materials_.find(name);
    std::shared_ptr<Vkx::Material> pMaterial;

    if (pEntry != materials_.end())
        pMaterial = pEntry->second;
    else
        pMaterial = nullptr;

    return pMaterial;
}

std::shared_ptr<Vkx::Texture> Builder::findTexture(std::string const & name)
{
    TextureMap::iterator          pEntry = textures_.find(name);
    std::shared_ptr<Vkx::Texture> pTexture;

    if (pEntry != textures_.end())
        pTexture = pEntry->second;
    else
        pTexture = nullptr;

    return pTexture;
}

Builder::EnvironmentMap::iterator Builder::addEnvironment(std::string const & name, std::shared_ptr<Environment> pEnvironment)
{
    auto i = environments_.emplace(name, pEnvironment);
    assert(i.second);

    return i.first;
}

Builder::AppearanceMap::iterator Builder::addAppearance(std::string const & name, std::shared_ptr<Appearance> pAppearance)
{
    auto i = appearances_.emplace(name, pAppearance);
    assert(i.second);

    return i.first;
}

Builder::BouncePlaneListMap::iterator Builder::addBouncePlaneList(std::string const &                           name,
                                                                  std::shared_ptr<Environment::BouncePlaneList> pList)
{
    auto i = bouncePlaneLists_.emplace(name, pList);
    assert(i.second);

    return i.first;
}

Builder::ClipPlaneListMap::iterator Builder::addClipPlaneList(std::string const &                         name,
                                                              std::shared_ptr<Environment::ClipPlaneList> pList)
{
    auto i = clipPlaneLists_.emplace(name, pList);
    assert(i.second);

    return i.first;
}

Builder::MaterialMap::iterator Builder::addMaterial(std::string const & name, std::shared_ptr<Vkx::Material> pMaterial)
{
    auto i = materials_.emplace(name, pMaterial);
    assert(i.second);

    return i.first;
}

Builder::TextureMap::iterator Builder::addTexture(std::string const & name, std::shared_ptr<Vkx::Texture> pTexture)
{
    auto i = textures_.emplace(name, pTexture);
    assert(i.second);

    return i.first;
}
} // namespace Confetti
