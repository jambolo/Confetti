#if !defined(CONFETTI_JSONCONFIGURATION_H)
#define CONFETTI_JSONCONFIGURATION_H

#pragma once

#include <Confetti/Configuration.h>
#include <nlohmann/json.hpp>

namespace Confetti
{
//! A Configuration loaded from JSON.
class JsonConfiguration : public Configuration
{
public:
    //! Constructor.
    explicit JsonConfiguration(char const * path);

    //! Constructor.
    explicit JsonConfiguration(nlohmann::json j);

    //! Constructor.
    JsonConfiguration(Configuration const & c) : Configuration(c) {}

    virtual ~JsonConfiguration() override = default;

    //! Saves the configuration to a JSON object
    nlohmann::json toJson();
};
} // namespace Confetti

#endif // !defined(CONFETTI_JSONCONFIGURATION_H)
