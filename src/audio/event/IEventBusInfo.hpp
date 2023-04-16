#ifndef YADAW_SRC_AUDIO_EVENT_IEVENTBUSINFO
#define YADAW_SRC_AUDIO_EVENT_IEVENTBUSINFO

#include <QString>

#include <cstdint>

namespace YADAW::Audio::Event
{
class IEventBusInfo
{
public:
	virtual QString name() const = 0;
	virtual std::uint32_t channelCount() const = 0;
	virtual bool isMain() const = 0;
};
}

#endif //YADAW_SRC_AUDIO_EVENT_IEVENTBUSINFO