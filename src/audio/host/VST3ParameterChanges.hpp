#ifndef YADAW_SRC_AUDIO_HOST_VST3PARAMETERCHANGES
#define YADAW_SRC_AUDIO_HOST_VST3PARAMETERCHANGES

#include "audio/host/VST3ParameterValueQueue.hpp"

#include <vector>

namespace YADAW::Audio::Host
{
using namespace Steinberg;

class VST3ParameterChanges: public Vst::IParameterChanges
{
public:
    VST3ParameterChanges(std::size_t reserveParameterCount = 0);
    ~VST3ParameterChanges() noexcept;
public:
    tresult queryInterface(const int8* _iid, void** obj) override;
    uint32 addRef() override;
    uint32 release() override;
public:
    int32 getParameterCount() override;
    Vst::IParamValueQueue* getParameterData(int32 index) override;
    Vst::IParamValueQueue* addParameterData(const Vst::ParamID& id, int32& index) override;
public:
    void reserve(std::size_t reserveParameterCount);
    void reset();
    void clearPointsInQueue();
private:
    std::vector<YADAW::Audio::Host::VST3ParameterValueQueue> parameterValueQueues_;
    std::int64_t timestampInNanosecond_;
};
}

#endif //YADAW_SRC_AUDIO_HOST_VST3PARAMETERCHANGES
