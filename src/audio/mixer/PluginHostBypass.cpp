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
        && firstMainInput->type() == firstMainOutput->type())
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
PluginHostBypass::BypassSwitcher::BypassSwitcher(
    YADAW::Audio::Device::IAudioDevice& plugin):
    plugin_(&plugin)
{}

PluginHostBypass::BypassSwitcher::~BypassSwitcher()
{}

bool PluginHostBypass::BypassSwitcher::initialize(double sampleRate, std::uint32_t maxSampleCount)
{
    {
        if(sampleRate > 0
            && maxSampleCount > 0
            && maxSampleCount < values_[0].max_size())
        {
            sampleRate_ = sampleRate;
            values_[0].resize(maxSampleCount, false);
            values_[1].resize(maxSampleCount, false);
            return true;
        }
        return false;
    }
}

void PluginHostBypass::BypassSwitcher::uninitialize()
{
    values_[0].clear();
    values_[0].shrink_to_fit();
    values_[1].clear();
    values_[1].shrink_to_fit();
}

std::uint32_t PluginHostBypass::BypassSwitcher::audioInputGroupCount() const
{
    return plugin_->audioOutputGroupCount() * 2;
}

std::uint32_t PluginHostBypass::BypassSwitcher::audioOutputGroupCount() const
{
    return plugin_->audioOutputGroupCount();
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
PluginHostBypass::BypassSwitcher::audioInputGroupAt(std::uint32_t index) const
{
    if(index < audioInputGroupCount())
    {
        return plugin_->audioOutputGroupAt(index % plugin_->audioOutputGroupCount());
    }
    return std::nullopt;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
PluginHostBypass::BypassSwitcher::audioOutputGroupAt(std::uint32_t index) const
{
    return plugin_->audioOutputGroupAt(index);
}

std::uint32_t PluginHostBypass::BypassSwitcher::latencyInSamples() const
{
    return 0U;
}

void PluginHostBypass::BypassSwitcher::process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    // TODO
}

// PluginHostBypass
ade::NodeHandle PluginHostBypass::pluginNode() const
{
    return plugin_.index() == 0?
        std::get<0>(plugin_):
        std::get<1>(plugin_)->deviceNode();
}

std::uint32_t PluginHostBypass::inputCount() const
{
    return graph_->getNodeData(pluginNode()).process.device()->audioInputGroupCount();
}

std::uint32_t PluginHostBypass::outputCount() const
{
    return switcherPDC_->outputCount();
}

std::optional<YADAW::Audio::Engine::NodeSet::Position>
PluginHostBypass::inputAt(std::uint32_t index) const
{
    if(plugin_.index() == 0)
    {
        if(index < inputCount())
        {
            return YADAW::Audio::Engine::NodeSet::NodePosition {
                .node = std::get<0>(plugin_),
                .index = index
            };
        }
        else
        {
            return std::nullopt;
        }
    }
    else
    {
        return std::get<1>(plugin_)->inputAt(index);
    }
}

std::optional<YADAW::Audio::Engine::NodeSet::Position>
PluginHostBypass::outputAt(std::uint32_t index) const
{
    return switcherPDC_->outputAt(index);
}

YADAW::Util::PMRUniquePtr<void> PluginHostBypass::dismiss()
{
    return NodeSet::dismiss(); // TODO
}

bool PluginHostBypass::dismissed() const
{
    return false; // TODO
}

std::uint32_t PluginHostBypass::innerNodeSetCount() const
{
    return (plugin_.index() == 1) + (hostBypass_.index() == 1) + 1;
}

OptionalRef<YADAW::Audio::Engine::NodeSet>
PluginHostBypass::innerNodeSetAt(std::uint32_t index)
{
    if(index == innerNodeSetCount() - 1)
    {
        return *switcherPDC_;
    }
    if(innerNodeSetCount() == 3)
    {
        if(index == 0)
        {
            return *std::get<1>(plugin_);
        }
        else if(index == 1)
        {
            return *std::get<1>(hostBypass_);
        }
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Engine::NodeSet>
PluginHostBypass::innerNodeSetAt(std::uint32_t index) const
{
    if(index == innerNodeSetCount() - 1)
    {
        return *switcherPDC_;
    }
    if(innerNodeSetCount() == 3)
    {
        if(index == 0)
        {
            return *std::get<1>(plugin_);
        }
        else if(index == 1)
        {
            return *std::get<1>(hostBypass_);
        }
    }
    return std::nullopt;
}
}
