#include "VST3EventBusInfo.hpp"

namespace YADAW::Audio::Plugin
{
VST3EventBusInfo::VST3EventBusInfo(const Steinberg::Vst::BusInfo& busInfo):
	busInfo_(&busInfo)
{
}

QString VST3EventBusInfo::name() const
{
	return QString::fromUtf16(busInfo_->name);
}

std::uint32_t VST3EventBusInfo::channelCount() const
{
	return busInfo_->channelCount;
}

bool VST3EventBusInfo::isMain() const
{
	return busInfo_->busType == Steinberg::Vst::BusTypes::kMain;
}
}