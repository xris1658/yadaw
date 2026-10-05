#ifndef YADAW_SRC_AUDIO_HOST_CLAPEVENTLIST
#define YADAW_SRC_AUDIO_HOST_CLAPEVENTLIST

#include "audio/util/CLAPHelper.hpp"
#include "util/FixedSizeCircularDeque.hpp"

#include <clap/events.h>
#include <clap/process.h>

#include <atomic>
#include <memory>

namespace YADAW::Audio::Host
{
// QUES: Triple buffer is probably a better choice:
//                                          |  realtime thread only  |
//  Input buffer  (host):   Enqueue events -> attach to process data -> GC
//  Output buffer (plugin): Enqueue events -> read queue             -> GC
class CLAPEventList
{
private:
    struct Event
    {
        using Deleter = void(clap_event_header_t*);
        clap_event_header_t* header = nullptr;
        Deleter* deleter = nullptr;
        Event(clap_event_header_t* header, Deleter* deleter):
            header(header), deleter(deleter) {}
        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;
        Event(Event&& rhs) noexcept:
            header(rhs.header), deleter(rhs.deleter)
        {
            rhs.deleter = nullptr;
        }
        Event& operator=(Event&& rhs) noexcept
        {
            header = rhs.header;
            deleter = rhs.deleter;
            rhs.deleter = nullptr;
            return *this;
        }
        ~Event() noexcept
        {
            if(deleter)
            {
                deleter(header);
            }
        }
    };
    using CircularDequeType = YADAW::Util::FixedSizeCircularDeque<Event, 4096>;
    template<YADAW::Audio::Util::IsCLAPEvent T>
    friend Event copyKnownEvent(const clap_event_header_t* header);
    friend Event copyUnknownEvent(const clap_event_header_t* header);
    static Event copyEvent(const clap_event_header_t* event);
public:
    CLAPEventList();
    ~CLAPEventList();
private:
    static std::uint32_t size(const clap_input_events* list);
    static const clap_event_header* get(const clap_input_events* list, std::uint32_t index);
    std::uint32_t doSize() const;
    const clap_event_header* doGet(std::uint32_t index) const;
private:
    static bool tryPush(const clap_output_events* list, const clap_event_header* event);
    bool doTryPush(const clap_event_header* event);
public: // host-side buffer reads and writes
    bool pushBackEvent(const clap_event_header* event);
    std::size_t outputEventCount() const;
    const clap_event_header* outputEventAt(std::size_t index) const;
    // Called upon start of audio callback
    void flipped();
    void attachToProcessData(clap_process& process);
private:
    std::atomic<int> pluginBufferIndex_ {0};
    clap_input_events inputEvents_;
    clap_output_events outputEvents_;
    CircularDequeType inputEventLists_[2];
    CircularDequeType outputEventLists_[2];
};
}

#endif // YADAW_SRC_AUDIO_HOST_CLAPEVENTLIST
