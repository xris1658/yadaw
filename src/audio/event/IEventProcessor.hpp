#ifndef YADAW_SRC_AUDIO_EVENT_IEVENTPROCESSOR
#define YADAW_SRC_AUDIO_EVENT_IEVENTPROCESSOR

#include "audio/event/IEventBusInfo.hpp"

#include <cstdint>
#include <optional>
#include <vector>

namespace YADAW::Audio::Event
{
class IEventProcessor
{
public:
    using OptionalEventBusInfo = std::optional<std::reference_wrapper<const IEventBusInfo>>;
public:
    virtual ~IEventProcessor() = default;
public:
	virtual std::uint32_t eventInputBusCount() const = 0;
	virtual std::uint32_t eventOutputBusCount() const = 0;
	virtual OptionalEventBusInfo eventInputBusAt(std::uint32_t index) const = 0;
	virtual OptionalEventBusInfo eventOutputBusAt(std::uint32_t index) const = 0;
};
}

#endif // YADAW_SRC_AUDIO_EVENT_IEVENTPROCESSOR
