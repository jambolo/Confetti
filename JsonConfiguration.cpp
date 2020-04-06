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

namespace
{
uint64_t    nextDefaultId = 1;
}

namespace Confetti
{
static void from_json(json const & j, Configuration::Particle & particle)
{
    if (j.contains("lifetime")) j.at("lifetime").get_to(particle.lifetime_);
    if (j.contains("age")) j.at("age").get_to(particle.age_);
    if (j.contains("position")) j.at("position").get_to(particle.position_);
    if (j.contains("velocity")) j.at("velocity").get_to(particle.velocity_);
    if (j.contains("color")) j.at("color").get_to(particle.color_);
    if (j.contains("radius")) j.at("radius").get_to(particle.radius_);
    if (j.contains("rotation")) j.at("rotation").get_to(particle.rotation_);
    if (j.contains("orientation")) j.at("orientation").get_to(particle.orientation_);
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

static void from_json(json const & j, Configuration::ClipperList & list)
{
    if (j.contains("name"))
        j.at("name").get_to(list.name_);
    else
        list.name_ = "_" + std::to_string(nextDefaultId++);

    if (j.contains("surfaces")) j.at("surfaces").get_to(list.planes_);
}

static void to_json(json & j, Configuration::ClipperList const & list)
{
    j = json{
        { "name", list.name_ },
        { "planes", list.planes_ }
    };
}

static void from_json(json const & j, Configuration::Surface & surface)
{
    if (j.contains("plane")) j.at("plane").get_to(surface.plane_);
    if (j.contains("dampening")) j.at("dampening").get_to(surface.dampening_);
}

static void to_json(json & j, Configuration::Surface const & surface)
{
    j = json{
        { "plane", surface.plane_ },
        { "dampening", surface.dampening_ }
    };
}

static void from_json(json const & j, Configuration::SurfaceList & list)
{
    if (j.contains("name"))
        j.at("name").get_to(list.name_);
    else
        list.name_ = "_" + std::to_string(nextDefaultId++);

    if (j.contains("surfaces")) j.at("surfaces").get_to(list.surfaces_);
}

static void to_json(json & j, Configuration::SurfaceList const & list)
{
    j = json{
        { "name", list.name_ },
        { "surfaces", list.surfaces_ }
    };
}

static void from_json(json const & j, Configuration::Appearance & appearance)
{
    if (j.contains("name"))
        j.at("name").get_to(appearance.name_);
    else
        appearance.name_ = "_" + std::to_string(nextDefaultId++);

    if (j.contains("colorChange")) j.at("colorChange").get_to(appearance.colorChange_);
    if (j.contains("radiusChange")) j.at("radiusChange").get_to(appearance.radiusChange_);
    if (j.contains("radialVelocity")) j.at("radialVelocity").get_to(appearance.radialVelocity_);
    if (j.contains("texture")) j.at("texture").get_to(appearance.texture_);
    if (j.contains("size")) j.at("size").get_to(appearance.size_);
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
    if (j.contains("name"))
        j.at("name").get_to(environment.name_);
    else
        environment.name_ = "_" + std::to_string(nextDefaultId++);

    if (j.contains("gravity")) j.at("gravity").get_to(environment.gravity_);
    if (j.contains("windVelocity")) j.at("windVelocity").get_to(environment.windVelocity_);
    if (j.contains("gustiness")) j.at("gustiness").get_to(environment.gustiness_);
    if (j.contains("airFriction")) j.at("airFriction").get_to(environment.airFriction_);
    if (j.contains("surface")) j.at("surface").get_to(environment.surface_);
    if (j.contains("clip")) j.at("clip").get_to(environment.clip_);
}

static void to_json(json & j, Configuration::Environment const & environment)
{
    j = json{
        { "name", environment.name_ },
        { "gravity", environment.gravity_ },
        { "windVelocity", environment.windVelocity_ },
        { "gustiness", environment.gustiness_ },
        { "airFriction", environment.airFriction_ },
        { "surface", environment.surface_ },
        { "clip", environment.clip_ }
    };
}

static void from_json(json const & j, Configuration::EmitterVolume & volume)
{
    if (j.contains("name"))
        j.at("name").get_to(volume.name_);
    else
        volume.name_ = "_" + std::to_string(nextDefaultId++);

    if (j.contains("type")) j.at("type").get_to(volume.type_);
    if (j.contains("length")) j.at("length").get_to(volume.length_);
    if (j.contains("width")) j.at("width").get_to(volume.width_);
    if (j.contains("height")) j.at("height").get_to(volume.height_);
    if (j.contains("depth")) j.at("depth").get_to(volume.depth_);
    if (j.contains("radius")) j.at("radius").get_to(volume.radius_);
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
    if (j.contains("name"))
        j.at("name").get_to(emitter.name_);
    else
        emitter.name_ = "_" + std::to_string(nextDefaultId++);

    if (j.contains("type")) j.at("type").get_to(emitter.type_);
    if (j.contains("volume")) j.at("volume").get_to(emitter.volume_);
    if (j.contains("environment")) j.at("environment").get_to(emitter.environment_);
    if (j.contains("appearance")) j.at("appearance").get_to(emitter.appearance_);
    if (j.contains("minSpeed")) j.at("minSpeed").get_to(emitter.minSpeed_);
    if (j.contains("maxSpeed")) j.at("maxSpeed").get_to(emitter.maxSpeed_);
    if (j.contains("count")) j.at("count").get_to(emitter.count_);
    if (j.contains("lifetime")) j.at("lifetime").get_to(emitter.lifetime_);
    if (j.contains("spread")) j.at("spread").get_to(emitter.spread_);
    if (j.contains("color")) j.at("color").get_to(emitter.color_);
    if (j.contains("radius")) j.at("radius").get_to(emitter.radius_);
    if (j.contains("sorted")) j.at("sorted").get_to(emitter.sorted_);
    if (j.contains("position")) j.at("position").get_to(emitter.position_);
    if (j.contains("orientation")) j.at("orientation").get_to(emitter.orientation_);
    if (j.contains("velocity")) j.at("velocity").get_to(emitter.velocity_);
    if (j.contains("particles")) j.at("particles").get_to(emitter.particles_);
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
        { "particles", emitter.particles_ }
    };
}

template<class T>
static void from_json_array(json const & j, char const * name, std::map<std::string, T> & map )
{
    std::vector<T> a;
    j.at(name).get_to(a);
    for (auto const & e : a)
    {
        if (map.find(e.name_) == map.end())
        {
            map.emplace(e.name_, e);
        }
        else
        {
            throw std::runtime_error(std::string("Duplicated element in '") + name + "': '" + e.name_ + "'");
        }
    }
}
static void from_json(json const & j, Configuration & configuration)
{
    if (j.contains("emitters")) from_json_array(j, "emitters", configuration.emitters_);
    if (j.contains("emitterVolumes")) from_json_array(j, "emitterVolumes", configuration.emitterVolumes_);
    if (j.contains("environments")) from_json_array(j, "environments", configuration.environments_);
    if (j.contains("appearances")) from_json_array(j, "appearances", configuration.appearances_);
    if (j.contains("clipperLists")) from_json_array(j, "clipperLists", configuration.clipperLists_);
    if (j.contains("surfaceLists")) from_json_array(j, "surfaceLists", configuration.surfaceLists_);
}

static void to_json(json & j, Configuration const & configuration)
{
    j = json{
        { "emitters", configuration.emitters_ },
        { "emitterVolumes", configuration.emitterVolumes_ },
        { "environments", configuration.environments_ },
        { "appearances", configuration.appearances_ },
        { "clipperLists", configuration.clipperLists_ },
        { "surfaceLists", configuration.surfaceLists_ }
    };
}
} // namespace Confetti

Confetti::JsonConfiguration::JsonConfiguration(char const * filename)
{
    std::ifstream file(filename);
    if (!file) throw std::runtime_error(std::string("Cannot open file '") + filename + "'");

    json j;
    file >> j;
    j.get_to(*static_cast<Configuration *>(this));
}

Confetti::JsonConfiguration::JsonConfiguration(nlohmann::json const & j)
{
    j.get_to(*static_cast<Configuration *>(this));
}

nlohmann::json Confetti::JsonConfiguration::toJson()
{
    return json(*this);
}
