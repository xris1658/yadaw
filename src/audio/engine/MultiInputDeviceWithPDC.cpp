#include "MultiInputDeviceWithPDC.hpp"

namespace YADAW::Audio::Engine
{
MultiInputDeviceWithPDC::MultiInputDeviceWithPDC(AudioDeviceProcess&& process):
    process_(std::move(process))
{
    auto device = process_.device();
    auto inputCount = device->audioInputGroupCount();
    pdcs_.reserve(inputCount);
    pdcAudioProcessData_.reserve(inputCount);
    FOR_RANGE0(i, inputCount)
    {
        auto& inputGroup = device->audioInputGroupAt(i)->get();
        pdcs_.emplace_back(0U, inputGroup);
        auto& audioProcessData = pdcAudioProcessData_.emplace_back();
        audioProcessData.inputGroupCount = 1;
        audioProcessData.outputGroupCount = 1;
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
        pdcAudioProcessData_[i].singleBufferSize = audioProcessData.singleBufferSize;
        pdcAudioProcessData_[i].inputCounts = audioProcessData.inputCounts + i;
        pdcAudioProcessData_[i].outputCounts = audioProcessData.inputCounts + i;
        pdcAudioProcessData_[i].inputs = audioProcessData.inputs + i;
        pdcAudioProcessData_[i].outputs = audioProcessData.inputs + i;
    }
    FOR_RANGE0(i, pdcs_.size())
    {
        pdcs_[i].process(pdcAudioProcessData_[i]);
    }
    process_.process(audioProcessData);
}

void MultiInputDeviceWithPDC::setDelayOfPDC(std::uint32_t audioInputGroupIndex, std::uint32_t delay)
{
    if(audioInputGroupIndex < pdcs_.size())
    {
        pdcs_[audioInputGroupIndex].setDelay(delay);
    }
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