#ifndef YADAW_SRC_AUDIO_HOST_VST3COMPONENTHANDLER
#define YADAW_SRC_AUDIO_HOST_VST3COMPONENTHANDLER

#include "audio/plugin/VST3Plugin.hpp"

#include <pluginterfaces/vst/ivsteditcontroller.h>

namespace YADAW::Audio::Host
{
using namespace Steinberg;
using namespace Steinberg::Vst;
class VST3ComponentHandler:
    public Steinberg::Vst::IComponentHandler
{
    using Self = VST3ComponentHandler;
public:
    VST3ComponentHandler(YADAW::Audio::Plugin::VST3Plugin* plugin);
    VST3ComponentHandler(const Self&) = delete;
    VST3ComponentHandler(Self&& rhs) = delete;
    Self& operator=(const Self&) = delete;
    Self& operator=(Self&&) = delete;
    ~VST3ComponentHandler() noexcept;
public:
    DECLARE_FUNKNOWN_METHODS
public:
    tresult PLUGIN_API beginEdit(ParamID id) override;
    tresult PLUGIN_API performEdit(ParamID id, ParamValue normalizedValue) override;
    tresult PLUGIN_API endEdit(ParamID id) override;
    tresult PLUGIN_API restartComponent(int32 flags) override;
private:
    YADAW::Audio::Plugin::VST3Plugin* plugin_;
};

}

#endif //YADAW_SRC_AUDIO_HOST_VST3COMPONENTHANDLER
