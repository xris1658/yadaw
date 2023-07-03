#ifndef YADAW_TEST_CLAPTEST_CLAPPLUGINLATENCYCHANGEDCALLBACK
#define YADAW_TEST_CLAPTEST_CLAPPLUGINLATENCYCHANGEDCALLBACK

#include "audio/plugin/CLAPPlugin.hpp"

class CLAPPluginLatencyChangedCallback
{
public:
    CLAPPluginLatencyChangedCallback(YADAW::Audio::Plugin::CLAPPlugin& plugin);
public:
    void latencyChanged();
private:
    YADAW::Audio::Plugin::CLAPPlugin* plugin_;
};
#endif // YADAW_TEST_CLAPTEST_CLAPPLUGINLATENCYCHANGEDCALLBACK
