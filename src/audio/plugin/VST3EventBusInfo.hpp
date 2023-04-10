#ifndef YADAW_SRC_AUDIO_PLUGIN_VST3EVENTBUSINFO
#define YADAW_SRC_AUDIO_PLUGIN_VST3EVENTBUSINFO

#include "audio/event/IEventBusInfo.hpp"

#include <pluginterfaces/vst/ivstcomponent.h>

namespace YADAW::Audio::Plugin
{
class VST3EventBusInfo : public YADAW::Audio::Event::IEventBusInfo
{
public:
	VST3EventBusInfo(const Steinberg::Vst::BusInfo& busInfo);
public:
	QString name() const override;
	std::uint32_t channelCount() const override;
	bool isMain() const override;
private:
	const Steinberg::Vst::BusInfo* busInfo_;
};
}

#endif //YADAW_SRC_AUDIO_PLUGIN_VST3EVENTBUSINFO