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

#include <random>

using namespace DirectX;

namespace Confetti
{
Builder::Builder(std::minstd_rand & rng)
    : rng_(rng)
{
    // Nothing to do
}

Builder::~Builder()
{
    for (auto & e : emitters_)
    {
        delete e.second;
    }

    for (auto & v : emitterVolumes_)
    {
        delete v.second;
    }

    for (auto & e : environments_)
    {
        delete e.second;
    }

    for (auto & a : appearances_)
    {
        delete a.second;
    }

    for (auto & p : bouncePlaneLists_)
    {
        delete p.second;
    }

    for (auto & p : clipPlaneLists_)
    {
        delete p.second;
    }

    for (auto & m : materials_)
    {
        delete m.second;
    }
}

std::vector<ParticleSystem> Builder::buildParticleSystem(Configuration const & configuration,
                                                         Vkx::Camera const *   pCamera)
{
    std::vector<ParticleSystem> pPS = std::make_unique<ParticleSystem>();

    // Build the bounce lists

    for (auto const & p : configuration.bouncePlaneLists_)
    {
        buildBouncePlaneList(p.second);
    }

    // Build the clip plane lists

    for (auto const & p : configuration.clipPlaneLists_)
    {
        buildClipPlaneList(p.second);
    }

    // Build the emitter volumes

    for (auto const & v : configuration.emitterVolumes_)
    {
        buildEmitterVolume(v.second);
    }

    // Build the environments

    for (auto const & e : configuration.environments_)
    {
        Environment * environment = buildEnvironment(e.second);
        if (environment)
            pPS->add(environment);
    }

    // Build the appearances

    for (auto const & a : configuration.appearances_)
    {
        Appearance * appearance = buildAppearance(a.second, pCamera);
        if (appearance)
            pPS->add(appearance);
    }

    // Build the emitters

    for (auto const & e : configuration.emitters_)
    {
        BasicEmitter * emitter = buildEmitter(e.second);
        if (emitter)
            pPS->add(emitter);
    }

    return pPS;
}

std::vector<PointParticle> Builder::buildPointParticles(
    int                                            n,
    Configuration::Emitter::ParticleVector const & configurations,
    Configuration::Emitter const &                 emitterConfiguration,
    EmitterVolume const &                          volume,
    Environment const &                            environment,
    Appearance const &                             appearance)
{
    std::vector<PointParticle> particles(n);

    // If there are no individual particle configurations, then generate the particles' characteristics from the
    // emitter configuration. Otherwise, use the individual configurations.

    if (configurations.empty())
    {
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

            particles[i].Initialize(emitterConfiguration.lifetime_,
                                    age,
                                    volume.next(),
                                    velocity,
                                    emitterConfiguration.color_);
        }
    }
    else
    {
        assert(configurations.size() == n);

        for (int i = 0; i < n; i++)
        {
            Configuration::Particle const & configuration = configurations[i];      // Convenience

            particles[i].Initialize(configuration.lifetime_,
                                    configuration.age_,
                                    configuration.position_,
                                    configuration.velocity_,
                                    configuration.color_);
        }
    }

    return particles;
}

std::vector<StreakParticle> Builder::buildStreakParticles(
    int                                            n,
    Configuration::Emitter::ParticleVector const & configurations,
    Configuration::Emitter const &                 emitterConfiguration,
    EmitterVolume const &                          volume,
    Environment const &                            environment,
    Appearance const &                             appearance)
{
    std::vector<StreakParticle> particles(n);

    // If there are no individual particle configurations, then generate the particles' characteristics from the
    // emitter configuration. Otherwise, use the individual configurations.

    if (configurations.empty())
    {
        for (int i = 0; i < n; i++)
        {
            Vkx::RandomDirection randomDirection(emitterConfiguration.spread_);
            glm::vec3 direction = randomDirection(rng_);
            std::uniform_real_distribution<float> randomSpeed(emitterConfiguration.minSpeed_, emitterConfiguration.maxSpeed_);
            float speed = randomSpeed(rng_);
            std::uniform_real_distribution<float> randomAge(0.0f, emitterConfiguration.lifetime_);
            float     age      = randomAge(rng_);
            glm::vec3 velocity = glm::vec3(-direction.z * speed, direction.y * speed, direction.x * speed);

            particles[i].Initialize(emitterConfiguration.lifetime_,
                                    age,
                                    volume.next(),
                                    velocity,
                                    emitterConfiguration.color_);
        }
    }
    else
    {
        assert(configurations.size() == n);

        for (int i = 0; i < n; i++)
        {
            Configuration::Particle const & configuration = configurations[i];      // Convenience

            particles[i].Initialize(configuration.lifetime_,
                                    configuration.age_,
                                    configuration.position_,
                                    configuration.velocity_,
                                    configuration.color_);
        }
    }

    return std::vector<StreakParticle>(particles);
}

std::vector<TexturedParticle> Builder::buildTexturedParticles(
    int                                            n,
    Configuration::Emitter::ParticleVector const & configurations,
    Configuration::Emitter const &                 emitterConfiguration,
    EmitterVolume const &                          volume,
    Environment const &                            environment,
    Appearance const &                             appearance)
{
    TexturedParticle * particles = new TexturedParticle[n];

    // If there are no individual particle configurations, then generate the particles' characteristics from the
    // emitter configuration. Otherwise, use the individual configurations.

    if (configurations.empty())
    {
        for (int i = 0; i < n; i++)
        {
            Vkx::RandomDirection randomDirection(emitterConfiguration.spread_);
            glm::vec3 direction = randomDirection(rng_);
            std::uniform_real_distribution<float> randomSpeed(emitterConfiguration.minSpeed_, emitterConfiguration.maxSpeed_);
            float speed = randomSpeed(rng_);
            std::uniform_real_distribution<float> randomAge(0.0f, emitterConfiguration.lifetime_);
            float     age      = randomAge(rng_);
            glm::vec3 velocity = glm::vec3(-direction.z * speed, direction.y * speed, direction.x * speed);
            std::uniform_real_distribution<float> randomRotation(0.0f, glm::two_pi<float>());
            float rotation = randomRotation(rng_);

            particles[i].Initialize(emitterConfiguration.lifetime_,
                                    age,
                                    volume.next(),
                                    velocity,
                                    emitterConfiguration.color_,
                                    emitterConfiguration.radius_,
                                    rotation);
        }
    }
    else
    {
        assert(configurations.size() == n);

        for (int i = 0; i < n; i++)
        {
            Configuration::Particle const & configuration = configurations[i];      // Convenience

            particles[i].Initialize(configuration.lifetime_,
                                    configuration.age_,
                                    configuration.position_,
                                    configuration.velocity_,
                                    configuration.color_,
                                    configuration.radius_,
                                    configuration.rotation_);
        }
    }

    return std::vector<TexturedParticle>(particles);
}

std::vector<SphereParticle> Builder::buildSphereParticles(
    int                                            n,
    Configuration::Emitter::ParticleVector const & configurations,
    Configuration::Emitter const &                 emitterConfiguration,
    EmitterVolume const &                          volume,
    Environment const &                            environment,
    Appearance const &                             appearance)
{
    SphereParticle * particles = new SphereParticle[n];

    // If there are no individual particle configurations, then generate the particles' characteristics from the
    // emitter configuration. Otherwise, use the individual configurations.

    if (configurations.empty())
    {
        for (int i = 0; i < n; i++)
        {
            Vkx::RandomDirection randomDirection(emitterConfiguration.spread_);
            glm::vec3 direction = randomDirection(rng_);
            std::uniform_real_distribution<float> randomSpeed(emitterConfiguration.minSpeed_, emitterConfiguration.maxSpeed_);
            float speed = randomSpeed(rng_);
            std::uniform_real_distribution<float> randomAge(0.0f, emitterConfiguration.lifetime_);
            float     age      = randomAge(rng_);
            glm::vec3 velocity = glm::vec3(-direction.z * speed, direction.y * speed, direction.x * speed);

            particles[i].Initialize(emitterConfiguration.lifetime_,
                                    age,
                                    volume.next(),
                                    velocity,
                                    emitterConfiguration.color_,
                                    emitterConfiguration.radius_);
        }
    }
    else
    {
        assert(configurations.size() == n);

        for (int i = 0; i < n; i++)
        {
            Configuration::Particle const & configuration = configurations[i];      // Convenience

            particles[i].Initialize(configuration.lifetime_,
                                    configuration.age_,
                                    configuration.position_,
                                    configuration.velocity_,
                                    configuration.color_,
                                    configuration.radius_);
        }
    }

    return std::vector<SphereParticle>(particles);
}

// ********************************************************************************************************************/
// *																													*/
// ********************************************************************************************************************/
//
// std::vector< EmitterParticle > Builder::BuildEmitterParticles(
//												int n,
//												Configuration::Emitter::ParticleVector const & configurations,
//												Configuration::Emitter const & emitterConfiguration,
//											    EmitterVolume const & volume,
//											    Environment const & environment,
//											    Appearance const & appearance )
// {
//	EmitterParticle * const	particles	= new EmitterParticle[ n ];
//
//	// If there are no individual particle configurations, then generate the particles' characteristics from the
//	// emitter configuration. Otherwise, use the individual configurations.
//
//	if ( configurations.empty() )
//	{
//		for ( int i = 0; i < n; i++ )
//		{
//			glm::vec4	const	direction	= dRng_.Get( emitterConfiguration.spread_ );
//			float const			speed		= fRng_( emitterConfiguration.minSpeed_,
//														  emitterConfiguration.maxSpeed_ );
//			glm::vec4	const	velocity	= glm::vec4( -direction.z, direction.y, direction.x ) * speed;
//
//			particles[ i ].Initialize( emitterConfiguration.lifetime_,
//										 fRng_( emitterConfiguration.lifetime_ ),
//										 volume.next(),
//										 velocity,
//										 emitterConfiguration.color_,
//										 emitterConfiguration.radius_,
//										 oRng_.Get() );
//		}
//	}
//	else
//	{
//		assert( configurations.size() == n );
//
//		for ( int i = 0; i < n; i++ )
//		{
//			Configuration::Particle const & configuration	= configurations[ i ];	// Convenience
//
//			particles[ i ].Initialize( configuration.lifetime_,
//										 configuration.age_,
//										 configuration.position_,
//										 configuration.velocity_,
//										 configuration.color_,
//										 configuration.radius_,
//										 configuration.orientation_ );
//		}
//	}
//
//
//	return std::vector< EmitterParticle >( particles );
// }

BasicEmitter * Builder::buildEmitter(Configuration::Emitter const & configuration, std::shared_ptr<Vkx::Device> pD3dDevice)
{
    // Prevent duplicate entries

    if (findEmitter(configuration.name_))
        return nullptr;

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

    EmitterVolume * pVolume      = findEmitterVolume(configuration.volume_);
    Environment *   pEnvironment = findEnvironment(configuration.environment_);
    Appearance *    pAppearance  = findAppearance(configuration.appearance_);

    BasicEmitter * pEmitter;

    if (configuration.type_ == "point")
    {
        std::vector<PointParticle> qaParticles = buildPointParticles(configuration.count_,
                                                                     configuration.particleVector_,
                                                                     configuration,
                                                                     *pVolume,
                                                                     *pEnvironment,
                                                                     *pAppearance);

        pEmitter = new PointEmitter(pD3dDevice,
                                    std::move(qaParticles),
                                    pVolume,
                                    pEnvironment,
                                    pAppearance,
                                    configuration.count_,
                                    configuration.sorted_);
    }
    else if (configuration.type_ == "streak")
    {
        std::vector<StreakParticle> qaParticles = buildStreakParticles(configuration.count_,
                                                                       configuration.particleVector_,
                                                                       configuration,
                                                                       *pVolume,
                                                                       *pEnvironment,
                                                                       *pAppearance);

        pEmitter = new StreakEmitter(pD3dDevice,
                                     std::move(qaParticles),
                                     pVolume,
                                     pEnvironment,
                                     pAppearance,
                                     configuration.count_,
                                     configuration.sorted_);
    }
    else if (configuration.type_ == "textured")
    {
        std::vector<TexturedParticle> qaParticles = buildTexturedParticles(configuration.count_,
                                                                           configuration.particleVector_,
                                                                           configuration,
                                                                           *pVolume,
                                                                           *pEnvironment,
                                                                           *pAppearance);

        pEmitter = new TexturedEmitter(pD3dDevice,
                                       std::move(qaParticles),
                                       pVolume,
                                       pEnvironment,
                                       pAppearance,
                                       configuration.count_,
                                       configuration.sorted_);
    }
    else if (configuration.type_ == "sphere")
    {
        std::vector<SphereParticle> qaParticles = buildSphereParticles(configuration.count_,
                                                                       configuration.particleVector_,
                                                                       configuration,
                                                                       *pVolume,
                                                                       *pEnvironment,
                                                                       *pAppearance);

        pEmitter = new SphereEmitter(pD3dDevice,
                                     std::move(qaParticles),
                                     pVolume,
                                     pEnvironment,
                                     pAppearance,
                                     configuration.count_,
                                     configuration.sorted_);
    }
//	else if ( configuration.type_ == "emitter" )
//	{
//		std::vector< EmitterParticle >	qaParticles = BuildEmitterParticles( configuration.count_,
//																		         configuration.particleVector_,
//																				 configuration,
//																		         *pVolume,
//																		         *pEnvironment,
//																		         *pAppearance );
//
//		pEmitter = new EmitterEmitter( pD3dDevice,
//                                   std::move(qaParticles),
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
        addEmitter(configuration.name_, pEmitter);

    return pEmitter;
}

Appearance * Builder::buildAppearance(Configuration::Appearance const & configuration,
                                      std::shared_ptr<Vkx::Device>      pD3dDevice,
                                      ID3D11DeviceContext *             pD3dContext,
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

    std::shared_ptr<Vkx::Texture> pTexture;
    Vkx::Material * pMaterial;

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
//            hr = D3DXCreateTextureFromFile(pD3dDevice, configuration.texture_.c_str(), &pTexture);
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

    Appearance * pAppearance = new Appearance(pCamera,
                                              configuration.colorChange_,
                                              configuration.radiusChange_,
                                              configuration.radialVelocity_,
                                              pTexture,
                                              configuration.size_);

    addAppearance(configuration.name_, pAppearance);

    return pAppearance;
}

EmitterVolume * Builder::buildEmitterVolume(Configuration::EmitterVolume const & configuration)
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

    EmitterVolume * pVolume;

    if (configuration.type_ == "point")
    {
        pVolume = new EmitterPoint(rng_);
    }
    else if (configuration.type_ == "line")
    {
        pVolume = new EmitterLine(rng_, configuration.length_);
    }
    else if (configuration.type_ == "rectangle")
    {
        pVolume = new EmitterRectangle(rng_, configuration.width_, configuration.height_);
    }
    else if (configuration.type_ == "circle")
    {
        pVolume = new EmitterCircle(rng_, configuration.radius_);
    }
    else if (configuration.type_ == "sphere")
    {
        pVolume = new EmitterSphere(rng_, configuration.radius_);
    }
    else if (configuration.type_ == "box")
    {
        pVolume = new EmitterBox(rng_, { configuration.width_, configuration.height_, configuration.depth_ });
    }
    else if (configuration.type_ == "cylinder")
    {
        pVolume = new EmitterCylinder(rng_, configuration.radius_, configuration.height_);
    }
    else if (configuration.type_ == "cone")
    {
        pVolume = new EmitterCone(rng_, configuration.radius_, configuration.height_);
    }
    else
    {
        // Invalid configuration.type_
        pVolume = nullptr;
    }

    if (pVolume)
        addEmitterVolume(configuration.name_, pVolume);

    return pVolume;
}

Environment * Builder::buildEnvironment(Configuration::Environment const & configuration)
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
    //		float		friction_;
    //		std::string	bounce_;
    //		std::string	clip_;
    //	};

    Environment::BouncePlaneList * pBouncePlaneList = findBouncePlaneList(configuration.bounce_);
    Environment::ClipPlaneList *   pClipPlaneList   = findClipPlaneList(configuration.clip_);

    Environment * pEnvironment = new Environment(configuration.gravity_,
                                                 configuration.windVelocity_,
                                                 configuration.friction_,
                                                 configuration.gustiness_,
                                                 pBouncePlaneList, pClipPlaneList);
    addEnvironment(configuration.name_, pEnvironment);
    return pEnvironment;
}

Environment::BouncePlaneList * Builder::buildBouncePlaneList(Configuration::BouncePlaneList const & configuration)
{
    // Prevent duplicate entries

    if (findBouncePlaneList(configuration.name_))
        return nullptr;

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

    Environment::BouncePlaneList * pList = new Environment::BouncePlaneList;
    pList->resize(configuration.bouncePlanes_.size());
    for (size_t i = 0; i < configuration.bouncePlanes_.size(); i++)
    {
        (*pList)[i].plane_     = configuration.bouncePlanes_[i].plane_;
        (*pList)[i].dampening_ = configuration.bouncePlanes_[i].dampening_;
    }

    addBouncePlaneList(configuration.name_, pList);

    return pList;
}

Environment::ClipPlaneList * Builder::buildClipPlaneList(Configuration::ClipPlaneList const & configuration)
{
    // Prevent duplicate entries

    if (findClipPlaneList(configuration.name_))
        return nullptr;

    //	class Configuration::ClipPlaneList
    //	{
    //	public:
    //		std::string					name_;
    //		std::vector< glm::vec4 >	clipPlanes_;
    //	};

    Environment::ClipPlaneList * pList = new Environment::ClipPlaneList();
    pList->resize(configuration.clipPlanes_.size());
    for (size_t i = 0; i < configuration.clipPlanes_.size(); i++)
    {
        (*pList)[i] = configuration.clipPlanes_[i];
    }

    addClipPlaneList(configuration.name_, pList);

    return pList;
}

BasicEmitter * Builder::findEmitter(std::string const & name)
{
    EmitterMap::iterator pEntry = emitters_.find(name);
    BasicEmitter *       pEmitter;

    if (pEntry != emitters_.end())
        pEmitter = pEntry->second;
    else
        pEmitter = nullptr;

    return pEmitter;
}

Appearance * Builder::findAppearance(std::string const & name)
{
    AppearanceMap::iterator pEntry = appearances_.find(name);
    Appearance * pAppearance;

    if (pEntry != appearances_.end())
        pAppearance = pEntry->second;
    else
        pAppearance = nullptr;

    return pAppearance;
}

Environment * Builder::findEnvironment(std::string const & name)
{
    EnvironmentMap::iterator pEntry = environments_.find(name);
    Environment * pEnvironment;

    if (pEntry != environments_.end())
        pEnvironment = pEntry->second;
    else
        pEnvironment = nullptr;

    return pEnvironment;
}

EmitterVolume * Builder::findEmitterVolume(std::string const & name)
{
    EmitterVolumeMap::iterator pEntry = emitterVolumes_.find(name);
    EmitterVolume * pEmitterVolume;

    if (pEntry != emitterVolumes_.end())
        pEmitterVolume = pEntry->second;
    else
        pEmitterVolume = nullptr;

    return pEmitterVolume;
}

Environment::BouncePlaneList * Builder::findBouncePlaneList(std::string const & name)
{
    BouncePlaneListMap::iterator   pEntry = bouncePlaneLists_.find(name);
    Environment::BouncePlaneList * pBouncePlaneList;

    if (pEntry != bouncePlaneLists_.end())
        pBouncePlaneList = pEntry->second;
    else
        pBouncePlaneList = nullptr;

    return pBouncePlaneList;
}

Environment::ClipPlaneList * Builder::findClipPlaneList(std::string const & name)
{
    ClipPlaneListMap::iterator   pEntry = clipPlaneLists_.find(name);
    Environment::ClipPlaneList * pClipPlaneList;

    if (pEntry != clipPlaneLists_.end())
        pClipPlaneList = pEntry->second;
    else
        pClipPlaneList = nullptr;

    return pClipPlaneList;
}

// Vkx::Material * Builder::FindMaterial(std::string const & name)
// {
//     MaterialMap::iterator pEntry = materials_.find(name);
//     Vkx::Material *        pMaterial;
//
//     if (pEntry != materials_.end())
//         pMaterial = pEntry->second;
//     else
//         pMaterial= nullptr;
//
//     return pMaterial;
// }
//
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

Builder::EmitterMap::iterator Builder::addEmitter(std::string const & name, BasicEmitter * pEmitter)
{
    std::pair<EmitterMap::iterator, bool> rv;

    rv = emitters_.insert(EmitterMap::value_type(name, pEmitter));
    assert(rv.second);

    return rv.first;
}

Builder::EmitterVolumeMap::iterator Builder::addEmitterVolume(std::string const & name, EmitterVolume * pVolume)
{
    std::pair<EmitterVolumeMap::iterator, bool> rv;

    rv = emitterVolumes_.insert(EmitterVolumeMap::value_type(name, pVolume));
    assert(rv.second);

    return rv.first;
}

Builder::EnvironmentMap::iterator Builder::addEnvironment(std::string const & name, Environment * pEnvironment)
{
    std::pair<EnvironmentMap::iterator, bool> rv;

    rv = environments_.insert(EnvironmentMap::value_type(name, pEnvironment));
    assert(rv.second);

    return rv.first;
}

Builder::AppearanceMap::iterator Builder::addAppearance(std::string const & name, Appearance * pAppearance)
{
    std::pair<AppearanceMap::iterator, bool> rv;

    rv = appearances_.insert(AppearanceMap::value_type(name, pAppearance));
    assert(rv.second);

    return rv.first;
}

Builder::BouncePlaneListMap::iterator Builder::addBouncePlaneList(std::string const & name, Environment::BouncePlaneList * pList)
{
    std::pair<BouncePlaneListMap::iterator, bool> rv;

    rv = bouncePlaneLists_.insert(BouncePlaneListMap::value_type(name, pList));
    assert(rv.second);

    return rv.first;
}

Builder::ClipPlaneListMap::iterator Builder::addClipPlaneList(std::string const & name, Environment::ClipPlaneList * pList)
{
    std::pair<ClipPlaneListMap::iterator, bool> rv;

    rv = clipPlaneLists_.insert(ClipPlaneListMap::value_type(name, pList));
    assert(rv.second);

    return rv.first;
}
//
// Builder::MaterialMap::iterator Builder::AddMaterial(std::string const & name, Vkx::Material * pMaterial)
// {
//     std::pair<MaterialMap::iterator, bool> rv;
//
//     rv = materials_.insert(MaterialMap::value_type(name, pMaterial));
//     assert(rv.second);
//
//     return rv.first;
// }

Builder::TextureMap::iterator Builder::addTexture(std::string const & name, std::shared_ptr<Vkx::Texture> pTexture)
{
    std::pair<TextureMap::iterator, bool> rv;

    rv = textures_.insert(TextureMap::value_type(name, pTexture));
    assert(rv.second);

    return rv.first;
}
} // namespace Confetti
