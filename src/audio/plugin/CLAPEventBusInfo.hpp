#ifndef YADAW_SRC_AUDIO_PLUGIN_CLAPEVENTBUSINFO
#define YADAW_SRC_AUDIO_PLUGIN_CLAPEVENTBUSINFO

#include "audio/event/IEventBusInfo.hpp"

#include <clap/ext/note-ports.h>

namespace YADAW::Audio::Plugin
{
class CLAPEventBusInfo: public YADAW::Audio::Event::IEventBusInfo
{
public:
    CLAPEventBusInfo(const clap_note_port_info& notePortInfo);
    ~CLAPEventBusInfo() override;
public:
    QString name() const override;
    std::uint32_t channelCount() const override;
    bool isMain() const override;
private:
    const clap_note_port_info* notePortInfo_;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_CLAPEVENTBUSINFO
