#ifndef YADAW_SRC_AUDIO_HOST_VST3MESSAGE
#define YADAW_SRC_AUDIO_HOST_VST3MESSAGE

#include "audio/host/VST3AttributeList.hpp"

#include <pluginterfaces/vst/ivstmessage.h>

#include <atomic>
#include <vector>

namespace YADAW::Audio::Host
{
using namespace Steinberg;
using namespace Steinberg::Vst;
class VST3Message final: public IMessage
{
private:
    VST3Message();
public:
    static VST3Message* createMessage();
    VST3Message(const VST3Message&) = delete;
    VST3Message(VST3Message&&) = delete;
    VST3Message& operator=(const VST3Message&) = delete;
    VST3Message& operator=(VST3Message&&) = delete;
    ~VST3Message() noexcept;
public:
    uint32 PLUGIN_API addRef() override;
    uint32 PLUGIN_API release() override;
    tresult PLUGIN_API queryInterface(const TUID _iid, void** obj) override;
public:
    FIDString PLUGIN_API getMessageID() override;
    void PLUGIN_API setMessageID(FIDString id) override;
    IAttributeList* PLUGIN_API getAttributes() override;
private:
    std::atomic<std::uint32_t> refCount_ = 1;
    std::string id_ = "";
    VST3AttributeList* attributeList_ = nullptr;
};
}

#endif // YADAW_SRC_AUDIO_HOST_VST3MESSAGE
