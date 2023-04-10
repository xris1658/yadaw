#ifndef YADAW_SRC_AUDIO_PLUGIN_VST3EVENTPROCESSOR
#define YADAW_SRC_AUDIO_PLUGIN_VST3EVENTPROCESSOR

#include "audio/event/IEventBusInfo.hpp"
#include "audio/event/IEventProcessor.hpp"
#include "audio/plugin/VST3EventBusInfo.hpp"

namespace YADAW::Audio::Plugin
{
using YADAW::Audio::Event::IEventBusInfo;
class VST3EventProcessor : public YADAW::Audio::Event::IEventProcessor
{
public:
	VST3EventProcessor(Steinberg::Vst::IComponent& component);
    ~VST3EventProcessor() override;
public:
	std::uint32_t eventInputBusCount() const override;
	std::uint32_t eventOutputBusCount() const override;
	const IEventBusInfo* eventInputBusAt(std::uint32_t index) const override;
	const IEventBusInfo* eventOutputBusAt(std::uint32_t index) const override;
private:
	Steinberg::Vst::IComponent* component_;
	std::vector<Steinberg::Vst::BusInfo> eventInputBusInfoGroup_;
	std::vector<YADAW::Audio::Plugin::VST3EventBusInfo> input_;
	std::vector<Steinberg::Vst::BusInfo> eventOutputBusInfoGroup_;
	std::vector<YADAW::Audio::Plugin::VST3EventBusInfo> output_;
};
}

#endif //YADAW_SRC_AUDIO_PLUGIN_VST3EVENTPROCESSOR