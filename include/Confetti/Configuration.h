#if !defined(CONFETTI_CONFIGURATION_H)
#define CONFETTI_CONFIGURATION_H

#pragma once

#include <glm/fwd.hpp>
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
        float lifetime_ = 1.0f;
        float age_ = 0.0f;
        glm::vec3 position_{ 0.0f, 0.0f, 0.0f };
        glm::vec3 velocity_{ 0.0f, 0.0f, 0.0f };
        glm::vec4 color_{ 0.0f, 0.0f, 0.0f, 0.0f };
        float radius_ = 1.0f;
        float rotation_ = 0.0f;
        glm::quat orientation_{ 0.0f, 0.0f, 0.0f, 1.0f };
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
        float minSpeed_ = 0.0f;
        float maxSpeed_ = 0.0f;
        int count_ = 0;
        float lifetime_ = 1.0f;
        float spread_ = 0.0f;
        glm::vec4 color_{ 0.0f, 0.0f, 0.0f, 1.0f };
        float radius_ = 1.0f;
        bool sorted_ = false;
        glm::vec3 position_{ 0.0f, 0.0f, 0.0f };
        glm::quat orientation_{ 0.0f, 0.0f, 0.0f, 1.0f };
        glm::vec3 velocity_{ 0.0f, 0.0f, 0.0f };
        ParticleVector particles_;
    };

    //! Emitter volume configuration
    class EmitterVolume
    {
public:
        std::string name_;
        std::string type_;
        float length_ = 0.0f;
        float width_ = 0.0f;
        float height_ = 0.0f;
        float depth_ = 0.0f;
        float radius_ = 0.0f;
    };

    //! Environment configuration
    class Environment
    {
public:
        std::string name_;
        glm::vec3 gravity_{ 0.0f, 0.0f, 0.0f };
        glm::vec3 windVelocity_{ 0.0f, 0.0f, 0.0f };
        float gustiness_ = 0.0f;
        float airFriction_ = 0.0f;
        std::string surface_;
        std::string clip_;
    };

    //! Appearance configuration
    class Appearance
    {
public:
        std::string name_;
        glm::vec4 colorChange_{ 0.0f, 0.0f, 0.0f, 0.0f };
        float radiusChange_ = 0.0f;
        float radialVelocity_ = 0.0f;
        std::string texture_;
        float size_ = 0.0f;
    };

    //! Bounce plane configuration
    class Surface
    {
public:
        glm::vec4 plane_{ 1.0f, 0.0f, 0.0f, 0.0f };
        float dampening_ = 0.0f;
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
        std::vector<Surface> surfaces_;
    };

    Configuration()          = default;
    virtual ~Configuration() = default;

    using EmitterMap       = std::map<std::string, Emitter>;        //!< A map of Emitters
    using EmitterVolumeMap = std::map<std::string, EmitterVolume>;  //!< A map of EmitterVolumes
    using EnvironmentMap   = std::map<std::string, Environment>;    //!< A map of Environments
    using AppearanceMap    = std::map<std::string, Appearance>;     //!< A map of Appearances
    using ClipperListMap   = std::map<std::string, ClipperList>;    //!< A map of ClipperLists
    using SurfaceListMap   = std::map<std::string, SurfaceList>;    //!< A map of SurfaceLists

    //! @name  Object Configurations
    //@{
    EmitterMap emitters_;                       //!< Emitter configurations, indexed by name
    EmitterVolumeMap emitterVolumes_;           //!< EmitterVolume configurations, indexed by name
    EnvironmentMap environments_;               //!< Environment configurations, indexed by name
    AppearanceMap appearances_;                 //!< Appearance configurations, indexed by name
    ClipperListMap clipperLists_;               //!< ClipperList configurations, indexed by name
    SurfaceListMap surfaceLists_;               //!< SurfaceList configurations, indexed by name
    //@}
};
} // namespace Confetti

#endif // !defined(CONFETTI_CONFIGURATION_H)
