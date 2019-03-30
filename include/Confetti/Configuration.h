#if !defined(CONFETTI_CONFIGURATION_H)
#define CONFETTI_CONFIGURATION_H

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <map>
#include <string>
#include <vector>

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
        glm::quat orientation_;
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
        float gustiness_;
        float airFriction_;
        std::string surface_;
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
    class Surface
    {
public:
        glm::vec4 plane_;
        float dampening_;
    };

    //! Clip plane list configuration
    class ClipperList
    {
public:
        std::string name_;
        std::vector<glm::vec4> planes_;
    };

    //! Bounce plane list configuration
    class SurfaceList
    {
public:
        std::string name_;
        std::vector<Surface> planes_;
    };

    Configuration()          = default;
    virtual ~Configuration() = default;

    using EmitterMap       = std::map<std::string, Emitter>;            //!< A map of Emitters
    using EmitterVolumeMap = std::map<std::string, EmitterVolume>;       //!< A map of EmitterVolumes
    using EnvironmentMap   = std::map<std::string, Environment>;        //!< A map of Environments
    using AppearanceMap    = std::map<std::string, Appearance>;         //!< A map of Appearances
    using ClipperListMap = std::map<std::string, ClipperList>;       //!< A map of ClipperLists
    using SurfaceListMap   = std::map<std::string, SurfaceList>;   //!< A map of SurfaceLists

    //! @name  Object Configurations
    //@{
    EmitterMap emitters_;                      //!< Emitter configurations, indexed by name
    EmitterVolumeMap emitterVolumes_;          //!< EmitterVolume configurations, indexed by name
    EnvironmentMap environments_;              //!< Environment configurations, indexed by name
    AppearanceMap appearances_;                //!< Appearance configurations, indexed by name
    ClipperListMap clipperLists_;          //!< ClipperList configurations, indexed by name
    SurfaceListMap surfaceLists_;      //!< SurfaceList configurations, indexed by name
    //@}
};
} // namespace Confetti

#endif // !defined(CONFETTI_CONFIGURATION_H)
