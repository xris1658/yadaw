#include "MultiInputDeviceWithPDC.hpp"

namespace YADAW::Audio::Engine
{
MultiInputDeviceWithPDC::MultiInputDeviceWithPDC(AudioDeviceProcess&& process):
    process_(std::move(process))
{
    auto device = process_.device();
    auto inputCount = device->audioInputGroupCount();
    auto outputCount = device->audioOutputGroupCount();
    pdcs_.reserve(inputCount);
    intermediateContainers_.reserve(inputCount);
    deviceProcessDataContainer_.setInputGroupCount(inputCount);
    deviceProcessDataContainer_.setOutputGroupCount(outputCount);
    FOR_RANGE0(i, inputCount)
    {
        auto& inputGroup = device->audioInputGroupAt(i)->get();
        auto& pdc = pdcs_.emplace_back(0U, inputGroup);
        pdc.startProcessing();
        auto& container = intermediateContainers_.emplace_back();
        container.setInputGroupCount(1);
        container.setOutputGroupCount(1);
        auto channelCount = inputGroup.channelCount();
        container.setInputCount(0, channelCount);
        container.setOutputCount(0, channelCount);
        deviceProcessDataContainer_.setInputCount(i, channelCount);
    }
}

MultiInputDeviceWithPDC::~MultiInputDeviceWithPDC()
{}

std::uint32_t MultiInputDeviceWithPDC::audioInputGroupCount() const
{
    return process_.device()->audioInputGroupCount();
}

std::uint32_t MultiInputDeviceWithPDC::audioOutputGroupCount() const
{
    return process_.device()->audioOutputGroupCount();
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    MultiInputDeviceWithPDC::audioInputGroupAt(std::uint32_t index) const
{
    return process_.device()->audioInputGroupAt(index);
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    MultiInputDeviceWithPDC::audioOutputGroupAt(std::uint32_t index) const
{
    return process_.device()->audioOutputGroupAt(index);
}

std::uint32_t MultiInputDeviceWithPDC::latencyInSamples() const
{
    return process_.device()->latencyInSamples();
}

void MultiInputDeviceWithPDC::process(const AudioProcessData<float>& audioProcessData)
{
    FOR_RANGE0(i, pdcs_.size())
    {
        pdcs_[i].process(intermediateContainers_[i].audioProcessData());
    }
    auto data = deviceProcessDataContainer_.audioProcessData();
    data.outputGroupCount = audioProcessData.outputGroupCount;
    data.outputCounts = audioProcessData.outputCounts;
    data.outputs = audioProcessData.outputs;
    process_.process(data);
}

void MultiInputDeviceWithPDC::setDelayOfPDC(std::uint32_t audioInputGroupIndex, std::uint32_t delay)
{
    if(audioInputGroupIndex < pdcs_.size())
    {
        auto& pdc = pdcs_[audioInputGroupIndex];
        pdc.stopProcessing();
        pdcs_[audioInputGroupIndex].setDelay(delay);
        pdc.startProcessing();
    }
}

void MultiInputDeviceWithPDC::setBufferSize(std::uint32_t newBufferSize)
{
    auto pool = YADAW::Audio::Util::AudioBufferPool::createPool<float>(newBufferSize);
    decltype(intermediateBuffers_) intermediateBuffers;
    FOR_RANGE0(i, intermediateContainers_.size())
    {
        auto& buffers = intermediateBuffers.emplace_back();
        const auto& processData = intermediateContainers_[i].audioProcessData();
        FOR_RANGE0(j, processData.outputCounts[0])
        {
            auto& buffer = buffers.emplace_back(pool->lend());
            intermediateContainers_[i].setOutput(
                0, j, reinterpret_cast<float*>(buffer.pointer())
            );
            deviceProcessDataContainer_.setInput(
                0, j, reinterpret_cast<float*>(buffer.pointer())
            );
        }
    }
    pool_ = std::move(pool);
    intermediateBuffers_ = std::move(intermediateBuffers);
}

const YADAW::Audio::Engine::AudioDeviceProcess MultiInputDeviceWithPDC::process() const
{
    return process_;
}

YADAW::Audio::Engine::AudioDeviceProcess MultiInputDeviceWithPDC::process()
{
    return process_;
}

std::optional<std::uint32_t> MultiInputDeviceWithPDC::getDelayOfPDC(
    std::uint32_t audioInputGroupIndex) const
{
    if(audioInputGroupIndex < pdcs_.size())
    {
        return {pdcs_[audioInputGroupIndex].latencyInSamples()};
    }
    return std::nullopt;
}

std::optional<std::uint32_t>
MultiInputDeviceWithPDC::getPDCIndexOfMaximumDelay() const
{
    if(pdcs_.empty())
    {
        return std::nullopt;
    }
    using YADAW::Audio::Util::SampleDelay;
    return std::max_element(pdcs_.begin(), pdcs_.end(),
        [](const SampleDelay& lhs, const SampleDelay& rhs)
        {
            return lhs.latencyInSamples() < rhs.latencyInSamples();
        }
    ) - pdcs_.begin();
}
}