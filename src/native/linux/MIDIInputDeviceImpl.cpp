#if(__linux__)

#include "MIDIInputDeviceImpl.hpp"

#include "native/linux/ALSADeviceEnumerator.hpp"

namespace YADAW::MIDI
{
using namespace YADAW::Native;
std::size_t MIDIInputDevice::Impl::inputDeviceCount()
{
    return YADAW::Native::ALSADeviceEnumerator::midiDeviceCount();
}

std::optional<YADAW::MIDI::DeviceInfo>
    MIDIInputDevice::Impl::inputDeviceAt(std::size_t index)
{
    return ALSADeviceEnumerator::midiDeviceAt(index);
}

MIDIInputDevice::Impl::Impl(const MIDIInputDevice& device, const YADAW::Native::MIDIDeviceID& id):
    device_(device),
    deviceId_(id)
{
    auto seq = Sequencer::instance().seq();
    auto createPortResult = snd_seq_create_simple_port(
        seq, "YADAW application port",
        SND_SEQ_PORT_CAP_DUPLEX | SND_SEQ_PORT_CAP_SUBS_READ | SND_SEQ_PORT_CAP_SUBS_WRITE,
        SND_SEQ_PORT_TYPE_MIDI_GENERIC);
    if(createPortResult < 0)
    {
        auto errorMessage = snd_strerror(createPortResult);
        return;
    }
    seqPortId_ = createPortResult;
}

MIDIInputDevice::Impl::~Impl()
{
    stop();
    auto seq = Sequencer::instance().seq();
    if(seqPortId_ != -1)
    {
        snd_seq_delete_port(seq, seqPortId_);
    }
}

void MIDIInputDevice::Impl::start(MIDIInputDevice::ReceiveInputFunc* const func)
{
    auto seq = Sequencer::instance().seq();
    snd_seq_addr_t sender;
    snd_seq_addr_t dest;
    sender.client = snd_seq_client_id(seq);
    sender.port = seqPortId_;
    dest.client = deviceId_.clientId;
    dest.port = deviceId_.portId;
    snd_seq_queue_info_t* queueInfo;
    snd_seq_queue_info_alloca(&queueInfo);
    queueId_ = snd_seq_create_queue(seq, queueInfo);
    if(queueId_ < 0)
    {
        auto errorMessage = snd_strerror(queueId_);
        return;
    }
    snd_seq_port_subscribe_malloc(&subscription_);
    snd_seq_port_subscribe_set_sender(subscription_, &sender);
    snd_seq_port_subscribe_set_dest(subscription_, &dest);
    snd_seq_port_subscribe_set_queue(subscription_, queueId_);
    snd_seq_port_subscribe_set_time_update(subscription_, 1);
    snd_seq_port_subscribe_set_time_real(subscription_, 1);
    if(auto connectResult = snd_seq_subscribe_port(seq, subscription_);
        connectResult < 0)
    {
        auto errorMessage = snd_strerror(connectResult);
        snd_seq_port_subscribe_free(subscription_);
        subscription_ = nullptr;
        return;
    }
    snd_seq_control_queue(seq, queueId_, SND_SEQ_EVENT_START, 0, nullptr);
    snd_seq_drain_output(seq);
    midiThread_ = std::thread(
        [this, func, seq]()
        {
            while(run_.load(std::memory_order::memory_order_acquire))
            {
                if(snd_seq_event_input_pending(seq, 1) == 0)
                {
                    std::this_thread::yield();
                }
                else
                {
                    snd_seq_event_t* event;
                    auto getEventResult = snd_seq_event_input(seq, &event);
                    if(getEventResult == -ENOSPC)
                    {
                        // Input buffer overrun
                    }
                    else if(getEventResult < 0)
                    {
                        auto errorMessage = snd_strerror(getEventResult);
                    }
                    else
                    {
                        auto length = snd_seq_event_length(event);
                        YADAW::MIDI::Message message{};
                        message.size = length;
                        message.data = reinterpret_cast<std::uint8_t*>(&(event->data));
                        func(device_, message);
                    }
                }
            }
        }
    );
}

void MIDIInputDevice::Impl::stop()
{
    auto seq = Sequencer::instance().seq();
    if(midiThread_.joinable())
    {
        run_.store(false, std::memory_order::memory_order_release);
        midiThread_.join();
        snd_seq_control_queue(seq, queueId_, SND_SEQ_EVENT_STOP, 0, nullptr);
    }
    if(subscription_)
    {
        snd_seq_unsubscribe_port(seq, subscription_);
        snd_seq_port_subscribe_free(subscription_);
        subscription_ = nullptr;
    }
    if(queueId_ != -1)
    {
        snd_seq_free_queue(seq, queueId_);
    }
}
}

#endif