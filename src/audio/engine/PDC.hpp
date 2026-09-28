#ifndef YADAW_SRC_AUDIO_ENGINE_PDC
#define YADAW_SRC_AUDIO_ENGINE_PDC

#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/extension/UpstreamLatency.hpp"
#include "audio/engine/NodeSet.hpp"
#include "audio/util/SampleDelay.hpp"
#include "util/OptionalUtil.hpp"

#include <plf_hive.h>

#include <mutex>
#include <optional>
#include <unordered_map>

namespace YADAW::Audio::Engine
{
// `PDC` reads upstream latencies of a device node with multiple audio inputs
// and apply delays to align incoming signals.
// `PDC` consists of the device node with multi inputs and its incoming
// `SampleDelay`s:
//
// +-------------+   +--------+
// | SampleDelay +-->|        |
// +-------------+   |        |
//                   |        |
// +-------------+   |        |
// | SampleDelay +-->| Device |
// +-------------+   |        |
//                   |        |
// +-------------+   |        |
// | SampleDelay +-->|        |
// +-------------+   +--------+
//
class PDC: public YADAW::Audio::Engine::NodeSet
{
public:
    class Extension
    {
    public:
        class GraphData
        {
            friend PDC;
            std::unordered_map<ade::NodeHandle, PDC*, ade::HandleHasher<ade::Node>> pdcDeviceNodes;
            PDC* pPDC;
            std::once_flag setCallbackFlag;
        };
        template<IsExtension... Extensions>
        requires requires(YADAW::Audio::Engine::AudioDeviceGraph<Extensions...>& graph)
        {
            graph.template getExtension<PDC::Extension>();
        }
        Extension(AudioDeviceGraph<Extensions...>& graph):
            graph_(&graph),
        getGraphData_(static_cast<decltype(getGraphData_)>(
            AudioDeviceGraph<Extensions...>::template getExtensionGraphData<Extension>
        ))
        {}
    public:
        void onAddingPDC(PDC& pdc);
        void onDismissingPDC(PDC& pdc);
        void onNodeAdded(const ade::NodeHandle& nodeHandle);
        void onNodeAboutToBeRemoved(const ade::NodeHandle& nodeHandle);
        void onConnected(const ade::EdgeHandle& edgeHandle);
        void onAboutToBeDisconnected(const ade::EdgeHandle& edgeHandle);
    public:
        GraphData& getGraphData();
    private:
        YADAW::Audio::Engine::AudioDeviceGraphBase* graph_;
        GraphData&(*getGraphData_)(AudioDeviceGraphBase&);
    };
    static_assert(IsExtension<Extension>);
public:
    PDC(
        YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
        YADAW::Audio::Engine::Extension::UpstreamLatency& upstreamLatency,
        PDC::Extension& pdcExt,
        YADAW::Audio::Engine::AudioDeviceProcess process
    );
    template<IsExtension... Extensions>
    requires requires(YADAW::Audio::Engine::AudioDeviceGraph<Extensions...>& graph)
    {
        graph.template getExtension<YADAW::Audio::Engine::Extension::UpstreamLatency>();
        graph.template getExtension<PDC::Extension>();
    }
    static std::optional<PDC> createIfNeeded(
        YADAW::Audio::Engine::AudioDeviceGraph<Extensions...>& graph,
        YADAW::Audio::Engine::AudioDeviceProcess process)
    {
        if(process.device()->audioInputGroupCount() > 1)
        {
            std::optional<PDC> ret;
            return std::optional<PDC>(
                std::in_place_t {},
                graph,
                graph.template getExtension<
                    YADAW::Audio::Engine::Extension::UpstreamLatency
                >(),
                graph.template getExtension<
                    PDC::Extension
                >(),
                process
            );
        }
        return std::nullopt;
    }
    ~PDC();
public:
    ade::NodeHandle deviceNode() const;
public:
    std::uint32_t inputCount() const override;
    std::uint32_t outputCount() const override;
    std::optional<Position> inputAt(std::uint32_t index) const override;
    std::optional<Position> outputAt(std::uint32_t index) const override;
    [[nodiscard]] YADAW::Util::PMRUniquePtr<void> dismiss() override;
    bool dismissed() const override;
    void startProcessing();
    void stopProcessing();
    OptionalRef<YADAW::Audio::Util::SampleDelay> pdcAt(std::uint32_t index);
    std::optional<ade::NodeHandle> pdcNodeAt(std::uint32_t index);
private:
    void onUpstreamLatencyChanged();
    void onDeviceIOChanged();
private:
    YADAW::Audio::Engine::Extension::UpstreamLatency* upstreamLatency_;
    PDC::Extension* pdcExt_;
    ade::NodeHandle deviceNode_;
    static plf::hive<YADAW::Audio::Util::SampleDelay> pdcPool_;
    std::vector<YADAW::Audio::Util::SampleDelay*> pdcs_;
    std::vector<ade::NodeHandle> pdcNodes_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_PDC
