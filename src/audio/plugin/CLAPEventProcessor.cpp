#include "CLAPEventProcessor.hpp"

namespace YADAW::Audio::Plugin
{
using YADAW::Audio::Event::IEventProcessor;

CLAPEventProcessor::CLAPEventProcessor(const clap_plugin& plugin, const clap_plugin_note_ports& notePorts):
    plugin_(&plugin),
    notePorts_(&notePorts)
{
    inputNotePortInfo_.resize(notePorts.count(plugin_, true));
    inputEventBusInfo_.reserve(inputNotePortInfo_.size());
    outputNotePortInfo_.resize(notePorts.count(plugin_, false));
    outputEventBusInfo_.reserve(outputNotePortInfo_.size());
    for(std::uint32_t i = 0; i < inputNotePortInfo_.size(); ++i)
    {
        notePorts.get(plugin_, i, true, inputNotePortInfo_.data() + i);
        inputEventBusInfo_.emplace_back(inputNotePortInfo_[i]);
    }
    for(std::uint32_t i = 0; i < outputNotePortInfo_.size(); ++i)
    {
        notePorts.get(plugin_, i, false, outputNotePortInfo_.data() + i);
        outputEventBusInfo_.emplace_back(outputNotePortInfo_[i]);
    }
}

std::uint32_t CLAPEventProcessor::eventInputBusCount() const
{
    return inputNotePortInfo_.size();
}

std::uint32_t CLAPEventProcessor::eventOutputBusCount() const
{
    return outputNotePortInfo_.size();
}

IEventProcessor::OptionalEventBusInfo CLAPEventProcessor::eventInputBusAt(std::uint32_t index) const
{
    if(index < eventInputBusCount())
    {
        return std::ref(inputEventBusInfo_[index]);
    }
    return std::nullopt;
}

IEventProcessor::OptionalEventBusInfo CLAPEventProcessor::eventOutputBusAt(std::uint32_t index) const
{
    if(index < eventOutputBusCount())
    {
        return std::ref(outputEventBusInfo_[index]);
    }
    return std::nullopt;
}
}