#include "Builder.h"

#include "Appearance.h"
#include "Configuration.h"
#include "Emitter.h"
#include "EmitterVolume.h"
#include "Environment.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "PointParticle.h"
#include "SphereParticle.h"
#include "StreakParticle.h"
#include "TexturedParticle.h"
// #include "EmitterParticle.h"

#include "Wx/Wx.h"

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

namespace Confetti
{
Builder::Builder(uint32_t seed)
    : rng_(seed)
    , fRng_(~seed)
    , dRng_(seed ^ (uint32_t)uintptr_t(this))
    , oRng_(seed ^ ~(uint32_t)uintptr_t(this))
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

    for (auto & t : textures_)
    {
        Wx::SafeRelease(t.second);
    }
}

std::unique_ptr<ParticleSystem> Builder::BuildParticleSystem(Configuration const & configuration,
                                                           ID3D11Device *        pD3dDevice,
                                                           Dxx::Camera const *   pCamera)
{
    std::unique_ptr<ParticleSystem> pPS = std::make_unique<ParticleSystem>(pD3dDevice);

    // Build the bounce lists

    for (auto const & p : configuration.bouncePlaneLists_)
    {
        BuildBouncePlaneList(p.second);
    }

    // Build the clip plane lists

    for (auto const & p : configuration.clipPlaneLists_)
    {
        BuildClipPlaneList(p.second);
    }

    // Build the emitter volumes

    for (auto const & v : configuration.emitterVolumes_)
    {
        BuildEmitterVolume(v.second);
    }

    // Build the environments

    for (auto const & e : configuration.environments_)
    {
        Environment * environment = BuildEnvironment(e.second);
        if (environment)
            pPS->Register(environment);
    }

    // Build the appearances

    for (auto const & a : configuration.appearances_)
    {
        Appearance * appearance = BuildAppearance(a.second, pD3dDevice, pCamera);
        if (appearance)
            pPS->Register(appearance);
    }

    // Build the emitters

    for (auto const & e : configuration.emitters_)
    {
        BasicEmitter * emitter = BuildEmitter(e.second, pD3dDevice);
        if (emitter)
            pPS->Register(emitter);
    }

    return pPS;
}

std::unique_ptr<PointParticle> Builder::BuildPointParticles(
    int                                            n,
    Configuration::Emitter::ParticleVector const & configurations,
    Configuration::Emitter const &                 emitterConfiguration,
    EmitterVolume const &                          volume,
    Environment const &                            environment,
    Appearance const &                             appearance)
{
    PointParticle * paParticles = new PointParticle[n];

    // If there are no individual particle configurations, then generate the particles' characteristics from the
    // emitter configuration. Otherwise, use the individual configurations.

    if (configurations.empty())
    {
        for (int i = 0; i < n; i++)
        {
            // Note: RandomDirection returns a direction near the X axis, but the emitter points down the Z axis.
            // The direction returned by RandomDirection must be rotated -90 degrees around the Y axis.

            DirectX::XMFLOAT3 const direction = dRng_(emitterConfiguration.spread_);
            float const speed = fRng_(emitterConfiguration.minSpeed_, emitterConfiguration.maxSpeed_);
            DirectX::XMFLOAT3 const velocity = DirectX::XMFLOAT3(-direction.z * speed, direction.y * speed, direction.x * speed);

            paParticles[i].Initialize(emitterConfiguration.lifetime_,
                                      fRng_(emitterConfiguration.lifetime_),
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

            paParticles[i].Initialize(configuration.lifetime_,
                                      configuration.age_,
                                      configuration.position_,
                                      configuration.velocity_,
                                      configuration.color_);
        }
    }

    return std::unique_ptr<PointParticle>(paParticles);
}

std::unique_ptr<StreakParticle> Builder::BuildStreakParticles(
    int                                            n,
    Configuration::Emitter::ParticleVector const & configurations,
    Configuration::Emitter const &                 emitterConfiguration,
    EmitterVolume const &                          volume,
    Environment const &                            environment,
    Appearance const &                             appearance)
{
    StreakParticle * const paParticles = new StreakParticle[n];

    // If there are no individual particle configurations, then generate the particles' characteristics from the
    // emitter configuration. Otherwise, use the individual configurations.

    if (configurations.empty())
    {
        for (int i = 0; i < n; i++)
        {
            DirectX::XMFLOAT3 direction = dRng_(emitterConfiguration.spread_);
            float speed = fRng_(emitterConfiguration.minSpeed_, emitterConfiguration.maxSpeed_);
            DirectX::XMFLOAT3 velocity = DirectX::XMFLOAT3(-direction.z * speed, direction.y * speed, direction.x * speed);

            paParticles[i].Initialize(emitterConfiguration.lifetime_,
                                      fRng_(emitterConfiguration.lifetime_),
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

            paParticles[i].Initialize(configuration.lifetime_,
                                      configuration.age_,
                                      configuration.position_,
                                      configuration.velocity_,
                                      configuration.color_);
        }
    }

    return std::unique_ptr<StreakParticle>(paParticles);
}

std::unique_ptr<TexturedParticle> Builder::BuildTexturedParticles(
    int                                            n,
    Configuration::Emitter::ParticleVector const & configurations,
    Configuration::Emitter const &                 emitterConfiguration,
    EmitterVolume const &                          volume,
    Environment const &                            environment,
    Appearance const &                             appearance)
{
    TexturedParticle * paParticles = new TexturedParticle[n];

    // If there are no individual particle configurations, then generate the particles' characteristics from the
    // emitter configuration. Otherwise, use the individual configurations.

    if (configurations.empty())
    {
        for (int i = 0; i < n; i++)
        {
            DirectX::XMFLOAT3 direction = dRng_(emitterConfiguration.spread_);
            float speed = fRng_(emitterConfiguration.minSpeed_,
                                    emitterConfiguration.maxSpeed_);
            DirectX::XMFLOAT3 velocity = DirectX::XMFLOAT3(-direction.z * speed, direction.y * speed, direction.x * speed);

            paParticles[i].Initialize(emitterConfiguration.lifetime_,
                                      fRng_(emitterConfiguration.lifetime_),
                                      volume.next(),
                                      velocity,
                                      emitterConfiguration.color_,
                                      emitterConfiguration.radius_,
                                      fRng_(DirectX::XM_2PI));
        }
    }
    else
    {
        assert(configurations.size() == n);

        for (int i = 0; i < n; i++)
        {
            Configuration::Particle const & configuration = configurations[i];      // Convenience

            paParticles[i].Initialize(configuration.lifetime_,
                                      configuration.age_,
                                      configuration.position_,
                                      configuration.velocity_,
                                      configuration.color_,
                                      configuration.radius_,
                                      configuration.rotation_);
        }
    }

    return std::unique_ptr<TexturedParticle>(paParticles);
}

std::unique_ptr<SphereParticle> Builder::BuildSphereParticles(
    int                                            n,
    Configuration::Emitter::ParticleVector const & configurations,
    Configuration::Emitter const &                 emitterConfiguration,
    EmitterVolume const &                          volume,
    Environment const &                            environment,
    Appearance const &                             appearance)
{
    SphereParticle * paParticles = new SphereParticle[n];

    // If there are no individual particle configurations, then generate the particles' characteristics from the
    // emitter configuration. Otherwise, use the individual configurations.

    if (configurations.empty())
    {
        for (int i = 0; i < n; i++)
        {
            DirectX::XMFLOAT3 direction = dRng_(emitterConfiguration.spread_);
            float speed = fRng_(emitterConfiguration.minSpeed_,
                                    emitterConfiguration.maxSpeed_);
            DirectX::XMFLOAT3 velocity = DirectX::XMFLOAT3(-direction.z * speed, direction.y * speed, direction.x * speed);

            paParticles[i].Initialize(emitterConfiguration.lifetime_,
                                      fRng_(emitterConfiguration.lifetime_),
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

            paParticles[i].Initialize(configuration.lifetime_,
                                      configuration.age_,
                                      configuration.position_,
                                      configuration.velocity_,
                                      configuration.color_,
                                      configuration.radius_);
        }
    }

    return std::unique_ptr<SphereParticle>(paParticles);
}

/// ********************************************************************************************************************/
/// *																													*/
/// ********************************************************************************************************************/
//
// std::unique_ptr< EmitterParticle > Builder::BuildEmitterParticles(
//												int n,
//												Configuration::Emitter::ParticleVector const & configurations,
//												Configuration::Emitter const & emitterConfiguration,
//											    EmitterVolume const & volume,
//											    Environment const & environment,
//											    Appearance const & appearance )
// {
//	EmitterParticle * const	paParticles	= new EmitterParticle[ n ];
//
//	// If there are no individual particle configurations, then generate the particles' characteristics from the
//	// emitter configuration. Otherwise, use the individual configurations.
//
//	if ( configurations.empty() )
//	{
//		for ( int i = 0; i < n; i++ )
//		{
//			DirectX::XMFLOAT4	const	direction	= dRng_.Get( emitterConfiguration.spread_ );
//			float const			speed		= fRng_( emitterConfiguration.minSpeed_,
//														  emitterConfiguration.maxSpeed_ );
//			DirectX::XMFLOAT4	const	velocity	= DirectX::XMFLOAT4( -direction.z, direction.y, direction.x ) * speed;
//
//			paParticles[ i ].Initialize( emitterConfiguration.lifetime_,
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
//			paParticles[ i ].Initialize( configuration.lifetime_,
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
//	return std::unique_ptr< EmitterParticle >( paParticles );
// }

BasicEmitter * Builder::BuildEmitter(Configuration::Emitter const & configuration, ID3D11Device * pD3dDevice)
{
    // Prevent duplicate entries

    if (FindEmitter(configuration.name_))
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
    //	DirectX::XMFLOAT4		position_;
    //	DirectX::XMFLOAT4	orientation_;
    //	DirectX::XMFLOAT4		velocity_;
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

    EmitterVolume * pVolume      = FindEmitterVolume(configuration.volume_);
    Environment *   pEnvironment = FindEnvironment(configuration.environment_);
    Appearance *    pAppearance  = FindAppearance(configuration.appearance_);

    BasicEmitter * pEmitter;

    if (configuration.type_ == "point")
    {
        std::unique_ptr<PointParticle> qaParticles = BuildPointParticles(configuration.count_,
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
        std::unique_ptr<StreakParticle> qaParticles = BuildStreakParticles(configuration.count_,
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
        std::unique_ptr<TexturedParticle> qaParticles = BuildTexturedParticles(configuration.count_,
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
        std::unique_ptr<SphereParticle> qaParticles = BuildSphereParticles(configuration.count_,
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
//		std::unique_ptr< EmitterParticle >	qaParticles = BuildEmitterParticles( configuration.count_,
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
        pEmitter= nullptr;
    }

    // Manage the emitter

    if (pEmitter)
        AddEmitter(configuration.name_, pEmitter);

    return pEmitter;
}

Appearance * Builder::BuildAppearance(Configuration::Appearance const & configuration,
                                      ID3D11Device *                    pD3dDevice,
                                      Dxx::Camera const *               pCamera)
{
    // Prevent duplicate entries

    if (FindAppearance(configuration.name_))
        return nullptr;

    //	class Configuration::Appearance
    //	{
    //	public:
    //		std::string	name_;
    //		DirectX::XMFLOAT4	colorChange_;
    //		float		radiusChange_;
    //		float		radialVelocity_;
    //		std::string	texture_;
    //		float		size_;
    //	};

    ID3D11Texture2D * pTexture;
    _D3DMATERIAL11 *       pMaterial;

    // Create the texture (if specified)

    if (!configuration.texture_.empty())
    {
        // If the texture is not already created, then create it. Otherwise, use the existing one.

        pTexture = FindTexture(configuration.texture_);
        if (!pTexture)
        {
            HRESULT hr;
            hr = D3DXCreateTextureFromFile(pD3dDevice, configuration.texture_.c_str(), &pTexture);
            assert_succeeded(hr);

            AddTexture(configuration.texture_, pTexture);
        }
    }
    else
    {
        pTexture= nullptr;   // No texture specified
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
    pMaterial= nullptr;      // No material specified
//	}

    Appearance * pAppearance = new Appearance(pCamera,
                                              configuration.colorChange_,
                                              configuration.radiusChange_,
                                              configuration.radialVelocity_,
                                              pTexture,
                                              configuration.size_);

    AddAppearance(configuration.name_, pAppearance);

    return pAppearance;
}

EmitterVolume * Builder::BuildEmitterVolume(Configuration::EmitterVolume const & configuration)
{
    // Prevent duplicate entries

    if (FindEmitterVolume(configuration.name_))
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
        pVolume = new EmitterPoint(rng_());
    }
    else if (configuration.type_ == "line")
    {
        pVolume = new EmitterLine(rng_(), configuration.length_);
    }
    else if (configuration.type_ == "rectangle")
    {
        pVolume = new EmitterRectangle(rng_(), configuration.width_, configuration.height_);
    }
    else if (configuration.type_ == "circle")
    {
        pVolume = new EmitterCircle(rng_(), configuration.radius_);
    }
    else if (configuration.type_ == "sphere")
    {
        pVolume = new EmitterSphere(rng_(), configuration.radius_);
    }
    else if (configuration.type_ == "box")
    {
        pVolume = new EmitterBox(rng_(), { configuration.width_, configuration.height_, configuration.depth_ });
    }
    else if (configuration.type_ == "cylinder")
    {
        pVolume = new EmitterCylinder(rng_(), configuration.radius_, configuration.height_);
    }
    else if (configuration.type_ == "cone")
    {
        pVolume = new EmitterCone(rng_(), configuration.radius_, configuration.height_);
    }
    else
    {
        // Invalid configuration.type_
        pVolume = nullptr;
    }

    if (pVolume)
        AddEmitterVolume(configuration.name_, pVolume);

    return pVolume;
}

Environment * Builder::BuildEnvironment(Configuration::Environment const & configuration)
{
    // Prevent duplicate entries

    if (FindEnvironment(configuration.name_))
        return nullptr;

    //	class Configuration::Environment
    //	{
    //	public:
    //		std::string	name_;
    //		DirectX::XMFLOAT4	gravity_;
    //		DirectX::XMFLOAT4	windVelocity_;
    //		DirectX::XMFLOAT4	gustiness_;
    //		float		airFriction_;
    //		std::string	bounce_;
    //		std::string	clip_;
    //	};

    Environment::BouncePlaneList * pBouncePlaneList = FindBouncePlaneList(configuration.bounce_);
    Environment::ClipPlaneList *   pClipPlaneList   = FindClipPlaneList(configuration.clip_);

    Environment * pEnvironment = new Environment(configuration.gravity_,
                                                 configuration.windVelocity_,
                                                 configuration.airFriction_,
                                                 configuration.gustiness_,
                                                 pBouncePlaneList, pClipPlaneList);
    AddEnvironment(configuration.name_, pEnvironment);
    return pEnvironment;
}

Environment::BouncePlaneList * Builder::BuildBouncePlaneList(Configuration::BouncePlaneList const & configuration)
{
    // Prevent duplicate entries

    if (FindBouncePlaneList(configuration.name_))
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
    //		DirectX::XMFLOAT4	plane_;
    //		float		dampening_;
    //	};

    Environment::BouncePlaneList * pList = new Environment::BouncePlaneList;
    pList->resize(configuration.bouncePlanes_.size());
    for (size_t i = 0; i < configuration.bouncePlanes_.size(); i++)
    {
        (*pList)[i].plane_     = configuration.bouncePlanes_[i].plane_;
        (*pList)[i].dampening_ = configuration.bouncePlanes_[i].dampening_;
    }

    AddBouncePlaneList(configuration.name_, pList);

    return pList;
}

Environment::ClipPlaneList * Builder::BuildClipPlaneList(Configuration::ClipPlaneList const & configuration)
{
    // Prevent duplicate entries

    if (FindClipPlaneList(configuration.name_))
        return nullptr;

    //	class Configuration::ClipPlaneList
    //	{
    //	public:
    //		std::string					name_;
    //		std::vector< DirectX::XMFLOAT4 >	clipPlanes_;
    //	};

    Environment::ClipPlaneList * pList = new Environment::ClipPlaneList();
    pList->resize(configuration.clipPlanes_.size());
    for (size_t i = 0; i < configuration.clipPlanes_.size(); i++)
    {
        (*pList)[i] = configuration.clipPlanes_[i];
    }

    AddClipPlaneList(configuration.name_, pList);

    return pList;
}

BasicEmitter * Builder::FindEmitter(std::string const & name)
{
    EmitterMap::iterator pEntry = emitters_.find(name);
    BasicEmitter *       pEmitter;

    if (pEntry != emitters_.end())
        pEmitter = pEntry->second;
    else
        pEmitter= nullptr;

    return pEmitter;
}

Appearance * Builder::FindAppearance(std::string const & name)
{
    AppearanceMap::iterator pEntry = appearances_.find(name);
    Appearance * pAppearance;

    if (pEntry != appearances_.end())
        pAppearance = pEntry->second;
    else
        pAppearance= nullptr;

    return pAppearance;
}

Environment * Builder::FindEnvironment(std::string const & name)
{
    EnvironmentMap::iterator pEntry = environments_.find(name);
    Environment * pEnvironment;

    if (pEntry != environments_.end())
        pEnvironment = pEntry->second;
    else
        pEnvironment= nullptr;

    return pEnvironment;
}

EmitterVolume * Builder::FindEmitterVolume(std::string const & name)
{
    EmitterVolumeMap::iterator pEntry = emitterVolumes_.find(name);
    EmitterVolume * pEmitterVolume;

    if (pEntry != emitterVolumes_.end())
        pEmitterVolume = pEntry->second;
    else
        pEmitterVolume= nullptr;

    return pEmitterVolume;
}

Environment::BouncePlaneList * Builder::FindBouncePlaneList(std::string const & name)
{
    BouncePlaneListMap::iterator   pEntry = bouncePlaneLists_.find(name);
    Environment::BouncePlaneList * pBouncePlaneList;

    if (pEntry != bouncePlaneLists_.end())
        pBouncePlaneList = pEntry->second;
    else
        pBouncePlaneList= nullptr;

    return pBouncePlaneList;
}

Environment::ClipPlaneList * Builder::FindClipPlaneList(std::string const & name)
{
    ClipPlaneListMap::iterator   pEntry = clipPlaneLists_.find(name);
    Environment::ClipPlaneList * pClipPlaneList;

    if (pEntry != clipPlaneLists_.end())
        pClipPlaneList = pEntry->second;
    else
        pClipPlaneList= nullptr;

    return pClipPlaneList;
}

_D3DMATERIAL11 * Builder::FindMaterial(std::string const & name)
{
    MaterialMap::iterator pEntry = materials_.find(name);
    _D3DMATERIAL11 *        pMaterial;

    if (pEntry != materials_.end())
        pMaterial = pEntry->second;
    else
        pMaterial= nullptr;

    return pMaterial;
}

ID3D11Texture2D * Builder::FindTexture(std::string const & name)
{
    TextureMap::iterator pEntry = textures_.find(name);
    ID3D11Texture2D * pTexture;

    if (pEntry != textures_.end())
        pTexture = pEntry->second;
    else
        pTexture= nullptr;

    return pTexture;
}

Builder::EmitterMap::iterator Builder::AddEmitter(std::string const & name, BasicEmitter * pEmitter)
{
    std::pair<EmitterMap::iterator, bool> rv;

    rv = emitters_.insert(EmitterMap::value_type(name, pEmitter));
    assert(rv.second);

    return rv.first;
}

Builder::EmitterVolumeMap::iterator Builder::AddEmitterVolume(std::string const & name, EmitterVolume * pVolume)
{
    std::pair<EmitterVolumeMap::iterator, bool> rv;

    rv = emitterVolumes_.insert(EmitterVolumeMap::value_type(name, pVolume));
    assert(rv.second);

    return rv.first;
}

Builder::EnvironmentMap::iterator Builder::AddEnvironment(std::string const & name, Environment * pEnvironment)
{
    std::pair<EnvironmentMap::iterator, bool> rv;

    rv = environments_.insert(EnvironmentMap::value_type(name, pEnvironment));
    assert(rv.second);

    return rv.first;
}

Builder::AppearanceMap::iterator Builder::AddAppearance(std::string const & name, Appearance * pAppearance)
{
    std::pair<AppearanceMap::iterator, bool> rv;

    rv = appearances_.insert(AppearanceMap::value_type(name, pAppearance));
    assert(rv.second);

    return rv.first;
}

Builder::BouncePlaneListMap::iterator Builder::AddBouncePlaneList(std::string const & name, Environment::BouncePlaneList * pList)
{
    std::pair<BouncePlaneListMap::iterator, bool> rv;

    rv = bouncePlaneLists_.insert(BouncePlaneListMap::value_type(name, pList));
    assert(rv.second);

    return rv.first;
}

Builder::ClipPlaneListMap::iterator Builder::AddClipPlaneList(std::string const & name, Environment::ClipPlaneList * pList)
{
    std::pair<ClipPlaneListMap::iterator, bool> rv;

    rv = clipPlaneLists_.insert(ClipPlaneListMap::value_type(name, pList));
    assert(rv.second);

    return rv.first;
}

Builder::MaterialMap::iterator Builder::AddMaterial(std::string const & name, _D3DMATERIAL11 * pMaterial)
{
    std::pair<MaterialMap::iterator, bool> rv;

    rv = materials_.insert(MaterialMap::value_type(name, pMaterial));
    assert(rv.second);

    return rv.first;
}

Builder::TextureMap::iterator Builder::AddTexture(std::string const & name, ID3D11Texture2D * pTexture)
{
    std::pair<TextureMap::iterator, bool> rv;

    rv = textures_.insert(TextureMap::value_type(name, pTexture));
    assert(rv.second);

    return rv.first;
}
} // namespace Confetti
