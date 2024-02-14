#ifndef YADAW_SRC_AUDIO_HOST_VST3HOST
#define YADAW_SRC_AUDIO_HOST_VST3HOST

#include <pluginterfaces/vst/ivsthostapplication.h>
#include <pluginterfaces/vst/ivstpluginterfacesupport.h>
#include <pluginterfaces/vst/ivstprocesscontext.h>

namespace YADAW::Audio::Host
{
using namespace Steinberg;
using namespace Steinberg::Vst;

class VST3Host final: public IHostApplication
{
private:
    // A separate class aggregated with the `VST3Host` is used to implement
    // `IPlugInterfaceSupport` so that
    // `static_cast<SpecifiedInterface*>(&VST3Host::instance())` is not needed
    // in `IPluginBase::initialize`.
    class PlugInterfaceSupport final: public IPlugInterfaceSupport
    {
    public:
        PlugInterfaceSupport() = default;
        PlugInterfaceSupport(const PlugInterfaceSupport&) = delete;
        ~PlugInterfaceSupport() = default;
    public:
        uint32 PLUGIN_API addRef() override;
        uint32 PLUGIN_API release() override;
        tresult PLUGIN_API queryInterface(const char* _iid, void** obj) override;
    public: // IPlugInterfaceSupport
        tresult isPlugInterfaceSupported(const char* _iid) override;
    };
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
public: // IHostApplication interfaces
    tresult PLUGIN_API getName(String128 name) override;
    tresult PLUGIN_API createInstance(TUID cid, TUID iid_, void** obj) override;
private:
    PlugInterfaceSupport plugInterfaceSupport_;
};
}

#endif // YADAW_SRC_AUDIO_HOST_VST3HOST
