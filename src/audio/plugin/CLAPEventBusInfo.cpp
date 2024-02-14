#include "CLAPEventBusInfo.hpp"

namespace YADAW::Audio::Plugin
{
CLAPEventBusInfo::CLAPEventBusInfo(const clap_note_port_info& notePortInfo):
    notePortInfo_(&notePortInfo)
{}

CLAPEventBusInfo::~CLAPEventBusInfo()
{}

QString CLAPEventBusInfo::name() const
{
    return QString::fromUtf8(notePortInfo_->name);
}

std::uint32_t CLAPEventBusInfo::channelCount() const
{
    return 16; // FIXME
}

bool CLAPEventBusInfo::isMain() const
{
    return false; // FIXME
}
}