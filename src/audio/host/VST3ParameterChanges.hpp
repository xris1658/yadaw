#ifndef YADAW_SRC_AUDIO_HOST_VST3PARAMETERCHANGES
#define YADAW_SRC_AUDIO_HOST_VST3PARAMETERCHANGES

#include "audio/host/VST3ParameterValueQueue.hpp"

#include <deque>

namespace YADAW::Audio::Host
{
using namespace Steinberg;

class VST3ParameterChanges: public Vst::IParameterChanges
{
public:
    VST3ParameterChanges();
    ~VST3ParameterChanges() noexcept;
public:
    tresult queryInterface(const int8* _iid, void** obj) override;
    uint32 addRef() override;
    uint32 release() override;
public:
    int32 getParameterCount() override;
    Vst::IParamValueQueue* getParameterData(int32 index) override;
    Vst::IParamValueQueue* addParameterData(const Vst::ParamID& id, int32& index) override;
private:
    std::deque<YADAW::Audio::Host::VST3ParameterValueQueue> parameterValueQueues_;
};
}

#endif //YADAW_SRC_AUDIO_HOST_VST3PARAMETERCHANGES
