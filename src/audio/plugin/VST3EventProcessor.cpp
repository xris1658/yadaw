#include "VST3EventProcessor.hpp"

namespace YADAW::Audio::Plugin
{
using namespace Steinberg;
using namespace Steinberg::Vst;
VST3EventProcessor::VST3EventProcessor(Steinberg::Vst::IComponent& component) :
	component_(&component)
{
    component_->addRef();
	auto inputCount = eventInputBusCount();
	auto outputCount = eventOutputBusCount();
	eventInputBusInfoGroup_.resize(inputCount);
	eventOutputBusInfoGroup_.resize(outputCount);
	for (int i = 0; i < inputCount; ++i)
	{
		component_->getBusInfo(MediaTypes::kEvent, BusDirections::kInput,
			i, eventInputBusInfoGroup_[i]);
        input_.emplace_back(eventInputBusInfoGroup_[i]);
	}
    for(int i = 0; i < outputCount; ++i)
    {
        component_->getBusInfo(MediaTypes::kEvent, BusDirections::kOutput,
            i, eventInputBusInfoGroup_[i]);
        output_.emplace_back(eventOutputBusInfoGroup_[i]);
    }
}

VST3EventProcessor::~VST3EventProcessor()
{
    component_->release();
}

std::uint32_t VST3EventProcessor::eventInputBusCount() const
{
	return component_->getBusCount(MediaTypes::kEvent, BusDirections::kInput);
}

std::uint32_t VST3EventProcessor::eventOutputBusCount() const
{
	return component_->getBusCount(MediaTypes::kEvent, BusDirections::kOutput);
}

const IEventBusInfo* VST3EventProcessor::eventInputBusAt(std::uint32_t index) const
{
    if(index < eventInputBusCount())
    {
        return &input_[index];
    }
    return nullptr;
}

const IEventBusInfo* VST3EventProcessor::eventOutputBusAt(std::uint32_t index) const
{
    if(index < eventOutputBusCount())
    {
        return &output_[index];
    }
    return nullptr;
}
}