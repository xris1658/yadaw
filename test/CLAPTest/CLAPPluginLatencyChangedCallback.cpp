#include "CLAPPluginLatencyChangedCallback.hpp"

#include <cstdio>

CLAPPluginLatencyChangedCallback::CLAPPluginLatencyChangedCallback
    (YADAW::Audio::Plugin::CLAPPlugin& plugin):
    plugin_(&plugin)
{}

void CLAPPluginLatencyChangedCallback::latencyChanged()
{
    std::printf("New latency: %u\n", plugin_->latencyInSamples());
}
