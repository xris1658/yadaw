#ifndef YADAW_SRC_AUDIO_HOST_VST3HOST
#define YADAW_SRC_AUDIO_HOST_VST3HOST

#include <pluginterfaces/vst/ivsthostapplication.h>
#include <pluginterfaces/vst/ivstprocesscontext.h>

namespace YADAW::Audio::Host
{
using namespace Steinberg;
using namespace Steinberg::Vst;
class VST3Host: public IHostApplication
{
private:
    VST3Host() = default;
public:
    static VST3Host& instance();
public:
    VST3Host(const VST3Host&) = delete;
    VST3Host(VST3Host&&) = delete;
    ~VST3Host() = default;
public:
    uint32 PLUGIN_API addRef() override;
    uint32 PLUGIN_API release() override;
    tresult PLUGIN_API queryInterface(const char* _iid, void** obj) override;
public:
    tresult PLUGIN_API getName(String128 name) override;
    tresult PLUGIN_API createInstance(TUID cid, TUID iid_, void** obj) override;
public:
    static Steinberg::Vst::ProcessContext& processContext();
};
}

#endif // YADAW_SRC_AUDIO_HOST_VST3HOST
