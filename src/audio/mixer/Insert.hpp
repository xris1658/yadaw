#ifndef YADAW_SRC_AUDIO_MIXER_INSERT
#define YADAW_SRC_AUDIO_MIXER_INSERT

#include "audio/engine/AudioDeviceGraphBase.hpp"

namespace YADAW::Audio::Mixer
{
class Insert
{
public:
    Insert(YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
        ade::NodeHandle inNode, ade::NodeHandle outNode,
        std::uint32_t inChannel, std::uint32_t outChannel);
public:
    std::uint32_t insertCount() const noexcept;
    bool empty() const noexcept;
    std::optional<ade::NodeHandle> insertAt(std::uint32_t index) const;
public:
    bool insert(const ade::NodeHandle& nodeHandle, std::uint32_t position);
    bool append(const ade::NodeHandle& nodeHandle);
    bool remove(std::uint32_t position, std::uint32_t removeCount = 1);
    void clear();
private:
    YADAW::Audio::Engine::AudioDeviceGraphBase& graph_;
    ade::NodeHandle inNode_;
    ade::NodeHandle outNode_;
    std::uint32_t inChannel_;
    std::uint32_t outChannel_;
    std::vector<ade::NodeHandle> nodes_;
};
}

#endif //YADAW_SRC_AUDIO_MIXER_INSERT
