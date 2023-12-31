#include "AudioBackendController.hpp"

#include "util/Base.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Controller
{
using YADAW::Entity::AudioBackendSupport;
AudioBackendSupport::Backend backendFromConfig(const YAML::Node& node)
{
    const auto& audioBackendStringNode = node["audio-api"];
    auto audioBackendString = node.IsDefined()?
        std::optional(audioBackendStringNode.as<std::string>()):
        std::nullopt;
    if(audioBackendString.has_value())
    {
        FOR_RANGE0(i, YADAW::Util::stackArraySize(AudioBackendSupport::backendNames))
        {
            if(audioBackendString == AudioBackendSupport::backendNames[i])
            {
                return static_cast<AudioBackendSupport::Backend>(i);
            }
        }
    }
    return YADAW::Entity::AudioBackendSupport::Backend::Off;
}

void saveBackendToConfig(YADAW::Entity::AudioBackendSupport::Backend backend,
    YAML::Node& node)
{
    node["audio-api"] = AudioBackendSupport::backendNames[backend];
}
}