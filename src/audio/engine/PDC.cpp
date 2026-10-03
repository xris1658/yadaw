#include "PDC.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Engine
{
plf::hive<YADAW::Audio::Util::SampleDelay> PDC::pdcPool_;

void PDC::Extension::onAddingPDC(PDC& pdc)
{
    getGraphData_(*graph_).pPDC = &pdc;
}

void PDC::Extension::onDismissingPDC(PDC& pdc)
{
    auto& graphData = getGraphData_(*graph_);
    graphData.pdcDeviceNodes.erase(pdc.deviceNode());
    getGraphData_(*graph_).pPDC = nullptr;
}

void PDC::Extension::onNodeAdded(const ade::NodeHandle& nodeHandle)
{
    if(auto& graphData = getGraphData_(*graph_);
        graphData.pPDC)
    {
        graphData.pdcDeviceNodes.emplace(nodeHandle, graphData.pPDC);
        graphData.pPDC = nullptr;
    }
}

void PDC::Extension::onNodeAboutToBeRemoved(const ade::NodeHandle& nodeHandle)
{
    auto& graphData = getGraphData_(*graph_);
    graphData.pdcDeviceNodes.erase(nodeHandle);
}

void PDC::Extension::onConnected(const ade::EdgeHandle& edgeHandle)
{}

void PDC::Extension::onAboutToBeDisconnected(const ade::EdgeHandle& edgeHandle)
{}

PDC::Extension::GraphData& PDC::Extension::getGraphData()
{
    return (*getGraphData_)(*graph_);
}

PDC::PDC(
    YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
    YADAW::Audio::Engine::Extension::UpstreamLatency& upstreamLatency,
    PDC::Extension& pdcExt,
    YADAW::Audio::Engine::AudioDeviceProcess process
):
    NodeSet(graph),
    upstreamLatency_(&upstreamLatency),
    pdcExt_(&pdcExt)
{
    pdcExt_->onAddingPDC(*this);
    deviceNode_ = graph_->addNode(process);
    auto device = process.device();
    auto count = device->audioInputGroupCount();
    pdcs_.reserve(count);
    pdcNodes_.reserve(count);
    FOR_RANGE0(i, count)
    {
        auto it = pdcPool_.emplace(
            0,
            device->audioInputGroupAt(i)->get()
        );
        auto& pdc = *pdcs_.emplace_back(&*it);
        auto& pdcNode = pdcNodes_.emplace_back(
            graph_->addNode(
                AudioDeviceProcess(pdc)
            )
        );
        graph_->connect(
            pdcNode, deviceNode_, 0, i
        );
    }
    std::call_once(
        pdcExt.getGraphData().setCallbackFlag,
        [this]()
        {
            upstreamLatency_->setLatencyOfNodeUpdatedCallback(
                [this](
                    const YADAW::Audio::Engine::Extension::UpstreamLatency& sender,
                    const ade::NodeHandle& nodeHandle)
                {
                    auto& pdcDeviceNodes = pdcExt_->getGraphData().pdcDeviceNodes;
                    auto it = pdcDeviceNodes.find(nodeHandle);
                    if(it != pdcDeviceNodes.end() && !it->second->dismissed())
                    {
                        it->second->onUpstreamLatencyChanged();
                    }
                }
            );
        }
    );
}

PDC::~PDC()
{
    for(const auto& node: pdcNodes_)
    {
        graph_->removeNode(node);
    }
    if(!dismissed())
    {
        for(auto pdc: pdcs_)
        {
            PDC::pdcPool_.erase(PDC::pdcPool_.get_iterator(pdc));
        }
    }
}

ade::NodeHandle PDC::deviceNode() const
{
    return deviceNode_;
}

std::uint32_t PDC::inputCount() const
{
    return graph_->getNodeData(deviceNode_).process.device()->audioInputGroupCount();
}

std::uint32_t PDC::outputCount() const
{
    return graph_->getNodeData(deviceNode_).process.device()->audioOutputGroupCount();
}

std::optional<NodeSet::InputPosition> PDC::inputAt(std::uint32_t index) const
{
    if(index < inputCount())
    {
        return std::vector<NodeSet::NodePosition>{NodeSet::NodePosition{
            .node = pdcNodes_[index],
            .index = 0
        }};
    }
    return std::nullopt;
}

std::optional<NodeSet::OutputPosition> PDC::outputAt(std::uint32_t index) const
{
    if(index < outputCount())
    {
        return {NodeSet::NodePosition{
            .node = deviceNode_,
            .index = index
        }};
    }
    return std::nullopt;
}

YADAW::Util::PMRUniquePtr<void> PDC::dismiss()
{
    struct DismissedData
    {
        std::vector<YADAW::Audio::Util::SampleDelay*> pdcs;
        DismissedData(PDC& pdc):
            pdcs(std::move(pdc.pdcs_))
        {}
        ~DismissedData()
        {
            for(auto pdc: pdcs)
            {
                PDC::pdcPool_.erase(PDC::pdcPool_.get_iterator(pdc));
            }
        }
    };
    return YADAW::Util::createPMRUniquePtr(
        std::make_unique<DismissedData>(
            *this
        )
    );
}

bool PDC::dismissed() const
{
    return pdcs_.empty();
}

void PDC::startProcessing()
{
    for(auto pdc: pdcs_)
    {
        pdc->startProcessing();
    }
}

void PDC::stopProcessing()
{
    for(auto pdc: pdcs_)
    {
        pdc->stopProcessing();
    }
}

OptionalRef<YADAW::Audio::Util::SampleDelay> PDC::pdcAt(std::uint32_t index)
{
    if(index < pdcs_.size())
    {
        return *pdcs_[index];
    }
    return std::nullopt;
}

std::optional<ade::NodeHandle> PDC::pdcNodeAt(std::uint32_t index)
{
    if(index < pdcNodes_.size())
    {
        return pdcNodes_[index];
    }
    return std::nullopt;
}

void PDC::onUpstreamLatencyChanged()
{
    auto maxUpstreamLatency = upstreamLatency_->getMaxUpstreamLatency(deviceNode_);
    bool pdcProcessing = pdcs_.front()->isProcessing();
    FOR_RANGE0(i, pdcs_.size())
    {
        auto delay = maxUpstreamLatency - *upstreamLatency_->getUpstreamLatency(deviceNode_, i);
        auto& pdc = *pdcs_[i];
        if(pdcProcessing)
        {
            pdc.stopProcessing();
        }
        pdc.setDelay(delay);
        if(pdcProcessing)
        {
            pdc.startProcessing();
        }
    }
}

void PDC::onDeviceIOChanged()
{
    // TODO
}
}
