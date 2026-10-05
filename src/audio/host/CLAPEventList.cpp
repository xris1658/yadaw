#include "CLAPEventList.hpp"

#include "audio/host/HostContext.hpp"
#include "util/Algorithm.hpp"

#include <plf_hive.h>

#include <cstdlib>
#include <cstring>

namespace YADAW::Audio::Host
{
template<YADAW::Audio::Util::IsCLAPEvent T>
plf::hive<T>& eventHive()
{
    static plf::hive<T> hive;
    return hive;
}

template<YADAW::Audio::Util::IsCLAPEvent T>
CLAPEventList::Event copyKnownEvent(const clap_event_header_t* header)
{
    auto event = &*eventHive<T>().emplace();
    std::memcpy(event, header, header->size);
    return CLAPEventList::Event(&event->header, [](clap_event_header_t* event)
        {
            eventHive<T>().erase(
                eventHive<T>().get_iterator(reinterpret_cast<T*>(event))
            );
        }
    );
}

CLAPEventList::Event copyUnknownEvent(const clap_event_header_t* header)
{
    auto event = reinterpret_cast<clap_event_header_t*>(std::malloc(header->size));
    std::memcpy(&event, header, header->size);
    return CLAPEventList::Event(event, [](clap_event_header_t* event)
        {
            std::free(event);
        }
    );
}

CLAPEventList::Event CLAPEventList::copyEvent(const clap_event_header_t* event)
{
    if(event->type == CLAP_EVENT_NOTE_ON ||
       event->type == CLAP_EVENT_NOTE_OFF ||
       event->type == CLAP_EVENT_NOTE_CHOKE ||
       event->type == CLAP_EVENT_NOTE_END)
    {
        if(event->size == sizeof(clap_event_note_t))
        {
            return copyKnownEvent<clap_event_note_t>(event);
        }
    }
    else if(event->type == CLAP_EVENT_NOTE_EXPRESSION)
    {
        if(event->size == sizeof(clap_event_note_expression_t))
        {
            return copyKnownEvent<clap_event_note_expression_t>(event);
        }
    }
    else if(event->type == CLAP_EVENT_PARAM_VALUE)
    {
        if(event->size == sizeof(clap_event_param_value_t))
        {
            return copyKnownEvent<clap_event_param_value_t>(event);
        }
    }
    else if(event->type == CLAP_EVENT_PARAM_MOD)
    {
        if(event->size == sizeof(clap_event_param_mod_t))
        {
            return copyKnownEvent<clap_event_param_mod_t>(event);
        }
    }
    else if(event->type == CLAP_EVENT_PARAM_GESTURE_BEGIN ||
            event->type == CLAP_EVENT_PARAM_GESTURE_END)
    {
        if(event->size == sizeof(clap_event_param_gesture_t))
        {
            return copyKnownEvent<clap_event_param_gesture_t>(event);
        }
    }
    if(event->type == CLAP_EVENT_TRANSPORT)
    {
        if(event->size == sizeof(clap_event_transport_t))
        {
            return copyKnownEvent<clap_event_transport_t>(event);
        }
    }
    if(event->type == CLAP_EVENT_MIDI)
    {
        if(event->size == sizeof(clap_event_midi_t))
        {
            return copyKnownEvent<clap_event_midi_t>(event);
        }
    }
    if(event->type == CLAP_EVENT_MIDI_SYSEX)
    {
        if(event->size == sizeof(clap_event_midi_sysex_t))
        {
            return copyKnownEvent<clap_event_midi_sysex_t>(event);
        }
    }
    if(event->type == CLAP_EVENT_MIDI2)
    {
        if(event->size == sizeof(clap_event_midi2_t))
        {
            return copyKnownEvent<clap_event_midi2_t>(event);
        }
    }
    return copyUnknownEvent(event);
}

CLAPEventList::CLAPEventList():
    inputEvents_{reinterpret_cast<void*>(this), &size, &get},
    outputEvents_{reinterpret_cast<void*>(this), &tryPush}
{
}

CLAPEventList::~CLAPEventList()
{
    //
}

std::uint32_t CLAPEventList::size(const clap_input_events* list)
{
    return reinterpret_cast<CLAPEventList*>(list->ctx)->doSize();
}

const clap_event_header* CLAPEventList::get(const clap_input_events* list, std::uint32_t index)
{
    return reinterpret_cast<CLAPEventList*>(list->ctx)->doGet(index);
}

std::uint32_t CLAPEventList::doSize() const
{
    return inputEventLists_[YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get()].size();
}

const clap_event_header* CLAPEventList::doGet(std::uint32_t index) const
{
    auto& inputEventList = inputEventLists_[YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get()];
    if(index < inputEventList.size())
    {
        return inputEventList[index].header;
    }
    return nullptr;
}

bool CLAPEventList::tryPush(const clap_output_events* list, const clap_event_header* event)
{
    return reinterpret_cast<CLAPEventList*>(list->ctx)->doTryPush(event);
}

bool CLAPEventList::doTryPush(const clap_event_header* event)
{
    auto& outputEventList = outputEventLists_[YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get()];
    if(outputEventList.full())
    {
        return false;
    }
    if(event->type == CLAP_EVENT_PARAM_VALUE)
    {
        auto paramValue = reinterpret_cast<const clap_event_param_value_t*>(event);
        // TODO: Add mechanism recording automations here (a `performEdit`)
    }
    else if(event->type == CLAP_EVENT_PARAM_GESTURE_BEGIN)
    {
        auto paramGesture = reinterpret_cast<const clap_event_param_gesture_t*>(event);
        // TODO: Add mechanism recording automations here (a `beginEdit`)
    }
    else if(event->type == CLAP_EVENT_PARAM_GESTURE_END)
    {
        auto paramGesture = reinterpret_cast<const clap_event_param_gesture_t*>(event);
        // TODO: Add mechanism recording automations here (an `endEdit`)
    }
    outputEventList.pushBack(copyEvent(event));
    return true;
}

bool CLAPEventList::pushBackEvent(const clap_event_header* event)
{
    auto& inputEventList = inputEventLists_[YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get() ^ 1];
    if(inputEventList.full())
    {
        return false;
    }
    auto copy = reinterpret_cast<clap_event_header*>(std::malloc(event->size));
    std::memcpy(copy, event, event->size);
    inputEventList.pushBack(copyEvent(event));
    return true;
}

std::size_t CLAPEventList::outputEventCount() const
{
    return outputEventLists_[YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get() ^ 1].size();
}

const clap_event_header* CLAPEventList::outputEventAt(std::size_t index) const
{
    if(index < outputEventCount())
    {
        return outputEventLists_[YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get() ^ 1][index].header;
    }
    return nullptr;
}

void CLAPEventList::flipped()
{
    auto newIndex = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get();
    inputEventLists_[newIndex ^ 1].clear();
    outputEventLists_[newIndex].clear();
}

void CLAPEventList::attachToProcessData(clap_process& process)
{
    auto& inputEventList = inputEventLists_[YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get()];
    YADAW::Util::insertionSort(inputEventList.begin(), inputEventList.end(),
        [](const Event& lhs, const Event& rhs)
        {
            return lhs.header->time < rhs.header->time;
        }
    );
    process.in_events = &inputEvents_;
    process.out_events = &outputEvents_;
}
}
