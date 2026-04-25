#include "MiniMix.hpp"

namespace YADAW::Audio::Mixer
{
MiniMix::MiniMix(YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
    bool sharedInNode,
    YADAW::Audio::Base::ChannelGroupType channelGroupType, std::uint32_t channelCountInGroup):
    graph_(&graph)
{
    if(sharedInNode)
    {
        //
    }
    else
    {
        // inNodes_ = decltype(inNodes_)<0>();
    }
    summing_.first = std::make_unique<YADAW::Audio::Util::Summing>(
        0, channelGroupType, channelCountInGroup
    );
    summing_.second = graph_->addNode(
        YADAW::Audio::Engine::AudioDeviceProcess(*summing_.first)
    );
}

std::uint32_t MiniMix::chainCount() const
{
    return inserts_.size();
}
}
