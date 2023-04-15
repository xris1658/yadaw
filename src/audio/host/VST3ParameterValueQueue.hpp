#ifndef YADAW_SRC_AUDIO_HOST_VST3PARAMETERVALUEQUEUE
#define YADAW_SRC_AUDIO_HOST_VST3PARAMETERVALUEQUEUE

#include "util/FixedSizeCircularDeque.hpp"

#include <pluginterfaces/base/ftypes.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>

namespace YADAW::Audio::Host
{
using namespace Steinberg;
class VST3ParameterValueQueue: public Vst::IParamValueQueue
{
    struct Point
    {
        int32 sampleOffset_;
        Vst::ParamValue paramValue_;
        Point(const int32& sampleOffset, double paramValue):
            sampleOffset_(sampleOffset), paramValue_(paramValue)
        {}
        ~Point() = default;
    };
public:
    VST3ParameterValueQueue(Vst::ParamID paramId);
    ~VST3ParameterValueQueue() = default;
public: // FUnknown interfaces
    tresult queryInterface(const TUID _iid, void** obj) override;
    uint32 addRef() override;
    uint32 release() override;
public:
    Vst::ParamID getParameterId() override;
    int32 getPointCount() override;
    tresult getPoint(int32 index, int32& sampleOffset, Vst::ParamValue& value) override;
    // This function DOES NOT sort: the output index might be used later, but sorting the container
    // might modify the index, resulting in issues with `getPoint`.
    // It's the producer's responsibility to guarantee that `addPoint` call is sorted.
    tresult addPoint(int32 sampleOffset, Vst::ParamValue value, int32& index) override;
public:
    void clear();
private:
    Vst::ParamID paramId_;
    YADAW::Util::FixedSizeCircularDeque<Point, 2048> container_;
};
}

#endif //YADAW_SRC_AUDIO_HOST_VST3PARAMETERVALUEQUEUE
