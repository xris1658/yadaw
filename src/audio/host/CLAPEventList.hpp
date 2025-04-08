#ifndef YADAW_SRC_AUDIO_HOST_CLAPEVENTLIST
#define YADAW_SRC_AUDIO_HOST_CLAPEVENTLIST

#include "util/CDeleter.hpp"
#include "util/FixedSizeCircularDeque.hpp"

#include <clap/events.h>
#include <clap/process.h>

#include <atomic>
#include <cstdlib>
#include <memory>

namespace YADAW::Audio::Host
{
// QUES: Triple buffer is probably a better choice:
//                                          |  realtime thread only  |
//  Input buffer  (host):   Enqueue events -> attach to process data -> GC
//  Output buffer (plugin): Enqueue events -> read queue             -> GC
class CLAPEventList
{
public:
    using EventUniquePointer = YADAW::Util::UniquePtrWithCDeleter<clap_event_header>;
    using CircularDequeType = YADAW::Util::FixedSizeCircularDeque<EventUniquePointer, 4096>;
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
