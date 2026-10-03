#include "PluginHostBypass.hpp"

#include "audio/host/HostContext.hpp"
#include "util/Util.hpp"

namespace YADAW::Audio::Mixer
{
plf::hive<PluginHostBypass::PassthroughDevice> PluginHostBypass::passthroughDevicePool_;
plf::hive<PluginHostBypass::BypassSwitcher> PluginHostBypass::bypassSwitcherPool_;

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
    if(routePair_)
    {
        auto& [from, to] = *routePair_;
        FOR_RANGE0(i, to)
        {
            FOR_RANGE0(j, audioProcessData.outputCounts[i])
            {
                std::fill_n(
                    audioProcessData.outputs[i][j],
                    audioProcessData.singleBufferSize,
                    0.0f
                );
            }
        }
        FOR_RANGE0(i, audioProcessData.outputCounts[to])
        {
            std::copy_n(
                audioProcessData.inputs[from][i],
                audioProcessData.singleBufferSize,
                audioProcessData.outputs[to][i]
            );
        }
        FOR_RANGE(i, to, audioProcessData.outputGroupCount)
        {
            FOR_RANGE0(j, audioProcessData.outputCounts[i])
            {
                std::fill_n(
                    audioProcessData.outputs[i][j],
                    audioProcessData.singleBufferSize,
                    0.0f
                );
            }
        }
    }
    else
    {
        FOR_RANGE0(i, audioProcessData.outputGroupCount)
        {
            FOR_RANGE0(j, audioProcessData.outputCounts[i])
            {
                std::fill_n(
                    audioProcessData.outputs[i][j],
                    audioProcessData.singleBufferSize,
                    0.0f
                );
            }
        }
    }
}

// PluginHostBypass::BypassSwitcher
PluginHostBypass::BypassSwitcher::BypassSwitcher(
    YADAW::Audio::Device::IAudioDevice& plugin):
    plugin_(&plugin),
    sampleRate_(0.0)
{}

PluginHostBypass::BypassSwitcher::~BypassSwitcher()
{
    uninitialize();
}

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
    sampleRate_ = 0.0;
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

void PluginHostBypass::BypassSwitcher::process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    if(sampleRate_ == 0.0)
    {
        return;
    }
    auto pluginBufferIndex = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get();
    if(auto size = this->timePoints_[pluginBufferIndex].size(); size == 0)
    {
        auto offset = lastValue_ * audioProcessData.outputGroupCount;
        FOR_RANGE0(i, audioProcessData.outputGroupCount)
        {
            FOR_RANGE0(j, audioProcessData.outputCounts[i])
            {
                std::copy_n(
                    audioProcessData.inputs[i + offset][j],
                    audioProcessData.singleBufferSize,
                    audioProcessData.outputs[i][j]
                );
            }
        }
    }
    else
    {
        //    ----o----o--o----o------
        // 1) ----
        auto inputOffset = lastValue_ * audioProcessData.outputGroupCount;
        FOR_RANGE0(i, audioProcessData.outputGroupCount)
        {
            FOR_RANGE0(j, audioProcessData.outputCounts[i])
            {
                std::copy_n(
                    audioProcessData.inputs[i + inputOffset][j]/* + 0 */,
                    timePoints_[pluginBufferIndex].front()/* - 0 */,
                    audioProcessData.outputs[i][j]/* +0 */
                );
            }
        }
        // 2)     o----o--o----
        FOR_RANGE0(h, timePoints_[pluginBufferIndex].size() - 1)
        {
            inputOffset = values_[pluginBufferIndex][h] * audioProcessData.outputGroupCount;
            FOR_RANGE0(i, audioProcessData.outputGroupCount)
            {
                FOR_RANGE0(j, audioProcessData.outputCounts[i])
                {
                    std::copy_n(
                        audioProcessData.inputs[i + inputOffset][j] + timePoints_[pluginBufferIndex][h],
                        timePoints_[pluginBufferIndex][h + 1] - timePoints_[pluginBufferIndex][h],
                        audioProcessData.outputs[i][j] + timePoints_[pluginBufferIndex][h]
                    );
                }
            }
        }
        // 3)                  o------
        inputOffset = timePoints_[pluginBufferIndex].back();
        FOR_RANGE0(i, audioProcessData.outputGroupCount)
        {
            FOR_RANGE0(j, audioProcessData.outputCounts[i])
            {
                std::copy_n(
                    audioProcessData.inputs[i + inputOffset][j] + timePoints_[pluginBufferIndex].back(),
                    audioProcessData.singleBufferSize - timePoints_[pluginBufferIndex].back(),
                    audioProcessData.outputs[i][j] + timePoints_[pluginBufferIndex].back()
                );
            }
        }
        lastValue_ = values_[pluginBufferIndex].back();
    }
}

bool PluginHostBypass::BypassSwitcher::getValue() const
{
    auto& hostBuffer = values_[YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get() ^ 1];
    return hostBuffer.empty()? lastValue_: hostBuffer.back();
}

void PluginHostBypass::BypassSwitcher::setValue(bool value)
{
    auto hostIndex = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get() ^ 1;
    std::uint32_t sampleOffset = std::round(
        (YADAW::Util::currentTimeValueInNanosecond() - switchTimestampInNanosecond_) / (sampleRate_ * std::giga::num)
    );
    timePoints_[hostIndex].emplace_back(sampleOffset);
    values_[hostIndex].emplace_back(value);
}

void PluginHostBypass::BypassSwitcher::onBufferSwitched(std::uint64_t switchTimestampInNanosecond)
{
    auto currentPluginBufferIndex = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get();
    switchTimestampInNanosecond_ = switchTimestampInNanosecond;
    auto currentHostBufferIndex = currentPluginBufferIndex ^ 1;
    timePoints_[currentHostBufferIndex].clear();
    values_[currentHostBufferIndex].clear();
}

// PluginHostBypass
PluginHostBypass::~PluginHostBypass()
{
    if(plugin_.index() == 0)
    {
        graph_->removeNode(std::get<0>(plugin_));
    }
    if(hostBypass_.index() == 0)
    {
        graph_->removeNode(std::get<0>(hostBypass_));
    }
    if(!PluginHostBypass::dismissed())
    {
        bypassSwitcher_->uninitialize();
        std::vector<YADAW::Util::PMRUniquePtr<void>> dismissedData;
        dismissedData.reserve(3);
        dismissRecursively(*this, std::back_inserter(dismissedData));
    }
}

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

std::optional<Engine::NodeSet::InputPosition>
    PluginHostBypass::inputAt(std::uint32_t index) const
{
    if(plugin_.index() == 0)
    {
        if(index < inputCount())
        {
            return std::vector<YADAW::Audio::Engine::NodeSet::NodePosition>{
                YADAW::Audio::Engine::NodeSet::NodePosition {
                    .node = std::get<0>(plugin_),
                    .index = index
                },
                YADAW::Audio::Engine::NodeSet::NodePosition {
                    .node = std::get<0>(hostBypass_),
                    .index = index
                }
            };
        }
        else
        {
            return std::nullopt;
        }
    }
    else
    {
        if(index < inputCount())
        {
            auto ret1 = std::get<1>(*(std::get<1>(plugin_)->inputAt(index)));
            auto ret2 = std::get<1>(*(std::get<1>(hostBypass_)->inputAt(index)));
            std::ranges::copy(ret2, std::back_inserter(ret1));
            return ret1;
        }
        else
        {
            return std::nullopt;
        }
    }
}

std::optional<YADAW::Audio::Engine::NodeSet::OutputPosition>
PluginHostBypass::outputAt(std::uint32_t index) const
{
    return switcherPDC_->outputAt(index);
}

YADAW::Util::PMRUniquePtr<void> PluginHostBypass::dismiss()
{
    struct DismissedData
    {
        decltype(PluginHostBypass::passthroughDevicePool_)::iterator passthroughDevice;
        decltype(PluginHostBypass::bypassSwitcherPool_)::iterator bypassSwitcher;
        DismissedData(PluginHostBypass& phb):
            passthroughDevice(
                PluginHostBypass::passthroughDevicePool_.get_iterator(
                    phb.passthroughDevice_
                )
            ),
            bypassSwitcher(
                PluginHostBypass::bypassSwitcherPool_.get_iterator(
                    phb.bypassSwitcher_
                )
            )
        {
            phb.passthroughDevice_ = nullptr;
            phb.bypassSwitcher_ = nullptr;
        }
        ~DismissedData()
        {
            bypassSwitcher->uninitialize();
            PluginHostBypass::passthroughDevicePool_.erase(passthroughDevice);
            PluginHostBypass::bypassSwitcherPool_.erase(bypassSwitcher);
        }
    };
    return YADAW::Util::createPMRUniquePtr(
        std::make_unique<DismissedData>(*this)
    );
}

bool PluginHostBypass::dismissed() const
{
    return passthroughDevice_ == nullptr && bypassSwitcher_ == nullptr;
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

const PluginHostBypass::BypassSwitcher& PluginHostBypass::bypassSwitcher() const
{
    return *bypassSwitcher_;
}

PluginHostBypass::BypassSwitcher& PluginHostBypass::bypassSwitcher()
{
    return *bypassSwitcher_;
}

bool PluginHostBypass::initialize(double sampleRate, std::uint32_t maxSampleCount)
{
    return bypassSwitcher_->initialize(sampleRate, maxSampleCount);
}

void PluginHostBypass::uninitialize()
{
    bypassSwitcher_->uninitialize();
}
}
