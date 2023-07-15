#ifndef YADAW_SRC_AUDIO_PLUGIN_CLAPEVENTPROCESSOR
#define YADAW_SRC_AUDIO_PLUGIN_CLAPEVENTPROCESSOR

#include "audio/event/IEventBusInfo.hpp"
#include "audio/event/IEventProcessor.hpp"
#include "audio/plugin/CLAPEventBusInfo.hpp"

#include <clap/plugin.h>
#include <clap/ext/note-ports.h>

#include <vector>

namespace YADAW::Audio::Plugin
{
using YADAW::Audio::Event::IEventBusInfo;
class CLAPEventProcessor: public YADAW::Audio::Event::IEventProcessor
{
public:
    CLAPEventProcessor(const clap_plugin& plugin, const clap_plugin_note_ports& notePorts);
    ~CLAPEventProcessor() override = default;
public:
    std::uint32_t eventInputBusCount() const override;
    std::uint32_t eventOutputBusCount() const override;
    OptionalEventBusInfo eventInputBusAt(std::uint32_t index) const override;
    OptionalEventBusInfo eventOutputBusAt(std::uint32_t index) const override;
private:
    const clap_plugin* plugin_;
    const clap_plugin_note_ports* notePorts_;
    std::vector<clap_note_port_info> inputNotePortInfo_;
    std::vector<clap_note_port_info> outputNotePortInfo_;
    std::vector<CLAPEventBusInfo> inputEventBusInfo_;
    std::vector<CLAPEventBusInfo> outputEventBusInfo_;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_CLAPEVENTPROCESSOR
