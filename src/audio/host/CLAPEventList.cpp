#include "CLAPEventList.hpp"

#include <cassert>

namespace YADAW::Audio::Host
{
int CLAPEventList::bufferIndex_ = 0;

CLAPEventList::CLAPEventList():
    inputEvents_{reinterpret_cast<void*>(this), &size, &get},
    outputEvents_{reinterpret_cast<void*>(this), &tryPush}
{
    assert(reinterpret_cast<CLAPEventList*>(inputEvents_.ctx) == this);
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

std::uint32_t CLAPEventList::doSize()
{
    return inputEventLists_[bufferIndex_].size();
}

const clap_event_header* CLAPEventList::doGet(std::uint32_t index)
{
    auto& inputEventList = inputEventLists_[bufferIndex_];
    if(index < inputEventList.size())
    {
        return inputEventList[index].get();
    }
    return nullptr;
}

bool CLAPEventList::tryPush(const clap_output_events* list, const clap_event_header* event)
{
    return reinterpret_cast<CLAPEventList*>(list->ctx)->doTryPush(event);
}

bool CLAPEventList::doTryPush(const clap_event_header* event)
{
    auto bufferIndex = bufferIndex_;
    auto& outputEventList = outputEventLists_[bufferIndex];
    if(outputEventList.full())
    {
        return false;
    }
    auto copy = reinterpret_cast<clap_event_header*>(std::malloc(event->size));
    outputEventList.pushBack(std::move(EventUniquePointer(copy, Impl::EventDeleter())));
    return true;
}

void CLAPEventList::flip()
{
    bufferIndex_ ^= 1;
}

void CLAPEventList::fillProcessData(clap_process* process)
{
    process->in_events = &inputEvents_;
    process->out_events = &outputEvents_;
}
}