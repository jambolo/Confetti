/** @file *//********************************************************************************************************

                                                   Configuration.h

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Configuration.h#10 $

    $NoKeywords: $

********************************************************************************************************************/

#pragma once

#include <d3dx9math.h>
#include <map>
#include <Misc/Types.h>
#include <string>
#include <vector>

struct IXMLDOMDocument2;
struct IXMLDOMElement;
struct IXMLDOMNodeVector;
struct IXMLDOMNode;

namespace Confetti
{
/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! A generic configuration for ParticleSystem

class Configuration
{
public:

    //! Particle configuration
    class Particle
    {
public:

        float m_Lifetime;
        float m_Age;
        D3DXVECTOR3 m_Position;
        D3DXVECTOR3 m_Velocity;
        D3DXCOLOR m_Color;
        float m_Radius;
        float m_Rotation;
        D3DXQUATERNION m_Orientation;
    };

    //! Emitter configuration
    class Emitter
    {
public:

        typedef std::vector<Particle> ParticleVector;

        std::string m_Name;
        std::string m_Type;
        std::string m_Volume;
        std::string m_Environment;
        std::string m_Appearance;
        float m_MinSpeed;
        float m_MaxSpeed;
        int m_Count;
        float m_Lifetime;
        float m_Spread;
        D3DXCOLOR m_Color;
        float m_Radius;
        bool m_Sorted;
        D3DXVECTOR3 m_Position;
        D3DXQUATERNION m_Orientation;
        D3DXVECTOR3 m_Velocity;
        ParticleVector m_ParticleVector;
    };

    //! Emitter volume configuration
    class EmitterVolume
    {
public:

        std::string m_Name;
        std::string m_Type;
        float m_Length;
        float m_Width;
        float m_Height;
        float m_Depth;
        float m_Radius;
    };

    //! Environment configuration
    class Environment
    {
public:
        std::string m_Name;
        D3DXVECTOR3 m_Gravity;
        D3DXVECTOR3 m_WindVelocity;
        D3DXVECTOR3 m_Gustiness;
        float m_AirFriction;
        std::string m_Bounce;
        std::string m_Clip;
    };

    //! Appearance configuration
    class Appearance
    {
public:
        std::string m_Name;
        D3DXCOLOR m_ColorChange;
        float m_RadiusChange;
        float m_RadialVelocity;
        std::string m_Texture;
        float m_Size;
    };

    //! Bounce plane configuration
    class BouncePlane
    {
public:
        D3DXPLANE m_Plane;
        float m_Dampening;
    };

    //! Clip plane list configuration
    class ClipPlaneList
    {
public:
        std::string m_Name;
        std::vector<D3DXPLANE> m_ClipPlanes;
    };

    //! Bounce plane list configuration
    class BouncePlaneList
    {
public:
        std::string m_Name;
        std::vector<BouncePlane> m_BouncePlanes;
    };

    //! Constructor
    Configuration();

    // Destructor
    virtual ~Configuration();

    typedef std::map<std::string, Emitter>            EmitterMap;               //!< A map of Emitters
    typedef std::map<std::string, EmitterVolume>      EmitterVolumeMap;         //!< A map of EmitterVolumes
    typedef std::map<std::string, Environment>        EnvironmentMap;           //!< A map of Environments
    typedef std::map<std::string, Appearance>         AppearanceMap;            //!< A map of Appearances
    typedef std::map<std::string, ClipPlaneList>      ClipPlaneListMap;         //!< A map of ClipPlaneLists
    typedef std::map<std::string, BouncePlaneList>    BouncePlaneListMap;       //!< A map of BouncePlaneLists

    //! @name  Object Configurations
    //@{
    EmitterMap m_Emitters;                      //!< Emitter configurations, indexed by name
    EmitterVolumeMap m_EmitterVolumes;          //!< EmitterVolume configurations, indexed by name
    EnvironmentMap m_Environments;              //!< Environment configurations, indexed by name
    AppearanceMap m_Appearances;                //!< Appearance configurations, indexed by name
    ClipPlaneListMap m_ClipPlaneLists;          //!< ClipPlaneList configurations, indexed by name
    BouncePlaneListMap m_BouncePlaneLists;      //!< BouncePlaneList configurations, indexed by name
    //@}
};

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! A configuration loaded from an XML file

class XmlConfiguration : public Configuration
{
public:

    //! Constructor
    XmlConfiguration(char const * sFilename = 0);

    //! Constructor
    XmlConfiguration(IXMLDOMDocument2 * pDoc);

    // Destructor
    virtual ~XmlConfiguration();

    //! Loads a configuration from an XML file. Returns @c true if successful.
    bool Load(char const * sFilename);

    //! Loads a configuration from an XML DOM. Returns @c true if successful.
    bool XmlConfiguration::Load(IXMLDOMDocument2 * pDocument);

    //! Saves the configuration to an XML file. Returns @c true if successful.
    bool Save(char const * sFilename);

    //! Saves the configuration to an XML DOM. Returns @c true if successful.
    bool Save(IXMLDOMDocument2 * pDocument);

private:

    static bool ProcessBouncePlane(IXMLDOMElement * pElement, uint32 context);
    static bool ProcessBouncePlaneList(IXMLDOMElement * pElement, uint32 context);
    static bool ProcessClipPlane(IXMLDOMElement * pElement, uint32 context);
    static bool ProcessClipPlaneList(IXMLDOMElement * pElement, uint32 context);
    static bool ProcessEnvironment(IXMLDOMElement * pElement, uint32 context);
    static bool ProcessAppearance(IXMLDOMElement * pElement, uint32 context);
    static bool ProcessVolume(IXMLDOMElement * pElement, uint32 context);
    static bool ProcessEmitter(IXMLDOMElement * pElement, uint32 context);
//	static void ProcessPointParticles( PointEmitter *		pEmitter,
//								  IXMLDOMNodeVector *		pNodeVector,
//								  int					numParticles,
//								  Configuration const *	pConfiguration,
//								  float					globalLifetime,
//								  uint32				globalColor,
//								  float					spread,
//								  float					minSpeed,
//								  float					maxSpeed );
//	static void ProcessStreakParticles( StreakEmitter *			pEmitter,
//										IXMLDOMNodeVector *		pNodeVector,
//										int						numParticles,
//										Configuration const *	pConfiguration,
//										float					globalLifetime,
//										uint32					globalColor,
//										float					spread,
//										float					minSpeed,
//										float					maxSpeed );
//	static void ProcessTexturedParticles( TexturedEmitter *     pEmitter,
//										  IXMLDOMNodeVector *   pNodeVector,
//										  int					numParticles,
//										  Configuration const * pConfiguration,
//										  float					globalLifetime,
//										  uint32				globalColor,
//										  float					spread,
//										  float					minSpeed,
//										  float					maxSpeed );
//	static void ProcessSphereParticles( SphereEmitter *			pEmitter,
//										IXMLDOMNodeVector *		pNodeVector,
//										int						numParticles,
//										Configuration const *	pConfiguration,
//										float					globalLifetime,
//										uint32					globalColor,
//										float					spread,
//										float					minSpeed,
//										float					maxSpeed );
//	static void ProcessParticle( IXMLDOMNode *	pNode,
//								 float *		pLifetime,
//								 float *		pAge,
//								 uint32 *		pColor,
//								 float *		pRadius,
//								 float *		pOrientation,
//								 D3DXVECTOR3 *		pVelocity,
//								 D3DXVECTOR3 *		pPosition );
};
} // namespace Confetti
