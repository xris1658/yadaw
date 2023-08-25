#ifndef YADAW_TEST_VST3TEST_VST3DOUBLEBUFFER
#define YADAW_TEST_VST3TEST_VST3DOUBLEBUFFER

#include "audio/host/VST3EventList.hpp"

#include <pluginterfaces/vst/ivstaudioprocessor.h>

#include <atomic>

class VST3DoubleBuffer
{
    using VST3EventList = YADAW::Audio::Host::VST3EventList;
    using ProcessData = Steinberg::Vst::ProcessData;
public:
    std::pair<VST3EventList&, VST3EventList&> hostSideEventList();
    std::pair<VST3EventList&, VST3EventList&> pluginSideEventList();
    void switchBuffer();
private:
    std::atomic<int> hostSideBufferIndex_ {1};
    VST3EventList inputs_[2];
    VST3EventList outputs_[2];
};

#endif // YADAW_TEST_VST3TEST_VST3DOUBLEBUFFER
