#include "PluginNode.hpp"

namespace YADAW::Audio::Engine
{
void PluginNode::process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    process_(audioProcessData);
}
}