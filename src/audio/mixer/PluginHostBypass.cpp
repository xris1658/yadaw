#include "PluginHostBypass.hpp"

namespace YADAW::Audio::Mixer
{
// PluginHostBypass::PassthroughDevice
PluginHostBypass::PassthroughDevice::PassthroughDevice(
    YADAW::Audio::Device::IAudioDevice& plugin
):
    inputCount_(plugin.audioInputGroupCount())
{
    channelGroups_.reserve(plugin.audioInputGroupCount() + plugin.audioOutputGroupCount());
    std::ranges::move(
        std::ranges::iota_view(0U, plugin.audioInputGroupCount())
        | std::views::transform([&plugin](std::uint32_t index)
        {
            return YADAW::Audio::Util::AudioChannelGroup::from(
                plugin.audioInputGroupAt(index)->get()
            );
        }),
        std::back_inserter(channelGroups_)
    );
    std::ranges::move(
        std::ranges::iota_view(0U, plugin.audioOutputGroupCount())
        | std::views::transform([&plugin](std::uint32_t index)
        {
            return YADAW::Audio::Util::AudioChannelGroup::from(
                plugin.audioOutputGroupAt(index)->get()
            );
        }),
        std::back_inserter(channelGroups_)
    );
    if(auto firstMainInput = std::find_if(
        channelGroups_.begin(), channelGroups_.begin() + inputCount_,
        [](const YADAW::Audio::Util::AudioChannelGroup& group) { return group.isMain(); }
    ); firstMainInput != channelGroups_.begin() + inputCount_)
    {
        if(auto firstMainOutput = std::find_if(
            channelGroups_.begin() + inputCount_, channelGroups_.end(),
            [](const YADAW::Audio::Util::AudioChannelGroup& group) { return group.isMain(); }
        ); firstMainOutput != channelGroups_.end()
        && firstMainInput->type() == firstMainOutput->type()
        {
            routePair_ = std::pair(
                std::distance(channelGroups_.begin(), firstMainInput),
                std::distance(channelGroups_.begin() + inputCount_, firstMainOutput)
            );
        }
    }
}

PluginHostBypass::PassthroughDevice::~PassthroughDevice()
{}

std::uint32_t PluginHostBypass::PassthroughDevice::audioInputGroupCount() const
{
    return inputCount_;
}

std::uint32_t PluginHostBypass::PassthroughDevice::audioOutputGroupCount() const
{
    return channelGroups_.size() - audioInputGroupCount();
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
PluginHostBypass::PassthroughDevice::audioInputGroupAt(std::uint32_t index) const
{
    if(index < audioInputGroupCount())
    {
        return channelGroups_[index];
    }
    return std::nullopt;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
PluginHostBypass::PassthroughDevice::audioOutputGroupAt(std::uint32_t index) const
{
    if(index < audioOutputGroupCount())
    {
        return channelGroups_[index + inputCount_];
    }
    return std::nullopt;
}

std::uint32_t PluginHostBypass::PassthroughDevice::latencyInSamples() const
{
    return 0U;
}

void PluginHostBypass::PassthroughDevice::process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    // TODO
}

// PluginHostBypass::BypassSwitcher
PluginHostBypass::BypassSwitcher::BypassSwitcher(YADAW::Audio::Device::IAudioDevice& plugin)
{}

PluginHostBypass::BypassSwitcher::~BypassSwitcher()
{}

bool PluginHostBypass::BypassSwitcher::initialize(double sampleRate, std::uint32_t maxSampleCount)
{}

void PluginHostBypass::BypassSwitcher::uninitialize()
{}

std::uint32_t PluginHostBypass::BypassSwitcher::audioInputGroupCount() const
{}

std::uint32_t PluginHostBypass::BypassSwitcher::audioOutputGroupCount() const
{}

Device::IAudioDevice::OptionalAudioChannelGroup PluginHostBypass::BypassSwitcher::audioInputGroupAt(
    std::uint32_t index) const
{}

Device::IAudioDevice::OptionalAudioChannelGroup PluginHostBypass::BypassSwitcher::audioOutputGroupAt(
    std::uint32_t index) const
{}

std::uint32_t PluginHostBypass::BypassSwitcher::latencyInSamples() const
{}

void PluginHostBypass::BypassSwitcher::process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{}

// PluginHostBypass
ade::NodeHandle PluginHostBypass::pluginNode() const
{}

std::uint32_t PluginHostBypass::inputCount() const
{}

std::uint32_t PluginHostBypass::outputCount() const
{}

std::optional<Engine::NodeSet::Position> PluginHostBypass::inputAt(std::uint32_t index) const
{}

std::optional<Engine::NodeSet::Position> PluginHostBypass::outputAt(std::uint32_t index) const
{}

YADAW::Util::PMRUniquePtr<void> PluginHostBypass::dismiss()
{
    return NodeSet::dismiss();
}

bool PluginHostBypass::dismissed() const
{}

std::uint32_t PluginHostBypass::innerNodeSetCount() const
{
    return NodeSet::innerNodeSetCount();
}

OptionalRef<Engine::NodeSet> PluginHostBypass::innerNodeSetAt(std::uint32_t index)
{
    return NodeSet::innerNodeSetAt(index);
}

OptionalRef<const Engine::NodeSet> PluginHostBypass::innerNodeSetAt(std::uint32_t index) const
{
    return NodeSet::innerNodeSetAt(index);
}
}
