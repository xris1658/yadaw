#if _WIN32

#include "AudioGraphBusConfiguration.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Backend
{
using YADAW::Audio::Device::Channel;
using YADAW::Audio::Device::IAudioDevice;
using YADAW::Audio::Device::IBus;
using ChannelPosition = YADAW::Audio::Device::IAudioBusConfiguration::ChannelPosition;

AudioGraphBusConfiguration::Bus::Bus(
    bool isInput, std::uint32_t channelCount):
    IBus(channelCount),
    channels_(channelCount),
    isInput_(isInput),
    audioChannelGroup_()
{
    audioChannelGroup_.setChannelGroupType(
        YADAW::Audio::Base::ChannelGroupType::eCustomGroup,
        channelCount);
    audioChannelGroup_.setMain(true);
    FOR_RANGE0(i, channelCount)
    {
        audioChannelGroup_.setSpeakerType(
            i, YADAW::Audio::Base::ChannelType::Custom);
        audioChannelGroup_.setSpeakerName(i, QString("N/A"));
    }
}

AudioGraphBusConfiguration::Bus::Bus(bool isInput,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCount):
    IBus(channelGroupType, channelCount),
    channels_(channelCount_),
    buffers_(nullptr),
    isInput_(isInput),
    audioChannelGroup_()
{
    audioChannelGroup_.setChannelGroupType(
        channelGroupType_,
        channelCount_);
    audioChannelGroup_.setMain(true);
    FOR_RANGE0(i, channelCount_)
    {
        audioChannelGroup_.setSpeakerType(
            i, YADAW::Audio::Base::ChannelType::Custom);
        audioChannelGroup_.setSpeakerName(i, QString("N/A"));
    }
}

std::optional<Channel> AudioGraphBusConfiguration::Bus::channelAt(std::uint32_t index) const
{
    return index < channelCount_?
        std::optional<Channel>(channels_[index]):
        std::nullopt;
}

bool AudioGraphBusConfiguration::Bus::setChannel(std::uint32_t index, Channel channel)
{
    if(index < channels_.size())
    {
        channels_[index] = channel;
        audioChannelGroup_.setSpeakerName(index,
            QString("Device %1, Channel %2")
            .arg(channel.deviceIndex)
            .arg(channel.channelIndex)
        );
        return true;
    }
    return false;
}

std::uint32_t AudioGraphBusConfiguration::Bus::audioInputGroupCount() const
{
    return isInput_? 0: 1;
}

std::uint32_t AudioGraphBusConfiguration::Bus::audioOutputGroupCount() const
{
    return isInput_? 1: 0;
}

IAudioDevice::OptionalAudioChannelGroup AudioGraphBusConfiguration::Bus::audioInputGroupAt(std::uint32_t index) const
{
    return index < audioInputGroupCount()?
        OptionalAudioChannelGroup(std::ref(audioChannelGroup_)):
        std::nullopt;
}

IAudioDevice::OptionalAudioChannelGroup AudioGraphBusConfiguration::Bus::audioOutputGroupAt(std::uint32_t index) const
{
    return index < audioOutputGroupCount()?
        OptionalAudioChannelGroup(std::ref(audioChannelGroup_)):
        std::nullopt;
}

std::uint32_t AudioGraphBusConfiguration::Bus::latencyInSamples() const
{
    return 0;
}

using ProcessFunc = void(
    const AudioGraphBackend::InterleaveAudioBuffer* buffers,
    const std::vector<YADAW::Audio::Device::Channel>& channels,
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData);

void processInputs(
    const AudioGraphBackend::InterleaveAudioBuffer* buffers,
    const std::vector<YADAW::Audio::Device::Channel>& channels,
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        if(auto& buffer = buffers[channels[i].deviceIndex]; buffer.data)
        {
            FOR_RANGE0(j, audioProcessData.singleBufferSize)
            {
                std::memcpy(
                    audioProcessData.outputs[0][i] + j,
                    buffer.at(j, channels[i].channelIndex),
                    sizeof(float)
                );
            }
        }
    }
}

void processOutputs(
    const AudioGraphBackend::InterleaveAudioBuffer* buffers,
    const std::vector<YADAW::Audio::Device::Channel>& channels,
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    if(buffers[0].data)
    {
        FOR_RANGE0(i, audioProcessData.inputCounts[0])
        {
            FOR_RANGE0(j, audioProcessData.singleBufferSize)
            {
                std::memcpy(buffers[0].at(j, channels[i].channelIndex),
                    audioProcessData.inputs[0][i] + j,
                    sizeof(float)
                );
            }
        }
    }
}

ProcessFunc* doProcessFunc[2] = {&processOutputs, &processInputs};

void AudioGraphBusConfiguration::Bus::process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    doProcessFunc[isInput_](buffers_, channels_, audioProcessData);
}

void AudioGraphBusConfiguration::Bus::setName(const QString& name)
{
    audioChannelGroup_.setName(name);
}

void AudioGraphBusConfiguration::Bus::setName(QString&& name)
{
    audioChannelGroup_.setName(std::move(name));
}

void AudioGraphBusConfiguration::Bus::setInterleaveAudioBuffers(
    const YADAW::Audio::Backend::AudioGraphBackend::InterleaveAudioBuffer* buffers)
{
    buffers_ = buffers;
}

AudioGraphBusConfiguration::AudioGraphBusConfiguration(const AudioGraphBackend& backend):
    backend_(&backend),
    inputBusses_(),
    outputBusses_()
{
    //
}

std::uint32_t AudioGraphBusConfiguration::inputBusCount() const
{
    return inputBusses_.size();
}

std::uint32_t AudioGraphBusConfiguration::outputBusCount() const
{
    return outputBusses_.size();
}

OptionalRef<const IBus> AudioGraphBusConfiguration::inputBusAt(std::uint32_t index) const
{
    return index < inputBusCount()? OptionalRef<const IBus>(*inputBusses_[index]): std::nullopt;
}

OptionalRef<const IBus> AudioGraphBusConfiguration::outputBusAt(std::uint32_t index) const
{
    return index < outputBusCount()? OptionalRef<const IBus>(*outputBusses_[index]): std::nullopt;
}

OptionalRef<IBus> AudioGraphBusConfiguration::inputBusAt(std::uint32_t index)
{
    return index < inputBusCount()? OptionalRef<IBus>(*inputBusses_[index]): std::nullopt;
}

OptionalRef<IBus> AudioGraphBusConfiguration::outputBusAt(std::uint32_t index)
{
    return index < outputBusCount()? OptionalRef<IBus>(*outputBusses_[index]): std::nullopt;
}

ChannelPosition AudioGraphBusConfiguration::channelPosition(bool isInput, Channel channel) const
{
    const auto& busses = isInput? inputBusses_: outputBusses_;
    for(std::uint32_t i = 0; i < busses.size(); ++i)
    {
        const auto& bus = busses[i];
        for(std::uint32_t j = 0; j < bus->channelCount(); ++j)
        {
            auto channelInBus = bus->channelAt(j);
            if(channelInBus == channel)
            {
                return {i, j};
            }
        }
    }
    return {};
}

std::uint32_t AudioGraphBusConfiguration::appendBus(bool isInput,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCount)
{
    auto& bus = isInput? inputBusses_: outputBusses_;
    bus.emplace_back(
        std::make_unique<Bus>(isInput, channelGroupType, channelCount)
    );
    return bus.size() - 1;
}

bool AudioGraphBusConfiguration::insertBus(
    std::uint32_t position,
    bool isInput,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCount)
{
    auto& bus = isInput? inputBusses_: outputBusses_;
    if(position <= bus.size())
    {
        bus.emplace(
            bus.begin() + position,
            std::make_unique<Bus>(isInput, channelGroupType, channelCount)
        );
        return true;
    }
    return false;
}

bool AudioGraphBusConfiguration::removeBus(bool isInput, std::uint32_t index)
{
    auto& busses = isInput? inputBusses_: outputBusses_;
    if(index < busses.size())
    {
        busses.erase(busses.begin() + index);
        return true;
    }
    return false;
}

void AudioGraphBusConfiguration::clearBus(bool isInput)
{
    auto& busses = isInput? inputBusses_: outputBusses_;
    busses.clear();
}

OptionalRef<const AudioGraphBusConfiguration::Bus>
AudioGraphBusConfiguration::getInputBusAt(std::uint32_t index) const
{
    return index < inputBusCount()? OptionalRef<const Bus>(*inputBusses_[index]): std::nullopt;
}

OptionalRef<const AudioGraphBusConfiguration::Bus>
AudioGraphBusConfiguration::getOutputBusAt(std::uint32_t index) const
{
    return index < outputBusCount()? OptionalRef<const Bus>(*outputBusses_[index]): std::nullopt;
}

OptionalRef<AudioGraphBusConfiguration::Bus>
AudioGraphBusConfiguration::getInputBusAt(std::uint32_t index)
{
    return index < inputBusCount()? OptionalRef<Bus>(*inputBusses_[index]): std::nullopt;
}

OptionalRef<AudioGraphBusConfiguration::Bus>
AudioGraphBusConfiguration::getOutputBusAt(std::uint32_t index)
{
    return index < outputBusCount()? OptionalRef<Bus>(*outputBusses_[index]): std::nullopt;
}

void AudioGraphBusConfiguration::setBuffers(const AudioGraphBackend::InterleaveAudioBuffer* inputs,
    const AudioGraphBackend::InterleaveAudioBuffer* outputs)
{
    for(auto& bus: inputBusses_)
    {
        bus->setInterleaveAudioBuffers(inputs);
    }
    for(auto& bus: outputBusses_)
    {
        bus->setInterleaveAudioBuffers(outputs);
    }
}
}

#endif