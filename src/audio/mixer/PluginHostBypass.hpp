#ifndef YADAW_SRC_AUDIO_MIXER_PLUGINHOSTBYPASS
#define YADAW_SRC_AUDIO_MIXER_PLUGINHOSTBYPASS

#include "audio/device/IAudioDevice.hpp"
#include "audio/engine/NodeSet.hpp"
#include "audio/engine/PDC.hpp"
#include "audio/host/HostContext.hpp"

#include <cstdint>
#include <optional>

namespace YADAW::Audio::Mixer
{
// `PluginHostBypass` adds bypass capability to plugins that does not support
// bypassing internally.
// `PluginHostBypass` creates a `PassthroughDevice` that replicates the plugin's
// I/O. The input signal group is split into two flows, routing to the plugin
// and the passthrough device respectively. The two output groups flow to a
// sample-accurate switcher that chooses which output group is used.
// Since the switcher accepts at least two inputs and the plugin might introduce
// latency, we need to wrap the switcher into a `PDC`. We also need to wrap the
// plugin and the `PassthroughDevice` into two their own `PDC`s if the plugin is
// a multi-input one.
// For multi I/O plugins, we have to figure out the internal signal flow of the
// plugin so that `PassthroughDevice` works properly. If the plugin has no such
// routing info, we have to make some assumptions:
// - If the plugin has exactly 1 main input and exactly 1 main output, and the
//   input / output channel group type are the same, then the main input signal
//   flows to the main output directly; remaining I/Os are ignored.
// - If the plugin has more than 1 main inputs and/or 1 main outputs, and the
//   first main input / output channel group type are the same, then the first
//   main input signal flows to the first main output directly; remaining I/Os
//   are ignored.
// - If the plugin does not have main inputs (indicating that the plugin might
//   be an instrument), then all I/Os are ignored.
// - If the plugin I/O meets none of those conditions above, then all I/Os are
//   ignored.
// TODO: Make signal flows customizable if needed
// `PluginHostBypass` consists of:
//    + -----------------------------------------------------------+
//    |             +--------+                                     |
//    |  +=========>| Plugin +==========+                          |
//    |  ||         +--------+         ||    +-----------------+   |
//    |  ||                            +====>|                 |   |
// ===+==+|                                  | Switcher w/ PDC +===+===>
//    |  ||                            +====>|                 |   |
//    |  ||    +-------------------+   ||    +-----------------+   |
//    |  +====>| PassthroughDevice |====+                          |
//    |        +-------------------+                               |
//    + -----------------------------------------------------------+
class PluginHostBypass: public YADAW::Audio::Engine::NodeSet
{
public:
    class PassthroughDevice: public YADAW::Audio::Device::IAudioDevice
    {
        friend class PluginHostBypass;
    public:
        PassthroughDevice(YADAW::Audio::Device::IAudioDevice& plugin);
        ~PassthroughDevice() override;
    public:
        std::uint32_t audioInputGroupCount() const override;
        std::uint32_t audioOutputGroupCount() const override;
        OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
        OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
        std::uint32_t latencyInSamples() const override;
        void process(const Device::AudioProcessData<float>& audioProcessData) override;
    private:
        std::vector<YADAW::Audio::Util::AudioChannelGroup> channelGroups_;
        std::uint32_t inputCount_;
        std::optional<std::pair<std::uint32_t, std::uint32_t>> routePair_;
    };
    class BypassSwitcher: public YADAW::Audio::Device::IAudioDevice
    {
        friend class PluginHostBypass;
    private:
        BypassSwitcher(
            YADAW::Audio::Device::IAudioDevice& plugin
        );
        ~BypassSwitcher() override;
    public:
        bool initialize(double sampleRate, std::uint32_t maxSampleCount);
        void uninitialize();
    public:
        std::uint32_t audioInputGroupCount() const override;
        std::uint32_t audioOutputGroupCount() const override;
        OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
        OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
        std::uint32_t latencyInSamples() const override;
        void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;
    private:
        YADAW::Audio::Device::IAudioDevice* plugin_;
        std::vector<std::uint32_t> timePoints_[2];
        std::vector<bool> values_[2];
        std::uint32_t bufferSize_ = 0U;
    };
private:
    template<typename CreatePDCFunc>
    requires std::invocable<
        CreatePDCFunc,
        YADAW::Audio::Engine::AudioDeviceGraphBase&,
        YADAW::Audio::Engine::AudioDeviceProcess
    > && std::same_as<
        std::invoke_result_t<
            CreatePDCFunc,
            YADAW::Audio::Engine::AudioDeviceGraphBase&,
            YADAW::Audio::Engine::AudioDeviceProcess
        >,
        std::optional<YADAW::Audio::Engine::PDC>
    >
    PluginHostBypass(
        YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
        YADAW::Audio::Engine::AudioDeviceProcess process,
        CreatePDCFunc&& func
    ):
        YADAW::Audio::Engine::NodeSet(graph),
        plugin_(
            process.device()->audioInputGroupCount() > 1?
            decltype(plugin_)(
                std::forward<CreatePDCFunc>(func)(graph, process)
            ):
            decltype(plugin_)(
                graph.addNode(process)
            )
        ),
        passthroughDevice_(*process.device()),
        hostBypass_(
            passthroughDevice_.audioInputGroupCount() > 1?
            decltype(hostBypass_)(
                std::forward<CreatePDCFunc>(func)(
                    graph,
                    YADAW::Audio::Engine::AudioDeviceProcess(passthroughDevice_)
                )
            ):
            decltype(hostBypass_)(graph.addNode(
                YADAW::Audio::Engine::AudioDeviceProcess(passthroughDevice_)
            ))
        ),
        bypassSwitcher_(*process.device()),
        switcherPDC_(
            std::forward<CreatePDCFunc>(func)(
                graph,
                YADAW::Audio::Engine::AudioDeviceProcess(bypassSwitcher_)
            )
        )
    {}
public:
    template<YADAW::Audio::Engine::IsExtension... Extensions>
    requires requires(
        YADAW::Audio::Engine::AudioDeviceGraph<Extensions...>& graph,
        YADAW::Audio::Engine::AudioDeviceProcess process)
    {
        YADAW::Audio::Engine::PDC::createIfNeeded(graph, process);
    }
    static std::optional<PluginHostBypass> createIfNeeded(
        YADAW::Audio::Engine::AudioDeviceGraph<Extensions...>& graph,
        YADAW::Audio::Engine::AudioDeviceProcess process
    )
    {
        using GraphRef = decltype(graph);
        // TODO
        if(true)
        {
            return std::optional<PluginHostBypass>(
                std::in_place_t {},
                graph,
                [](
                    YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
                    YADAW::Audio::Engine::AudioDeviceProcess process
                )
                {
                    return YADAW::Audio::Engine::PDC::createIfNeeded(
                        static_cast<GraphRef>(graph), process
                    );
                },
                process
            );
        }
        return std::nullopt;
    }
public:
    ade::NodeHandle pluginNode() const;
public:
    std::uint32_t inputCount() const override;
    std::uint32_t outputCount() const override;
    std::optional<Position> inputAt(std::uint32_t index) const override;
    std::optional<Position> outputAt(std::uint32_t index) const override;
    YADAW::Util::PMRUniquePtr<void> dismiss() override;
    bool dismissed() const override;
    std::uint32_t innerNodeSetCount() const override;
    OptionalRef<NodeSet> innerNodeSetAt(std::uint32_t index) override;
    OptionalRef<const NodeSet> innerNodeSetAt(std::uint32_t index) const override;
private:
    std::variant<ade::NodeHandle, std::optional<YADAW::Audio::Engine::PDC>> plugin_;
    PassthroughDevice passthroughDevice_;
    std::variant<ade::NodeHandle, std::optional<YADAW::Audio::Engine::PDC>> hostBypass_;
    BypassSwitcher bypassSwitcher_;
    std::optional<YADAW::Audio::Engine::PDC> switcherPDC_;
};
}

#endif // YADAW_SRC_AUDIO_MIXER_PLUGINHOSTBYPASS
