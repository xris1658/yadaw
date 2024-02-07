#include "CLAPEventList.hpp"

#include "util/Algorithm.hpp"

#include <cstdlib>
#include <cstring>

namespace YADAW::Audio::Host
{
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
    return inputEventLists_[pluginBufferIndex_.load(std::memory_order::memory_order_acquire)].size();
}

const clap_event_header* CLAPEventList::doGet(std::uint32_t index) const
{
    auto& inputEventList = inputEventLists_[pluginBufferIndex_.load(std::memory_order::memory_order_acquire)];
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
    auto& outputEventList = outputEventLists_[pluginBufferIndex_.load(std::memory_order::memory_order_acquire)];
    if(outputEventList.full())
    {
        return false;
    }
    // TODO: Use memory pool
    auto copy = reinterpret_cast<clap_event_header*>(std::malloc(event->size));
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
    outputEventList.pushBack(std::move(EventUniquePointer(copy, Impl::EventDeleter())));
    return true;
}

bool CLAPEventList::pushBackEvent(const clap_event_header* event)
{
    auto& inputEventList = inputEventLists_[pluginBufferIndex_.load(std::memory_order::memory_order_acquire) ^ 1];
    if(inputEventList.full())
    {
        return false;
    }
    // TODO: Use memory pool
    auto copy = reinterpret_cast<clap_event_header*>(std::malloc(event->size));
    std::memcpy(copy, event, event->size);
    inputEventList.pushBack(std::move(EventUniquePointer(copy, Impl::EventDeleter())));
    return true;
}

std::size_t CLAPEventList::outputEventCount() const
{
    return outputEventLists_[pluginBufferIndex_.load(std::memory_order::memory_order_acquire) ^ 1].size();
}

const clap_event_header* CLAPEventList::outputEventAt(std::size_t index) const
{
    if(index < outputEventCount())
    {
        return outputEventLists_[pluginBufferIndex_.load(std::memory_order::memory_order_acquire) ^ 1][index].get();
    }
    return nullptr;
}

void CLAPEventList::flip()
{
    inputEventLists_[pluginBufferIndex_.fetch_xor(1)].clear();
    outputEventLists_[pluginBufferIndex_.load(std::memory_order::memory_order_acquire)].clear();
}

void CLAPEventList::attachToProcessData(clap_process& process)
{
    auto& inputEventList = inputEventLists_[pluginBufferIndex_.load(std::memory_order::memory_order_acquire)];
    YADAW::Util::insertionSort(inputEventList.begin(), inputEventList.end(),
        [](const EventUniquePointer& lhs, const EventUniquePointer& rhs)
        {
            return lhs->time < rhs->time;
        }
    );
    process.in_events = &inputEvents_;
    process.out_events = &outputEvents_;
}
}
