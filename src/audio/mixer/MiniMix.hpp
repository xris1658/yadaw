#ifndef YADAW_SRC_AUDIO_MIXER_MINIMIX
#define YADAW_SRC_AUDIO_MIXER_MINIMIX

#include "audio/engine/AudioDeviceGraphBase.hpp"
#include "audio/mixer/Common.hpp"
#include "audio/mixer/Inserts.hpp"
#include "util/OptionalUtil.hpp"

#include <vector>

namespace YADAW::Audio::Mixer
{
class MiniMix
{
public:
    struct SharedInNodeInit
    {
        ade::NodeHandle node;
        std::uint32_t index;
        Context context;
    };
    MiniMix(
        YADAW::Audio::Engine::AudioDeviceGraphBase& graph, bool sharedInNode,
        YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCountInGroup = 0
    );
public:
    std::uint32_t chainCount() const;
    bool useSharedInNode() const;
    ade::NodeHandle inNode(std::uint32_t index = 0) const;
    OptionalRef<const YADAW::Audio::Mixer::Inserts> insertsAt(std::uint32_t index) const;
    OptionalRef<      YADAW::Audio::Mixer::Inserts> insertsAt(std::uint32_t index);
    OptionalRef<const YADAW::Audio::Util::Mute> muteAt(std::uint32_t index) const;
    OptionalRef<      YADAW::Audio::Util::Mute> muteAt(std::uint32_t index);
    OptionalRef<const VolumeFader> faderAt(std::uint32_t index) const;
    OptionalRef<      VolumeFader> faderAt(std::uint32_t index);
public:
    void insertChain(std::uint32_t insertIndex, DeviceAndNode<> inDeviceAndNode);
    void appendChain(DeviceAndNode<>);
    void removeChain(std::uint32_t first, std::uint32_t last);
    void clearChain();
private:
    YADAW::Audio::Engine::AudioDeviceGraphBase* graph_;
    std::variant<
        std::vector<Context>,
        std::pair<Context, std::uint32_t>
    > inNodes_;
    std::vector<std::unique_ptr<Inserts>> inserts_;
    std::vector<MuteAndNode> mutes_;
    std::vector<FaderAndNode> faders_;
    SummingAndNode summing_;
};
}

#endif // YADAW_SRC_AUDIO_MIXER_MINIMIX
