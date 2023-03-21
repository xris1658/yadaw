#ifndef YADAW_SRC_AUDIO_HOST_VST3ATTRIBUTELIST
#define YADAW_SRC_AUDIO_HOST_VST3ATTRIBUTELIST

#include <pluginterfaces/vst/ivstattributes.h>

#include <atomic>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace YADAW::Audio::Host
{
using namespace Steinberg;
using namespace Steinberg::Vst;
class VST3AttributeList final: public IAttributeList
{
    using Variant = std::variant<int64, double, std::u16string, std::vector<char>>;
private:
    VST3AttributeList();
public:
    static VST3AttributeList* createAttributeList();
    VST3AttributeList(const VST3AttributeList&) = delete;
    VST3AttributeList(VST3AttributeList&&) = delete;
    VST3AttributeList& operator=(const VST3AttributeList&) = delete;
    VST3AttributeList& operator=(VST3AttributeList&&) = delete;
    ~VST3AttributeList() noexcept = default;
public:
    uint32 PLUGIN_API addRef() override;
    uint32 PLUGIN_API release() override;
    tresult PLUGIN_API queryInterface(const TUID _iid, void** obj) override;
public:
    tresult PLUGIN_API setInt(AttrID id, int64 value) override;
    tresult PLUGIN_API getInt(AttrID id, int64& value) override;
    tresult PLUGIN_API setFloat(AttrID id, double value) override;
    tresult PLUGIN_API getFloat(AttrID id, double& value) override;
    tresult PLUGIN_API setString(AttrID id, const TChar* string) override;
    tresult PLUGIN_API getString(AttrID id, TChar* string, uint32 sizeInBytes) override;
    tresult PLUGIN_API setBinary(AttrID id, const void* data, uint32 sizeInBytes) override;
    tresult PLUGIN_API getBinary(AttrID id, const void*& data, uint32& sizeInBytes) override;
private:
    std::atomic<std::uint32_t> refCount_ = 1;
    std::map<std::string, Variant> map_;
};
}

#endif //YADAW_SRC_AUDIO_HOST_VST3ATTRIBUTELIST
