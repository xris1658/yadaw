#include "VST3PluginLatencyChangedCallback.hpp"

#include <cstdio>

VST3PluginLatencyChangedCallback::VST3PluginLatencyChangedCallback(
    YADAW::Audio::Plugin::VST3Plugin& plugin):
    plugin_(&plugin)
{}

void VST3PluginLatencyChangedCallback::latencyChanged()
{
    std::printf("New latency: %u\n", plugin_->latencyInSamples());
}
