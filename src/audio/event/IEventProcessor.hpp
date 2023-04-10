#ifndef YADAW_SRC_AUDIO_EVENT_IEVENTPROCESSOR
#define YADAW_SRC_AUDIO_EVENT_IEVENTPROCESSOR

#include "audio/event/IEventBusInfo.hpp"

#include <pluginterfaces/vst/ivstcomponent.h>

#include <cstdint>
#include <vector>

namespace YADAW::Audio::Event
{
class IEventProcessor
{
public:
    virtual ~IEventProcessor() = default;
public:
	virtual std::uint32_t eventInputBusCount() const = 0;
	virtual std::uint32_t eventOutputBusCount() const = 0;
	virtual const IEventBusInfo* eventInputBusAt(std::uint32_t index) const = 0;
	virtual const IEventBusInfo* eventOutputBusAt(std::uint32_t index) const = 0;
};
}

#endif //YADAW_SRC_AUDIO_EVENT_IEVENTPROCESSOR
