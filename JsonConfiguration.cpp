#include "JsonConfiguration.h"

#include <fstream>

using json = nlohmann::json;

namespace nlohmann
{
template <>
struct adl_serializer<glm::vec4>
{
    static void from_json(json const & j, glm::vec4 & v)
    {
        j[0].get_to(v.x);
        j[1].get_to(v.y);
        j[2].get_to(v.z);
        j[3].get_to(v.w);
    }
    static void to_json(json & j, glm::vec4 const & v)
    {
        j = json::array({ v.x, v.y, v.z, v.w });
    }
};

template <>
struct adl_serializer<glm::vec3>
{
    static void from_json(json const & j, glm::vec3 & v)
    {
        j[0].get_to(v.x);
        j[1].get_to(v.y);
        j[2].get_to(v.z);
    }
    static void to_json(json & j, glm::vec3 const & v)
    {
        j = json::array({ v.x, v.y, v.z });
    }
};

template <>
struct adl_serializer<glm::quat>
{
    static void from_json(json const & j, glm::quat & q)
    {
        j[0].get_to(q.x);
        j[1].get_to(q.y);
        j[2].get_to(q.z);
        j[3].get_to(q.w);
    }
    static void to_json(json & j, glm::quat const & q)
    {
        j = json::array({ q.x, q.y, q.z, q.w });
    }
};
} // namespace nlohmann

namespace Confetti
{
static void from_json(json const & j, Configuration::Particle & particle)
{
    j.at("lifetime").get_to(particle.lifetime_);
    j.at("age").get_to(particle.age_);
    j.at("position").get_to(particle.position_);
    j.at("velocity").get_to(particle.velocity_);
    j.at("color").get_to(particle.color_);
    j.at("radius").get_to(particle.radius_);
    j.at("rotation").get_to(particle.rotation_);
    j.at("orientation").get_to(particle.orientation_);
}

static void to_json(json & j, Configuration::Particle const & particle)
{
    j = json{
        { "lifetime", particle.lifetime_ },
        { "age", particle.age_ },
        { "position", particle.position_ },
        { "velocity", particle.velocity_ },
        { "color", particle.color_ },
        { "radius", particle.radius_ },
        { "rotation", particle.rotation_ },
        { "orientation", particle.orientation_ }
    };
}

static void from_json(json const & j, Configuration::ClipPlaneList & list)
{
    j.at("name").get_to(list.name_);
    j.at("bouncePlanes").get_to(list.planes_);
}

static void to_json(json & j, Configuration::ClipPlaneList const & list)
{
    j = json{
        { "name", list.name_ },
        { "bouncePlanes", list.planes_ }
    };
}

static void from_json(json const & j, Configuration::BouncePlane & bouncePlane)
{
    j.at("plane").get_to(bouncePlane.plane_);
    j.at("dampening").get_to(bouncePlane.dampening_);
}

static void to_json(json & j, Configuration::BouncePlane const & bouncePlane)
{
    j = json{
        { "plane", bouncePlane.plane_ },
        { "dampening", bouncePlane.dampening_ }
    };
}

static void from_json(json const & j, Configuration::BouncePlaneList & list)
{
    j.at("name").get_to(list.name_);
    j.at("bouncePlanes").get_to(list.planes_);
}

static void to_json(json & j, Configuration::BouncePlaneList const & list)
{
    j = json{
        { "name", list.name_ },
        { "bouncePlanes", list.planes_ }
    };
}

static void from_json(json const & j, Configuration::Appearance & appearance)
{
    j.at("name").get_to(appearance.name_);
    j.at("colorChange").get_to(appearance.colorChange_);
    j.at("radiusChange").get_to(appearance.radiusChange_);
    j.at("radialVelocity").get_to(appearance.radialVelocity_);
    j.at("texture").get_to(appearance.texture_);
    j.at("size").get_to(appearance.size_);
}

static void to_json(json & j, Configuration::Appearance const & appearance)
{
    j = json{
        { "name", appearance.name_ },
        { "colorChange", appearance.colorChange_ },
        { "radiusChange", appearance.radiusChange_ },
        { "radialVelocity", appearance.radialVelocity_ },
        { "texture", appearance.texture_ },
        { "size", appearance.size_ }
    };
}

static void from_json(json const & j, Configuration::Environment & environment)
{
    j.at("name").get_to(environment.name_);
    j.at("gravity").get_to(environment.gravity_);
    j.at("windVelocity").get_to(environment.windVelocity_);
    j.at("gustiness").get_to(environment.gustiness_);
    j.at("airFriction").get_to(environment.airFriction_);
    j.at("bounce").get_to(environment.bounce_);
    j.at("clip").get_to(environment.clip_);
}

static void to_json(json & j, Configuration::Environment const & environment)
{
    j = json{
        { "name", environment.name_ },
        { "gravity", environment.gravity_ },
        { "windVelocity", environment.windVelocity_ },
        { "gustiness", environment.gustiness_ },
        { "airFriction", environment.airFriction_ },
        { "bounce", environment.bounce_ },
        { "clip", environment.clip_ }
    };
}

static void from_json(json const & j, Configuration::EmitterVolume & volume)
{
    j.at("name").get_to(volume.name_);
    j.at("type").get_to(volume.type_);
    j.at("length").get_to(volume.length_);
    j.at("width").get_to(volume.width_);
    j.at("height").get_to(volume.height_);
    j.at("depth").get_to(volume.depth_);
    j.at("radius").get_to(volume.radius_);
}

static void to_json(json & j, Configuration::EmitterVolume const & volume)
{
    j = json{
        { "name", volume.name_ },
        { "type", volume.type_ },
        { "length", volume.length_ },
        { "width", volume.width_ },
        { "height", volume.height_ },
        { "depth", volume.depth_ },
        { "radius", volume.radius_ }
    };
}

static void from_json(json const & j, Configuration::Emitter & emitter)
{
    j.at("name").get_to(emitter.name_);
    j.at("type").get_to(emitter.type_);
    j.at("volume").get_to(emitter.volume_);
    j.at("environment").get_to(emitter.environment_);
    j.at("appearance").get_to(emitter.appearance_);
    j.at("minSpeed").get_to(emitter.minSpeed_);
    j.at("maxSpeed").get_to(emitter.maxSpeed_);
    j.at("count").get_to(emitter.count_);
    j.at("lifetime").get_to(emitter.lifetime_);
    j.at("spread").get_to(emitter.spread_);
    j.at("color").get_to(emitter.color_);
    j.at("radius").get_to(emitter.radius_);
    j.at("sorted").get_to(emitter.sorted_);
    j.at("position").get_to(emitter.position_);
    j.at("orientation").get_to(emitter.orientation_);
    j.at("velocity").get_to(emitter.velocity_);
    j.at("particleVector").get_to(emitter.particleVector_);
}

static void to_json(json & j, Configuration::Emitter const & emitter)
{
    j = json{
        { "name", emitter.name_ },
        { "type", emitter.type_ },
        { "volume", emitter.volume_ },
        { "environment", emitter.environment_ },
        { "appearance", emitter.appearance_ },
        { "minSpeed", emitter.minSpeed_ },
        { "maxSpeed", emitter.maxSpeed_ },
        { "count", emitter.count_ },
        { "lifetime", emitter.lifetime_ },
        { "spread", emitter.spread_ },
        { "color", emitter.color_ },
        { "radius", emitter.radius_ },
        { "sorted", emitter.sorted_ },
        { "position", emitter.position_ },
        { "orientation", emitter.orientation_ },
        { "velocity", emitter.velocity_ },
        { "particleVector", emitter.particleVector_ }
    };
}

static void from_json(json const & j, Configuration & configuration)
{
    j.at("emitters").get_to(configuration.emitters_);
    j.at("emitterVolumes").get_to(configuration.emitterVolumes_);
    j.at("environments").get_to(configuration.environments_);
    j.at("appearances").get_to(configuration.appearances_);
    j.at("clipPlaneLists").get_to(configuration.clipPlaneLists_);
    j.at("bouncePlaneLists").get_to(configuration.bouncePlaneLists_);
}

static void to_json(json & j, Configuration const & configuration)
{
    j = json{
        { "emitters", configuration.emitters_ },
        { "emitterVolumes", configuration.emitterVolumes_ },
        { "environments", configuration.environments_ },
        { "appearances", configuration.appearances_ },
        { "clipPlaneLists", configuration.clipPlaneLists_ },
        { "bouncePlaneLists", configuration.bouncePlaneLists_ }
    };
}
} // namespace Confetti

Confetti::JsonConfiguration::JsonConfiguration(char const * filename)
{
    std::ifstream file(filename);
    json          j;
    file >> j;
    j.get_to(*static_cast<Configuration *>(this));
}

Confetti::JsonConfiguration::JsonConfiguration(nlohmann::json j)
{
    j.get_to(*static_cast<Configuration *>(this));
}

nlohmann::json Confetti::JsonConfiguration::toJson()
{
    return json(*this);
}
