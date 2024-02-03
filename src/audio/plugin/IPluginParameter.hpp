#ifndef YADAW_SRC_AUDIO_PLUGIN_IPLUGINPARAMETER
#define YADAW_SRC_AUDIO_PLUGIN_IPLUGINPARAMETER

#include "audio/plugin/IParameter.hpp"

namespace YADAW::Audio::Plugin
{
class IPluginParameter
{
public:
    virtual ~IPluginParameter() {}
public:
    virtual std::uint32_t parameterCount() const = 0;
    virtual IParameter* parameter(std::uint32_t index) = 0;
    virtual const IParameter* parameter(std::uint32_t index) const = 0;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_IPLUGINPARAMETER
