#pragma once

#if !defined(CONFETTI_CONFIGURATION_H)
#define CONFETTI_CONFIGURATION_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct IXMLDOMDocument2;
struct IXMLDOMElement;
struct IXMLDOMNodeVector;
struct IXMLDOMNode;

namespace Confetti
{
//! A generic configuration for ParticleSystem
class Configuration
{
public:

    //! Particle configuration
    class Particle
    {
public:

        float lifetime_;
        float age_;
        glm::vec3 position_;
        glm::vec3 velocity_;
        glm::vec4 color_;
        float radius_;
        float rotation_;
        glm::vec4 orientation_;
    };

    //! Emitter configuration
    class Emitter
    {
public:

        using ParticleVector = std::vector<Particle>;

        std::string name_;
        std::string type_;
        std::string volume_;
        std::string environment_;
        std::string appearance_;
        float minSpeed_;
        float maxSpeed_;
        int count_;
        float lifetime_;
        float spread_;
        glm::vec4 color_;
        float radius_;
        bool sorted_;
        glm::vec3 position_;
        glm::quat orientation_;
        glm::vec3 velocity_;
        ParticleVector particleVector_;
    };

    //! Emitter volume configuration
    class EmitterVolume
    {
public:

        std::string name_;
        std::string type_;
        float length_;
        float width_;
        float height_;
        float depth_;
        float radius_;
    };

    //! Environment configuration
    class Environment
    {
public:
        std::string name_;
        glm::vec3 gravity_;
        glm::vec3 windVelocity_;
        glm::vec3 gustiness_;
        float airFriction_;
        std::string bounce_;
        std::string clip_;
    };

    //! Appearance configuration
    class Appearance
    {
public:
        std::string name_;
        glm::vec4 colorChange_;
        float radiusChange_;
        float radialVelocity_;
        std::string texture_;
        float size_;
    };

    //! Bounce plane configuration
    class BouncePlane
    {
public:
        glm::vec4 plane_;
        float dampening_;
    };

    //! Clip plane list configuration
    class ClipPlaneList
    {
public:
        std::string name_;
        std::vector<glm::vec4> clipPlanes_;
    };

    //! Bounce plane list configuration
    class BouncePlaneList
    {
public:
        std::string name_;
        std::vector<BouncePlane> bouncePlanes_;
    };

    //! Constructor.
    Configuration() = default;

    //! Destructor.
    virtual ~Configuration() = default;

    using EmitterMap         = std::map<std::string, Emitter>;          //!< A map of Emitters
    using EmitterVolumeMap   = std::map<std::string, EmitterVolume>;     //!< A map of EmitterVolumes
    using EnvironmentMap     = std::map<std::string, Environment>;      //!< A map of Environments
    using AppearanceMap      = std::map<std::string, Appearance>;       //!< A map of Appearances
    using ClipPlaneListMap   = std::map<std::string, ClipPlaneList>;     //!< A map of ClipPlaneLists
    using BouncePlaneListMap = std::map<std::string, BouncePlaneList>;     //!< A map of BouncePlaneLists

    //! @name  Object Configurations
    //@{
    EmitterMap emitters_;                      //!< Emitter configurations, indexed by name
    EmitterVolumeMap emitterVolumes_;          //!< EmitterVolume configurations, indexed by name
    EnvironmentMap environments_;              //!< Environment configurations, indexed by name
    AppearanceMap appearances_;                //!< Appearance configurations, indexed by name
    ClipPlaneListMap clipPlaneLists_;          //!< ClipPlaneList configurations, indexed by name
    BouncePlaneListMap bouncePlaneLists_;      //!< BouncePlaneList configurations, indexed by name
    //@}
};

//! A configuration loaded from an XML file
class XmlConfiguration : public Configuration
{
public:

    //! Constructor.
    XmlConfiguration(char const * sFilename = nullptr);

    //! Constructor.
    XmlConfiguration(IXMLDOMDocument2 * pDoc);

    //! Destructor.
    virtual ~XmlConfiguration() override = default;

    //! Loads a configuration from an XML file. Returns true if successful.
    bool Load(char const * sFilename);

    //! Loads a configuration from an XML DOM. Returns true if successful.
    bool XmlConfiguration::Load(IXMLDOMDocument2 * pDocument);

    //! Saves the configuration to an XML file. Returns true if successful.
    bool Save(char const * sFilename);

    //! Saves the configuration to an XML DOM. Returns true if successful.
    bool Save(IXMLDOMDocument2 * pDocument);

private:

    static bool ProcessBouncePlane(IXMLDOMElement * pElement, uintptr_t context);
    static bool ProcessBouncePlaneList(IXMLDOMElement * pElement, uintptr_t context);
    static bool ProcessClipPlane(IXMLDOMElement * pElement, uintptr_t context);
    static bool ProcessClipPlaneList(IXMLDOMElement * pElement, uintptr_t context);
    static bool ProcessEnvironment(IXMLDOMElement * pElement, uintptr_t context);
    static bool ProcessAppearance(IXMLDOMElement * pElement, uintptr_t context);
    static bool ProcessVolume(IXMLDOMElement * pElement, uintptr_t context);
    static bool ProcessEmitter(IXMLDOMElement * pElement, uintptr_t context);
//         static void ProcessPointParticles(PointEmitter *        pEmitter,
//                                           IXMLDOMNodeVector *   pNodeVector,
//                                           int                   numParticles,
//                                           Configuration const * pConfiguration,
//                                           float                 globalLifetime,
//                                           uint32_t              globalColor,
//                                           float                 spread,
//                                           float                 minSpeed,
//                                           float                 maxSpeed);
//         static void ProcessStreakParticles(StreakEmitter *       pEmitter,
//                                            IXMLDOMNodeVector *   pNodeVector,
//                                            int                   numParticles,
//                                            Configuration const * pConfiguration,
//                                            float                 globalLifetime,
//                                            uint32_t              globalColor,
//                                            float                 spread,
//                                            float                 minSpeed,
//                                            float                 maxSpeed);
//         static void ProcessTexturedParticles(TexturedEmitter *     pEmitter,
//                                              IXMLDOMNodeVector *   pNodeVector,
//                                              int                   numParticles,
//                                              Configuration const * pConfiguration,
//                                              float                 globalLifetime,
//                                              uint32_t              globalColor,
//                                              float                 spread,
//                                              float                 minSpeed,
//                                              float                 maxSpeed);
//         static void ProcessSphereParticles(SphereEmitter *       pEmitter,
//                                            IXMLDOMNodeVector *   pNodeVector,
//                                            int                   numParticles,
//                                            Configuration const * pConfiguration,
//                                            float                 globalLifetime,
//                                            uint32_t              globalColor,
//                                            float                 spread,
//                                            float                 minSpeed,
//                                            float                 maxSpeed);
//         static void ProcessParticle(IXMLDOMNode * pNode,
//                                     float *       pLifetime,
//                                     float *       pAge,
//                                     uint32_t *    pColor,
//                                     float *       pRadius,
//                                     float *       pOrientation,
//                                     glm::vec4 * pVelocity,
//                                     glm::vec4 * pPosition);
};

//! A configuration loaded from JSON
class JsonConfiguration : public Configuration
{
public:

    //! Constructor.
    JsonConfiguration(nlohmann::json json);

    //! Destructor.
    virtual ~JsonConfiguration() override;

    //! Saves the configuration to a JSON object
    nlohmann::json toJson();
};
} // namespace Confetti

#endif // !defined(CONFETTI_CONFIGURATION_H)
