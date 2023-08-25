#ifndef YADAW_SRC_NATIVE_LINUX_MIDIINPUTDEVICEIMPL
#define YADAW_SRC_NATIVE_LINUX_MIDIINPUTDEVICEIMPL

#if __linux__

#include "midi/DeviceInfo.hpp"
#include "midi/MIDIInputDevice.hpp"
#include "native/linux/Sequencer.hpp"

#include <alsa/asoundlib.h>

#include <thread>

namespace YADAW::MIDI
{
class MIDIInputDevice::Impl
{
public:
    static std::size_t inputDeviceCount();
    static std::optional<DeviceInfo> inputDeviceAt(std::size_t index);
public:
    Impl(const MIDIInputDevice& device, const YADAW::Native::MIDIDeviceID& id);
    ~Impl();
public:
    void start(ReceiveInputFunc* const func);
    void stop();
private:
    const MIDIInputDevice& device_;
    YADAW::Native::MIDIDeviceID deviceId_;
    snd_seq_port_subscribe_t* subscription_ = nullptr;
    std::thread midiThread_;
    std::atomic_bool run_ {true};
    int queueId_ = -1;
    int seqPortId_ = -1;
};
}

#endif

#endif //YADAW_SRC_NATIVE_LINUX_MIDIINPUTDEVICEIMPL
