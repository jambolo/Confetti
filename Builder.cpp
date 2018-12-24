/** @file *//********************************************************************************************************

                                                     Builder.cpp

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Builder.cpp#7 $

    $NoKeywords: $

********************************************************************************************************************/

#include "PrecompiledHeaders.h"

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

namespace Confetti
{
/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Builder::Builder(uint32_t seed)
    : m_Rng(seed)
    , m_FRng(~seed)
    , m_DRng(seed ^ (uint32_t)this)
    , m_ORng(seed ^ ~(uint32_t)this)
{
    // Nothing to do
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Builder::~Builder()
{
    for (auto & e : m_Emitters)
    {
        delete e.second;
    }

    for (auto & v : m_EmitterVolumes)
    {
        delete v.second;
    }

    for (auto & e : m_Environments)
    {
        delete e.second;
    }

    for (auto & a : m_Appearances)
    {
        delete a.second;
    }

    for (auto & p : m_BouncePlaneLists)
    {
        delete p.second;
    }

    for (auto & p : m_ClipPlaneLists)
    {
        delete p.second;
    }

    for (auto & m : m_Materials)
    {
        delete m.second;
    }

    for (auto & t : m_Textures)
    {
        Wx::SafeRelease(t.second);
    }
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

std::auto_ptr<ParticleSystem> Builder::BuildParticleSystem(Configuration const & configuration,
                                                           IDirect3DDevice9 *    pD3dDevice,
                                                           Dxx::Camera const *   pCamera)
{
    std::auto_ptr<ParticleSystem> pPS = std::auto_ptr<ParticleSystem>(new ParticleSystem(pD3dDevice));

    // Build the bounce lists

    for (auto const & p : configuration.m_BouncePlaneLists)
    {
        BuildBouncePlaneList(p.second);
    }

    // Build the clip plane lists

    for (auto const & p : configuration.m_ClipPlaneLists)
    {
        BuildClipPlaneList(p.second);
    }

    // Build the emitter volumes

    for (auto const & v : configuration.m_EmitterVolumes)
    {
        BuildEmitterVolume(v.second);
    }

    // Build the environments

    for (auto const & e : configuration.m_Environments)
    {
        Environment * environment = BuildEnvironment(e.second);
        if (environment)
            pPS->Register(environment);
    }

    // Build the appearances

    for (auto const & a : configuration.m_Appearances)
    {
        Appearance * appearance = BuildAppearance(a.second, pD3dDevice, pCamera);
        if (appearance)
            pPS->Register(appearance);
    }

    // Build the emitters

    for (auto const & e : configuration.m_Emitters)
    {
        BasicEmitter * emitter = BuildEmitter(e.second, pD3dDevice);
        if (emitter)
            pPS->Register(emitter);
    }

    return pPS;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

std::auto_ptr<PointParticle> Builder::BuildPointParticles(
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

            D3DXVECTOR3 const direction = m_DRng.Get(emitterConfiguration.m_Spread);
            float const       speed     = m_FRng.Get(emitterConfiguration.m_MinSpeed,
                                                     emitterConfiguration.m_MaxSpeed);
            D3DXVECTOR3 const velocity = D3DXVECTOR3(-direction.z, direction.y, direction.x) * speed;

            paParticles[i].Initialize(emitterConfiguration.m_Lifetime,
                                      m_FRng.Get(emitterConfiguration.m_Lifetime),
                                      volume.Next(),
                                      velocity,
                                      emitterConfiguration.m_Color);
        }
    }
    else
    {
        assert(configurations.size() == n);

        for (int i = 0; i < n; i++)
        {
            Configuration::Particle const & configuration = configurations[i];      // Convenience

            paParticles[i].Initialize(configuration.m_Lifetime,
                                      configuration.m_Age,
                                      configuration.m_Position,
                                      configuration.m_Velocity,
                                      configuration.m_Color);
        }
    }

    return std::auto_ptr<PointParticle>(paParticles);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

std::auto_ptr<StreakParticle> Builder::BuildStreakParticles(
    int                                            n,
    Configuration::Emitter::ParticleVector const & configurations,
    Configuration::Emitter const &                 emitterConfiguration,
    EmitterVolume const &                          volume,
    Environment const &                            environment,
    Appearance const &                             appearance)
{
    StreakParticle * const paParticles = new StreakParticle[n];
    if (paParticles == 0)
        throw std::bad_alloc();

    // If there are no individual particle configurations, then generate the particles' characteristics from the
    // emitter configuration. Otherwise, use the individual configurations.

    if (configurations.size() == 0)
    {
        for (int i = 0; i < n; i++)
        {
            D3DXVECTOR3 const direction = m_DRng.Get(emitterConfiguration.m_Spread);
            float const       speed     = m_FRng.Get(emitterConfiguration.m_MinSpeed,
                                                     emitterConfiguration.m_MaxSpeed);
            D3DXVECTOR3 const velocity = D3DXVECTOR3(-direction.z, direction.y, direction.x) * speed;

            paParticles[i].Initialize(emitterConfiguration.m_Lifetime,
                                      m_FRng.Get(emitterConfiguration.m_Lifetime),
                                      volume.Next(),
                                      velocity,
                                      emitterConfiguration.m_Color);
        }
    }
    else
    {
        assert(configurations.size() == n);

        for (int i = 0; i < n; i++)
        {
            Configuration::Particle const & configuration = configurations[i];      // Convenience

            paParticles[i].Initialize(configuration.m_Lifetime,
                                      configuration.m_Age,
                                      configuration.m_Position,
                                      configuration.m_Velocity,
                                      configuration.m_Color);
        }
    }

    return std::auto_ptr<StreakParticle>(paParticles);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

std::auto_ptr<TexturedParticle> Builder::BuildTexturedParticles(
    int                                            n,
    Configuration::Emitter::ParticleVector const & configurations,
    Configuration::Emitter const &                 emitterConfiguration,
    EmitterVolume const &                          volume,
    Environment const &                            environment,
    Appearance const &                             appearance)
{
    TexturedParticle * const paParticles = new TexturedParticle[n];
    if (paParticles == 0)
        throw std::bad_alloc();

    // If there are no individual particle configurations, then generate the particles' characteristics from the
    // emitter configuration. Otherwise, use the individual configurations.

    if (configurations.size() == 0)
    {
        for (int i = 0; i < n; i++)
        {
            D3DXVECTOR3 const direction = m_DRng.Get(emitterConfiguration.m_Spread);
            float const       speed     = m_FRng.Get(emitterConfiguration.m_MinSpeed,
                                                     emitterConfiguration.m_MaxSpeed);
            D3DXVECTOR3 const velocity = D3DXVECTOR3(-direction.z, direction.y, direction.x) * speed;

            paParticles[i].Initialize(emitterConfiguration.m_Lifetime,
                                      m_FRng.Get(emitterConfiguration.m_Lifetime),
                                      volume.Next(),
                                      velocity,
                                      emitterConfiguration.m_Color,
                                      emitterConfiguration.m_Radius,
                                      m_FRng.Get((float)Math::TWO_PI));
        }
    }
    else
    {
        assert(configurations.size() == n);

        for (int i = 0; i < n; i++)
        {
            Configuration::Particle const & configuration = configurations[i];      // Convenience

            paParticles[i].Initialize(configuration.m_Lifetime,
                                      configuration.m_Age,
                                      configuration.m_Position,
                                      configuration.m_Velocity,
                                      configuration.m_Color,
                                      configuration.m_Radius,
                                      configuration.m_Rotation);
        }
    }

    return std::auto_ptr<TexturedParticle>(paParticles);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

std::auto_ptr<SphereParticle> Builder::BuildSphereParticles(
    int                                            n,
    Configuration::Emitter::ParticleVector const & configurations,
    Configuration::Emitter const &                 emitterConfiguration,
    EmitterVolume const &                          volume,
    Environment const &                            environment,
    Appearance const &                             appearance)
{
    SphereParticle * const paParticles = new SphereParticle[n];
    if (paParticles == 0)
        throw std::bad_alloc();

    // If there are no individual particle configurations, then generate the particles' characteristics from the
    // emitter configuration. Otherwise, use the individual configurations.

    if (configurations.size() == 0)
    {
        for (int i = 0; i < n; i++)
        {
            D3DXVECTOR3 const direction = m_DRng.Get(emitterConfiguration.m_Spread);
            float const       speed     = m_FRng.Get(emitterConfiguration.m_MinSpeed,
                                                     emitterConfiguration.m_MaxSpeed);
            D3DXVECTOR3 const velocity = D3DXVECTOR3(-direction.z, direction.y, direction.x) * speed;

            paParticles[i].Initialize(emitterConfiguration.m_Lifetime,
                                      m_FRng.Get(emitterConfiguration.m_Lifetime),
                                      volume.Next(),
                                      velocity,
                                      emitterConfiguration.m_Color,
                                      emitterConfiguration.m_Radius);
        }
    }
    else
    {
        assert(configurations.size() == n);

        for (int i = 0; i < n; i++)
        {
            Configuration::Particle const & configuration = configurations[i];      // Convenience

            paParticles[i].Initialize(configuration.m_Lifetime,
                                      configuration.m_Age,
                                      configuration.m_Position,
                                      configuration.m_Velocity,
                                      configuration.m_Color,
                                      configuration.m_Radius);
        }
    }

    return std::auto_ptr<SphereParticle>(paParticles);
}

/// ********************************************************************************************************************/
/// *																													*/
/// ********************************************************************************************************************/
//
// std::auto_ptr< EmitterParticle > Builder::BuildEmitterParticles(
//												int n,
//												Configuration::Emitter::ParticleVector const & configurations,
//												Configuration::Emitter const & emitterConfiguration,
//											    EmitterVolume const & volume,
//											    Environment const & environment,
//											    Appearance const & appearance )
// {
//	EmitterParticle * const	paParticles	= new EmitterParticle[ n ];
//	if ( paParticles == 0 ) throw std::bad_alloc();
//
//	// If there are no individual particle configurations, then generate the particles' characteristics from the
//	// emitter configuration. Otherwise, use the individual configurations.
//
//	if ( configurations.size() == 0 )
//	{
//		for ( int i = 0; i < n; i++ )
//		{
//			D3DXVECTOR3	const	direction	= m_DRng.Get( emitterConfiguration.m_Spread );
//			float const			speed		= m_FRng.Get( emitterConfiguration.m_MinSpeed,
//														  emitterConfiguration.m_MaxSpeed );
//			D3DXVECTOR3	const	velocity	= D3DXVECTOR3( -direction.z, direction.y, direction.x ) * speed;
//
//			paParticles[ i ].Initialize( emitterConfiguration.m_Lifetime,
//										 m_FRng.Get( emitterConfiguration.m_Lifetime ),
//										 volume.Next(),
//										 velocity,
//										 emitterConfiguration.m_Color,
//										 emitterConfiguration.m_Radius,
//										 m_ORng.Get() );
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
//			paParticles[ i ].Initialize( configuration.m_Lifetime,
//										 configuration.m_Age,
//										 configuration.m_Position,
//										 configuration.m_Velocity,
//										 configuration.m_Color,
//										 configuration.m_Radius,
//										 configuration.m_Orientation );
//		}
//	}
//
//
//	return std::auto_ptr< EmitterParticle >( paParticles );
// }

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

BasicEmitter * Builder::BuildEmitter(Configuration::Emitter const & configuration, IDirect3DDevice9 * pD3dDevice)
{
    // Prevent duplicate entries

    if (FindEmitter(configuration.m_Name) != 0)
        return 0;

    // class Configuration::Emitter
    // {
    // public:
    //
    //	typedef std::vector< Particle > ParticleVector;
    //
    //	std::string		m_Name;
    //	std::string		m_Type;
    //	std::string		m_Volume;
    //	std::string		m_Environment;
    //	std::string		m_Appearance;
    //	D3DXVECTOR3		m_Position;
    //	D3DXQUATERNION	m_Orientation;
    //	D3DXVECTOR3		m_Velocity;
    //	float			m_MinSpeed;
    //	float			m_MaxSpeed;
    //	int				m_Count;
    //	float			m_Lifetime;
    //	float			m_Spread;
    //	DWORD			m_Color;
    //	float			m_Radius;
    //	bool			m_Sorted;
    //	ParticleVector	m_ParticleVector;
    // };

    EmitterVolume * pVolume      = FindEmitterVolume(configuration.m_Volume);
    Environment *   pEnvironment = FindEnvironment(configuration.m_Environment);
    Appearance *    pAppearance  = FindAppearance(configuration.m_Appearance);

    BasicEmitter * pEmitter;

    if (configuration.m_Type == "point")
    {
        std::auto_ptr<PointParticle> qaParticles = BuildPointParticles(configuration.m_Count,
                                                                       configuration.m_ParticleVector,
                                                                       configuration,
                                                                       *pVolume,
                                                                       *pEnvironment,
                                                                       *pAppearance);

        pEmitter = new PointEmitter(pD3dDevice,
                                    qaParticles,
                                    pVolume,
                                    pEnvironment,
                                    pAppearance,
                                    configuration.m_Count,
                                    configuration.m_Sorted);
        if (pEmitter == 0)
            throw std::bad_alloc();
    }
    else if (configuration.m_Type == "streak")
    {
        std::auto_ptr<StreakParticle> qaParticles = BuildStreakParticles(configuration.m_Count,
                                                                         configuration.m_ParticleVector,
                                                                         configuration,
                                                                         *pVolume,
                                                                         *pEnvironment,
                                                                         *pAppearance);

        pEmitter = new StreakEmitter(pD3dDevice,
                                     qaParticles,
                                     pVolume,
                                     pEnvironment,
                                     pAppearance,
                                     configuration.m_Count,
                                     configuration.m_Sorted);
        if (pEmitter == 0)
            throw std::bad_alloc();
    }
    else if (configuration.m_Type == "textured")
    {
        std::auto_ptr<TexturedParticle> qaParticles = BuildTexturedParticles(configuration.m_Count,
                                                                             configuration.m_ParticleVector,
                                                                             configuration,
                                                                             *pVolume,
                                                                             *pEnvironment,
                                                                             *pAppearance);

        pEmitter = new TexturedEmitter(pD3dDevice,
                                       qaParticles,
                                       pVolume,
                                       pEnvironment,
                                       pAppearance,
                                       configuration.m_Count,
                                       configuration.m_Sorted);
        if (pEmitter == 0)
            throw std::bad_alloc();
    }
    else if (configuration.m_Type == "sphere")
    {
        std::auto_ptr<SphereParticle> qaParticles = BuildSphereParticles(configuration.m_Count,
                                                                         configuration.m_ParticleVector,
                                                                         configuration,
                                                                         *pVolume,
                                                                         *pEnvironment,
                                                                         *pAppearance);

        pEmitter = new SphereEmitter(pD3dDevice,
                                     qaParticles,
                                     pVolume,
                                     pEnvironment,
                                     pAppearance,
                                     configuration.m_Count,
                                     configuration.m_Sorted);
        if (pEmitter == 0)
            throw std::bad_alloc();
    }
//	else if ( configuration.m_Type == "emitter" )
//	{
//		std::auto_ptr< EmitterParticle >	qaParticles = BuildEmitterParticles( configuration.m_Count,
//																		         configuration.m_ParticleVector,
//																				 configuration,
//																		         *pVolume,
//																		         *pEnvironment,
//																		         *pAppearance );
//
//		pEmitter = new EmitterEmitter( pD3dDevice,
//									 qaParticles,
//									 pVolume,
//									 pEnvironment,
//									 pAppearance,
//									 configuration.m_Count,
//									 configuration.m_Sorted );
//		if ( pEmitter == 0 ) throw std::bad_alloc();
//	}
    else
    {
        // Invalid emitter type
        pEmitter = 0;
    }

    // Manage the emitter

    if (pEmitter != 0)
        AddEmitter(configuration.m_Name, pEmitter);

    return pEmitter;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Appearance * Builder::BuildAppearance(Configuration::Appearance const & configuration,
                                      IDirect3DDevice9 *                pD3dDevice,
                                      Dxx::Camera const *               pCamera)
{
    // Prevent duplicate entries

    if (FindAppearance(configuration.m_Name) != 0)
        return 0;

    //	class Configuration::Appearance
    //	{
    //	public:
    //		std::string	m_Name;
    //		D3DXCOLOR	m_ColorChange;
    //		float		m_RadiusChange;
    //		float		m_RadialVelocity;
    //		std::string	m_Texture;
    //		float		m_Size;
    //	};

    IDirect3DTexture9 * pTexture;
    D3DMATERIAL9 *      pMaterial;

    // Create the texture (if specified)

    if (!configuration.m_Texture.empty())
    {
        // If the texture is not already created, then create it. Otherwise, use the existing one.

        pTexture = FindTexture(configuration.m_Texture);
        if (pTexture == 0)
        {
            HRESULT hr;
            hr = D3DXCreateTextureFromFile(pD3dDevice, configuration.m_Texture.c_str(), &pTexture);
            assert_succeeded(hr);

            AddTexture(configuration.m_Texture, pTexture);
        }
    }
    else
    {
        pTexture = 0;   // No texture specified
    }

//	// Create the material (if specified)
//
//	if ( !configuration.m_Material.empty() )
//	{
//		// If the material is not already created, then create it. Otherwise, use the existing one.
//
//		pMaterial = FindMaterial( configuration.m_Material );
//		if ( pMaterial == 0 )
//		{
//		}
//	}
//	else
//	{
    pMaterial = 0;      // No material specified
//	}

    Appearance * const pAppearance = new Appearance(pCamera,
                                                    configuration.m_ColorChange,
                                                    configuration.m_RadiusChange,
                                                    configuration.m_RadialVelocity,
                                                    pMaterial,
                                                    pTexture,
                                                    configuration.m_Size);

    if (pAppearance == 0)
        throw std::bad_alloc();

    AddAppearance(configuration.m_Name, pAppearance);

    return pAppearance;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

EmitterVolume * Builder::BuildEmitterVolume(Configuration::EmitterVolume const & configuration)
{
    // Prevent duplicate entries

    if (FindEmitterVolume(configuration.m_Name) != 0)
        return 0;

    //	class Configuration::EmitterVolume
    //	{
    //	public:
    //
    //		std::string	m_Name;
    //		std::string	m_Type;
    //		float		m_Length;
    //		float		m_Width;
    //		float		m_Height;
    //		float		m_Depth;
    //		float		m_Radius;
    //	};

    EmitterVolume * pVolume;

    if (configuration.m_Type == "point")
    {
        pVolume = new EmitterPoint(m_Rng.Get());
    }
    else if (configuration.m_Type == "line")
    {
        pVolume = new EmitterLine(m_Rng.Get(), configuration.m_Length);
    }
    else if (configuration.m_Type == "rectangle")
    {
        pVolume = new EmitterRectangle(m_Rng.Get(),
                                       D3DXVECTOR2(configuration.m_Width, configuration.m_Height));
    }
    else if (configuration.m_Type == "circle")
    {
        pVolume = new EmitterCircle(m_Rng.Get(), configuration.m_Radius);
    }
    else if (configuration.m_Type == "sphere")
    {
        pVolume = new EmitterSphere(m_Rng.Get(), configuration.m_Radius);
    }
    else if (configuration.m_Type == "box")
    {
        pVolume = new EmitterBox(m_Rng.Get(),
                                 D3DXVECTOR3(configuration.m_Width,
                                             configuration.m_Height,
                                             configuration.m_Depth));
    }
    else if (configuration.m_Type == "cylinder")
    {
        pVolume = new EmitterCylinder(m_Rng.Get(), configuration.m_Radius, configuration.m_Height);
    }
    else if (configuration.m_Type == "cone")
    {
        pVolume = new EmitterCone(m_Rng.Get(), configuration.m_Radius, configuration.m_Height);
    }
    else
    {
        // Invalid configuration.m_Type
        pVolume = 0;
    }

    if (pVolume != 0)
        AddEmitterVolume(configuration.m_Name, pVolume);

    return pVolume;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Environment * Builder::BuildEnvironment(Configuration::Environment const & configuration)
{
    // Prevent duplicate entries

    if (FindEnvironment(configuration.m_Name) != 0)
        return 0;

    //	class Configuration::Environment
    //	{
    //	public:
    //		std::string	m_Name;
    //		D3DXVECTOR3	m_Gravity;
    //		D3DXVECTOR3	m_WindVelocity;
    //		D3DXVECTOR3	m_Gustiness;
    //		float		m_AirFriction;
    //		std::string	m_Bounce;
    //		std::string	m_Clip;
    //	};

    Environment::BouncePlaneList * pBouncePlaneList = FindBouncePlaneList(configuration.m_Bounce);
    Environment::ClipPlaneList *   pClipPlaneList   = FindClipPlaneList(configuration.m_Clip);

    Environment * const pEnvironment = new Environment(configuration.m_Gravity,
                                                       configuration.m_WindVelocity,
                                                       configuration.m_AirFriction,
                                                       configuration.m_Gustiness,
                                                       pBouncePlaneList, pClipPlaneList);
    if (pEnvironment == 0)
        throw std::bad_alloc();

    AddEnvironment(configuration.m_Name, pEnvironment);

    return pEnvironment;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Environment::BouncePlaneList * Builder::BuildBouncePlaneList(Configuration::BouncePlaneList const & configuration)
{
    // Prevent duplicate entries

    if (FindBouncePlaneList(configuration.m_Name) != 0)
        return 0;

    //	class Configuration::BouncePlaneList
    //	{
    //	public:
    //		std::string					m_Name;
    //		std::vector< BouncePlane >	m_BouncePlanes;
    //	};
    //
    //	class Configuration::BouncePlane
    //	{
    //	public:
    //		D3DXPLANE	m_Plane;
    //		float		m_Dampening;
    //	};

    Environment::BouncePlaneList * const pList = new Environment::BouncePlaneList;
    if (pList == 0)
        throw std::bad_alloc();

    pList->resize(configuration.m_BouncePlanes.size());
    for (size_t i = 0; i < configuration.m_BouncePlanes.size(); i++)
    {
        (*pList)[i].m_Plane     = configuration.m_BouncePlanes[i].m_Plane;
        (*pList)[i].m_Dampening = configuration.m_BouncePlanes[i].m_Dampening;
    }

    AddBouncePlaneList(configuration.m_Name, pList);

    return pList;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Environment::ClipPlaneList * Builder::BuildClipPlaneList(Configuration::ClipPlaneList const & configuration)
{
    // Prevent duplicate entries

    if (FindClipPlaneList(configuration.m_Name) != 0)
        return 0;

    //	class Configuration::ClipPlaneList
    //	{
    //	public:
    //		std::string					m_Name;
    //		std::vector< D3DXPLANE >	m_ClipPlanes;
    //	};

    Environment::ClipPlaneList * const pList = new Environment::ClipPlaneList();
    if (pList == 0)
        throw std::bad_alloc();

    pList->resize(configuration.m_ClipPlanes.size());
    for (size_t i = 0; i < configuration.m_ClipPlanes.size(); i++)
    {
        (*pList)[i] = configuration.m_ClipPlanes[i];
    }

    AddClipPlaneList(configuration.m_Name, pList);

    return pList;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

BasicEmitter * Builder::FindEmitter(std::string const & name)
{
    EmitterMap::iterator const pEntry = m_Emitters.find(name);
    BasicEmitter * pEmitter;

    if (pEntry != m_Emitters.end())
        pEmitter = pEntry->second;
    else
        pEmitter = 0;

    return pEmitter;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Appearance * Builder::FindAppearance(std::string const & name)
{
    AppearanceMap::iterator const pEntry = m_Appearances.find(name);
    Appearance * pAppearance;

    if (pEntry != m_Appearances.end())
        pAppearance = pEntry->second;
    else
        pAppearance = 0;

    return pAppearance;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Environment * Builder::FindEnvironment(std::string const & name)
{
    EnvironmentMap::iterator const pEntry = m_Environments.find(name);
    Environment * pEnvironment;

    if (pEntry != m_Environments.end())
        pEnvironment = pEntry->second;
    else
        pEnvironment = 0;

    return pEnvironment;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

EmitterVolume * Builder::FindEmitterVolume(std::string const & name)
{
    EmitterVolumeMap::iterator const pEntry = m_EmitterVolumes.find(name);
    EmitterVolume * pEmitterVolume;

    if (pEntry != m_EmitterVolumes.end())
        pEmitterVolume = pEntry->second;
    else
        pEmitterVolume = 0;

    return pEmitterVolume;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Environment::BouncePlaneList * Builder::FindBouncePlaneList(std::string const & name)
{
    BouncePlaneListMap::iterator const pEntry = m_BouncePlaneLists.find(name);
    Environment::BouncePlaneList *     pBouncePlaneList;

    if (pEntry != m_BouncePlaneLists.end())
        pBouncePlaneList = pEntry->second;
    else
        pBouncePlaneList = 0;

    return pBouncePlaneList;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Environment::ClipPlaneList * Builder::FindClipPlaneList(std::string const & name)
{
    ClipPlaneListMap::iterator const pEntry = m_ClipPlaneLists.find(name);
    Environment::ClipPlaneList *     pClipPlaneList;

    if (pEntry != m_ClipPlaneLists.end())
        pClipPlaneList = pEntry->second;
    else
        pClipPlaneList = 0;

    return pClipPlaneList;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

D3DMATERIAL9 * Builder::FindMaterial(std::string const & name)
{
    MaterialMap::iterator const pEntry = m_Materials.find(name);
    D3DMATERIAL9 * pMaterial;

    if (pEntry != m_Materials.end())
        pMaterial = pEntry->second;
    else
        pMaterial = 0;

    return pMaterial;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

IDirect3DTexture9 * Builder::FindTexture(std::string const & name)
{
    TextureMap::iterator const pEntry = m_Textures.find(name);
    IDirect3DTexture9 *        pTexture;

    if (pEntry != m_Textures.end())
        pTexture = pEntry->second;
    else
        pTexture = 0;

    return pTexture;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Builder::EmitterMap::iterator Builder::AddEmitter(std::string const & name, BasicEmitter * pEmitter)
{
    std::pair<EmitterMap::iterator, bool> rv;

    rv = m_Emitters.insert(EmitterMap::value_type(name, pEmitter));
    assert(rv.second);

    return rv.first;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Builder::EmitterVolumeMap::iterator Builder::AddEmitterVolume(std::string const & name, EmitterVolume * pVolume)
{
    std::pair<EmitterVolumeMap::iterator, bool> rv;

    rv = m_EmitterVolumes.insert(EmitterVolumeMap::value_type(name, pVolume));
    assert(rv.second);

    return rv.first;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Builder::EnvironmentMap::iterator Builder::AddEnvironment(std::string const & name, Environment * pEnvironment)
{
    std::pair<EnvironmentMap::iterator, bool> rv;

    rv = m_Environments.insert(EnvironmentMap::value_type(name, pEnvironment));
    assert(rv.second);

    return rv.first;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Builder::AppearanceMap::iterator Builder::AddAppearance(std::string const & name, Appearance * pAppearance)
{
    std::pair<AppearanceMap::iterator, bool> rv;

    rv = m_Appearances.insert(AppearanceMap::value_type(name, pAppearance));
    assert(rv.second);

    return rv.first;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Builder::BouncePlaneListMap::iterator Builder::AddBouncePlaneList(std::string const & name, Environment::BouncePlaneList * pList)
{
    std::pair<BouncePlaneListMap::iterator, bool> rv;

    rv = m_BouncePlaneLists.insert(BouncePlaneListMap::value_type(name, pList));
    assert(rv.second);

    return rv.first;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Builder::ClipPlaneListMap::iterator Builder::AddClipPlaneList(std::string const & name, Environment::ClipPlaneList * pList)
{
    std::pair<ClipPlaneListMap::iterator, bool> rv;

    rv = m_ClipPlaneLists.insert(ClipPlaneListMap::value_type(name, pList));
    assert(rv.second);

    return rv.first;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Builder::MaterialMap::iterator Builder::AddMaterial(std::string const & name, _D3DMATERIAL9 * pMaterial)
{
    std::pair<MaterialMap::iterator, bool> rv;

    rv = m_Materials.insert(MaterialMap::value_type(name, pMaterial));
    assert(rv.second);

    return rv.first;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Builder::TextureMap::iterator Builder::AddTexture(std::string const & name, IDirect3DTexture9 * pTexture)
{
    std::pair<TextureMap::iterator, bool> rv;

    rv = m_Textures.insert(TextureMap::value_type(name, pTexture));
    assert(rv.second);

    return rv.first;
}
} // namespace Confetti
