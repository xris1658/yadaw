#if __linux__

#include "ALSABusConfiguration.hpp"

#include "util/IntegerRange.hpp"
#include "util/SampleFormat.hpp"

namespace YADAW::Audio::Backend
{
using YADAW::Audio::Device::IAudioDevice;
using YADAW::Audio::Device::Channel;
using YADAW::Audio::Device::IBus;
using ChannelPosition = YADAW::Audio::Device::IAudioBusConfiguration::ChannelPosition;

ALSABusConfiguration::Bus::Bus(bool isInput,
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

std::optional<Channel> ALSABusConfiguration::Bus::channelAt(std::uint32_t index) const
{
    return index < channelCount_?
        std::optional<Channel>(channels_[index]):
        std::nullopt;
}

bool ALSABusConfiguration::Bus::setChannel(std::uint32_t index, Channel channel)
{
    if(index < channels_.size())
    {
        channels_[index] = channel;
        return true;
    }
    return false;
}

std::uint32_t ALSABusConfiguration::Bus::audioInputGroupCount() const
{
    return isInput_? 0: 1;
}

std::uint32_t ALSABusConfiguration::Bus::audioOutputGroupCount() const
{
    return isInput_? 1: 0;
}

IAudioDevice::OptionalAudioChannelGroup ALSABusConfiguration::Bus::audioInputGroupAt(std::uint32_t index) const
{
    return index < audioInputGroupCount()?
        OptionalAudioChannelGroup(std::ref(audioChannelGroup_)):
        std::nullopt;
}

IAudioDevice::OptionalAudioChannelGroup ALSABusConfiguration::Bus::audioOutputGroupAt(std::uint32_t index) const
{
    return index < audioOutputGroupCount()?
        OptionalAudioChannelGroup(std::ref(audioChannelGroup_)):
        std::nullopt;
}

std::uint32_t ALSABusConfiguration::Bus::latencyInSamples() const
{
    return 0;
}

void processInputs(
    const ALSABackend::AudioBuffer* buffers,
    const std::vector<Channel>& channels,
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        auto [deviceIndex, channelIndex] = channels[i];
        if(auto& buffer = buffers[deviceIndex]; buffer.interleaved)
        {
            auto interleavedBuffer = buffer.buffer.interleavedBuffer;
            switch(buffer.format)
            {
            case ALSABackend::SampleFormat::DoubleFloat:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<double*>(interleavedBuffer)[j * buffer.channelCount + i];
                }
                break;
            }
            case ALSABackend::SampleFormat::DoubleFloatRE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<YADAW::Util::DoubleRE*>(interleavedBuffer)[j * buffer.channelCount + i];
                }
                break;
            }
            case ALSABackend::SampleFormat::Float:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<float*>(interleavedBuffer)[j * buffer.channelCount + i];
                }
                break;
            }
            case ALSABackend::SampleFormat::FloatRE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<YADAW::Util::FloatRE*>(interleavedBuffer)[j * buffer.channelCount + i];
                }
                break;
            }
            case ALSABackend::SampleFormat::Int32:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<std::int32_t*>(interleavedBuffer)[j * buffer.channelCount + i]
                        / static_cast<float>(1LL << 31);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int32RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<YADAW::Util::Int32RE*>(interleavedBuffer)[j * buffer.channelCount + i]
                        / static_cast<float>(1LL << 31);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int24:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<std::int32_t*>(interleavedBuffer)[j * buffer.channelCount + i]
                        / static_cast<float>(1LL << 23);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int24RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<YADAW::Util::Int32RE*>(interleavedBuffer)[j * buffer.channelCount + i]
                        / static_cast<float>(1LL << 23);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int20:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<std::int32_t*>(interleavedBuffer)[j * buffer.channelCount + i]
                        / static_cast<float>(1LL << 19);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int20RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<YADAW::Util::Int32RE*>(interleavedBuffer)[j * buffer.channelCount + i]
                        / static_cast<float>(1LL << 23);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int16:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<std::int16_t*>(interleavedBuffer)[j * buffer.channelCount + i]
                        / static_cast<float>(1LL << 15);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int16RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<YADAW::Util::Int16RE*>(interleavedBuffer)[j * buffer.channelCount + i]
                        / static_cast<float>(1LL << 15);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int8:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<std::int8_t*>(interleavedBuffer)[j * buffer.channelCount + i]
                        / static_cast<float>(1LL << 7);
                }
                break;
            }
            default:
                break;
            }
        }
        else
        {
            auto nonInterleavedBuffer = buffer.buffer.nonInterleavedBuffer[channelIndex];
            switch(buffer.format)
            {
            case ALSABackend::SampleFormat::DoubleFloat:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<double*>(nonInterleavedBuffer)[j];
                }
                break;
            }
            case ALSABackend::SampleFormat::DoubleFloatRE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<YADAW::Util::DoubleRE*>(nonInterleavedBuffer)[j];
                }
                break;
            }
            case ALSABackend::SampleFormat::Float:
            {
                auto dest = audioProcessData.outputs[0][i];
                std::memcpy(dest, nonInterleavedBuffer,
                    audioProcessData.singleBufferSize * sizeof(float)
                );
                break;
            }
            case ALSABackend::SampleFormat::FloatRE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<YADAW::Util::FloatRE*>(nonInterleavedBuffer)[j];
                }
                break;
            }
            case ALSABackend::SampleFormat::Int32:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<std::int32_t*>(nonInterleavedBuffer)[j]
                        / static_cast<float>(1LL << 31);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int32RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<YADAW::Util::Int32RE*>(nonInterleavedBuffer)[j]
                        / static_cast<float>(1LL << 31);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int24:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<std::int32_t*>(nonInterleavedBuffer)[j]
                        / static_cast<float>(1LL << 23);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int24RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<YADAW::Util::Int32RE*>(nonInterleavedBuffer)[j]
                        / static_cast<float>(1LL << 23);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int20:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<std::int32_t*>(nonInterleavedBuffer)[j]
                        / static_cast<float>(1LL << 19);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int20RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<YADAW::Util::Int32RE*>(nonInterleavedBuffer)[j]
                        / static_cast<float>(1LL << 19);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int16:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<std::int16_t*>(nonInterleavedBuffer)[j]
                        / static_cast<float>(1LL << 15);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int16RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<YADAW::Util::Int16RE*>(nonInterleavedBuffer)[j]
                        / static_cast<float>(1LL << 15);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int8:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.outputs[0][i][j] =
                        static_cast<std::int8_t*>(nonInterleavedBuffer)[j]
                        / static_cast<float>(1LL << 7);
                }
                break;
            }
            default:
                break;
            }
        }
    }
}

void processOutputs(
    const ALSABackend::AudioBuffer* buffers,
    const std::vector<Channel>& channels,
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    FOR_RANGE0(i, audioProcessData.inputCounts[0])
    {
        auto [deviceIndex, channelIndex] = channels[i];
        if(auto& buffer = buffers[deviceIndex]; buffer.interleaved)
        {
            auto interleavedBuffer = buffer.buffer.interleavedBuffer;
            switch(buffer.format)
            {
            case ALSABackend::SampleFormat::DoubleFloat:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<double*>(interleavedBuffer)[j * buffer.channelCount + i] =
                        audioProcessData.inputs[0][i][j];
                }
                break;
            }
            case ALSABackend::SampleFormat::DoubleFloatRE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<YADAW::Util::FloatRE*>(interleavedBuffer)[j * buffer.channelCount + i]
                        = audioProcessData.inputs[0][i][j];
                }
                break;
            }
            case ALSABackend::SampleFormat::Float:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<float*>(interleavedBuffer)[j * buffer.channelCount + i] =
                        audioProcessData.inputs[0][i][j];
                }
                break;
            }
            case ALSABackend::SampleFormat::FloatRE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<YADAW::Util::FloatRE*>(interleavedBuffer)[j * buffer.channelCount + i]
                        = audioProcessData.inputs[0][i][j];
                }
                break;
            }
            case ALSABackend::SampleFormat::Int32:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<std::int32_t*>(interleavedBuffer)[j * buffer.channelCount + i] =
                        audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 31);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int32RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<YADAW::Util::Int32RE*>(interleavedBuffer)[j * buffer.channelCount + i]
                        = audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 31);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int24:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<std::int32_t*>(interleavedBuffer)[j * buffer.channelCount + i] =
                        audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 23);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int24RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<YADAW::Util::Int32RE*>(interleavedBuffer)[j * buffer.channelCount + i] =
                        audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 23);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int20:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<std::int32_t*>(interleavedBuffer)[j * buffer.channelCount + i] =
                        audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 19);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int20RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<YADAW::Util::Int32RE*>(interleavedBuffer)[j * buffer.channelCount + i] =
                        audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 19);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int16:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<std::int16_t*>(interleavedBuffer)[j * buffer.channelCount + i] =
                        audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 15);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int16RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<YADAW::Util::Int16RE*>(interleavedBuffer)[j * buffer.channelCount + i] =
                        audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 15);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int8:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<std::int8_t*>(interleavedBuffer)[j * buffer.channelCount + i] =
                        audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 7);
                }
                break;
            }
            default:
                break;
            }
        }
        else
        {
            auto nonInterleavedBuffer = buffer.buffer.nonInterleavedBuffer[channelIndex];
            switch(buffer.format)
            {
            case ALSABackend::SampleFormat::DoubleFloat:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<double*>(nonInterleavedBuffer)[j]
                        = audioProcessData.inputs[0][i][j];
                }
                break;
            }
            case ALSABackend::SampleFormat::DoubleFloatRE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<YADAW::Util::DoubleRE*>(nonInterleavedBuffer)[j]
                        = audioProcessData.inputs[0][i][j];
                }
                break;
            }
            case ALSABackend::SampleFormat::Float:
            {
                auto src = audioProcessData.inputs[0][i];
                std::memcpy(nonInterleavedBuffer, src,
                    audioProcessData.singleBufferSize * sizeof(float)
                );
                break;
            }
            case ALSABackend::SampleFormat::FloatRE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<YADAW::Util::FloatRE*>(nonInterleavedBuffer)[j]
                        = audioProcessData.inputs[0][i][j];
                }
                break;
            }
            case ALSABackend::SampleFormat::Int32:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.inputs[0][i][j] =
                        static_cast<std::int32_t*>(nonInterleavedBuffer)[j]
                        / static_cast<float>(1LL << 31);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int32RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    audioProcessData.inputs[0][i][j] =
                        static_cast<YADAW::Util::Int32RE*>(nonInterleavedBuffer)[j]
                        / static_cast<float>(1LL << 31);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int24:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<std::int32_t*>(nonInterleavedBuffer)[j]
                        = audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 23);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int24RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<YADAW::Util::Int32RE*>(nonInterleavedBuffer)[j]
                        = audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 23);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int20:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<std::int32_t*>(nonInterleavedBuffer)[j]
                        = audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 19);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int20RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<YADAW::Util::Int32RE*>(nonInterleavedBuffer)[j]
                        = audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 19);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int16:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<std::int16_t*>(nonInterleavedBuffer)[j]
                        = audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 15);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int16RE:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<YADAW::Util::Int16RE*>(nonInterleavedBuffer)[j]
                        = audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 15);
                }
                break;
            }
            case ALSABackend::SampleFormat::Int8:
            {
                FOR_RANGE0(j, audioProcessData.singleBufferSize)
                {
                    static_cast<std::int8_t*>(nonInterleavedBuffer)[j]
                        = audioProcessData.inputs[0][i][j] * static_cast<float>(1LL << 7);
                }
                break;
            }
            default:
                break;
            }
        }
    }
}

using ProcessFunc = decltype(processInputs);

ProcessFunc* doProcessFunc[2] = {&processOutputs, &processInputs};

void ALSABusConfiguration::Bus::process(const Device::AudioProcessData<float>& audioProcessData)
{
    doProcessFunc[isInput_](buffers_, channels_, audioProcessData);
}

void ALSABusConfiguration::Bus::setName(const QString& name)
{
    audioChannelGroup_.setName(name);
}

void ALSABusConfiguration::Bus::setName(QString&& name)
{
    audioChannelGroup_.setName(std::move(name));
}

void ALSABusConfiguration::Bus::setAudioBuffers(const YADAW::Audio::Backend::ALSABackend::AudioBuffer* buffers)
{
    buffers_ = buffers;
}

uint32_t ALSABusConfiguration::Bus::audioChannelMapCount() const
{
    return 0;
}

YADAW::Audio::Device::IAudioDevice::OptionalChannelMap
    ALSABusConfiguration::Bus::audioChannelMapAt(std::uint32_t index) const
{
    return std::nullopt;
}

ALSABusConfiguration::ALSABusConfiguration(const ALSABackend& backend):
    backend_(&backend),
    inputBusses_(),
    outputBusses_()
{}

std::uint32_t ALSABusConfiguration::inputBusCount() const
{
    return inputBusses_.size();
}

std::uint32_t ALSABusConfiguration::outputBusCount() const
{
    return outputBusses_.size();
}

OptionalRef<const IBus> ALSABusConfiguration::inputBusAt(std::uint32_t index) const
{
    return index < inputBusCount()?
        OptionalRef<const IBus>(*inputBusses_[index]):
        std::nullopt;
}

OptionalRef<const IBus> ALSABusConfiguration::outputBusAt(std::uint32_t index) const
{
    return index < outputBusCount()?
       OptionalRef<const IBus>(*outputBusses_[index]):
       std::nullopt;
}

OptionalRef<IBus> ALSABusConfiguration::inputBusAt(std::uint32_t index)
{
    return index < inputBusCount()?
       OptionalRef<IBus>(*inputBusses_[index]):
       std::nullopt;
}

OptionalRef<IBus> ALSABusConfiguration::outputBusAt(std::uint32_t index)
{
    return index < outputBusCount()?
       OptionalRef<IBus>(*outputBusses_[index]):
       std::nullopt;
}

ChannelPosition ALSABusConfiguration::channelPosition(bool isInput, Channel channel) const
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

std::uint32_t ALSABusConfiguration::appendBus(bool isInput,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCount)
{
    auto& bus = isInput? inputBusses_: outputBusses_;
    bus.emplace_back(
        std::make_unique<Bus>(isInput, channelGroupType, channelCount)
    );
    return bus.size() - 1;
}

bool ALSABusConfiguration::removeBus(bool isInput, std::uint32_t index)
{
    auto& busses = isInput? inputBusses_: outputBusses_;
    if(index < busses.size())
    {
        busses.erase(busses.begin() + index);
        return true;
    }
    return false;
}

void ALSABusConfiguration::clearBus(bool isInput)
{
    auto& busses = isInput? inputBusses_: outputBusses_;
    busses.clear();
}

OptionalRef<const ALSABusConfiguration::Bus> ALSABusConfiguration::getInputBusAt(std::uint32_t index) const
{
    return index < inputBusCount()? OptionalRef<const Bus>(*inputBusses_[index]): std::nullopt;
}

OptionalRef<const ALSABusConfiguration::Bus> ALSABusConfiguration::getOutputBusAt(std::uint32_t index) const
{
    return index < inputBusCount()? OptionalRef<const Bus>(*outputBusses_[index]): std::nullopt;
}

OptionalRef<ALSABusConfiguration::Bus> ALSABusConfiguration::getInputBusAt(std::uint32_t index)
{
    return index < inputBusCount()? OptionalRef<Bus>(*inputBusses_[index]): std::nullopt;
}

OptionalRef<ALSABusConfiguration::Bus> ALSABusConfiguration::getOutputBusAt(std::uint32_t index)
{
    return index < inputBusCount()? OptionalRef<Bus>(*outputBusses_[index]): std::nullopt;
}

void ALSABusConfiguration::setBuffers(const ALSABackend::AudioBuffer* inputs, const ALSABackend::AudioBuffer* outputs)
{
    for(auto& bus: inputBusses_)
    {
        bus->setAudioBuffers(inputs);
    }
    for(auto& bus: outputBusses_)
    {
        bus->setAudioBuffers(outputs);
    }
}
}

#endif