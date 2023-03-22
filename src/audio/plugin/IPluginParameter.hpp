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
    virtual int parameterCount() = 0;
    virtual IParameter* parameter(int index) = 0;
};
}

#endif //YADAW_SRC_AUDIO_PLUGIN_IPLUGINPARAMETER
