#ifndef YADAW_TEST_VST3TEST_VST3PLUGINLATENCYCHANGEDCALLBACK
#define YADAW_TEST_VST3TEST_VST3PLUGINLATENCYCHANGEDCALLBACK

#include "audio/plugin/VST3Plugin.hpp"

class VST3PluginLatencyChangedCallback
{
public:
    VST3PluginLatencyChangedCallback(YADAW::Audio::Plugin::VST3Plugin& plugin);
public:
    void latencyChanged();
private:
    YADAW::Audio::Plugin::VST3Plugin* plugin_;
};
#endif // YADAW_TEST_VST3TEST_VST3PLUGINLATENCYCHANGEDCALLBACK
