#ifndef YADAW_SRC_AUDIO_HOST_CLAPEVENTLIST
#define YADAW_SRC_AUDIO_HOST_CLAPEVENTLIST

#include "util/FixedSizeCircularDeque.hpp"

#include <clap/events.h>
#include <clap/process.h>

#include <memory>

namespace YADAW::Audio::Host
{
namespace Impl
{
class EventDeleter
{
public:
    EventDeleter() noexcept = default;
    EventDeleter(const EventDeleter&) noexcept = default;
    EventDeleter& operator=(const EventDeleter&) noexcept = default;
    EventDeleter(EventDeleter&&) noexcept = default;
    EventDeleter& operator=(EventDeleter&&) noexcept = default;
    ~EventDeleter() noexcept = default;
public:
    void operator()(void* ptr)
    {
        std::free(ptr);
    }
};
}

class CLAPEventList
{
    using EventUniquePointer = std::unique_ptr<clap_event_header, Impl::EventDeleter>;
public:
    CLAPEventList();
    ~CLAPEventList();
private:
    static std::uint32_t size(const clap_input_events* list);
    static const clap_event_header* get(const clap_input_events* list, std::uint32_t index);
    std::uint32_t doSize();
    const clap_event_header* doGet(std::uint32_t index);
private:
    static bool tryPush(const clap_output_events* list, const clap_event_header* event);
    bool doTryPush(const clap_event_header* event);
public:
    void flip();
    void fillProcessData(clap_process* process);
private:
    static int bufferIndex_;
    clap_input_events inputEvents_;
    clap_output_events outputEvents_;
    YADAW::Util::FixedSizeCircularDeque<EventUniquePointer, 4096> inputEventLists_[2];
    YADAW::Util::FixedSizeCircularDeque<EventUniquePointer, 4096> outputEventLists_[2];
};
}

#endif //YADAW_SRC_AUDIO_HOST_CLAPEVENTLIST
