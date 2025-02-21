#ifndef YADAW_SRC_AUDIO_MIXER_INSERTS
#define YADAW_SRC_AUDIO_MIXER_INSERTS

#include "audio/engine/AudioDeviceGraphBase.hpp"

#include "audio/mixer/Common.hpp"

namespace YADAW::Audio::Mixer
{
class Inserts
{
public:
    using NodeAddedCallback = void(const Inserts&);
    using NodeRemovedCallback = void(const Inserts&);
    using ConnectionUpdatedCallback = void(const Inserts&);
public:
    Inserts(YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
        IDGen& auxInputIDGen, IDGen& auxOutputIDGen,
        ade::NodeHandle inNode, ade::NodeHandle outNode,
        std::uint32_t inChannelGroupIndex, std::uint32_t outChannelGroupIndex);
    ~Inserts();
public:
    YADAW::Audio::Engine::AudioDeviceGraphBase& graph() const;
    std::uint32_t insertCount() const noexcept;
    bool empty() const noexcept;
    std::optional<ade::NodeHandle> insertAt(std::uint32_t index) const;
    std::optional<QString> insertNameAt(std::uint32_t index) const;
    std::optional<std::uint32_t> insertLatencyAt(std::uint32_t index) const;
    std::optional<bool> insertBypassed(std::uint32_t index) const;
    const ade::NodeHandle& inNode() const;
    const ade::NodeHandle& outNode() const;
    std::uint32_t inChannelGroupIndex() const;
    std::uint32_t outChannelGroupIndex() const;
    bool setInNode(const ade::NodeHandle& inNode, std::uint32_t inChannelGroupIndex);
    bool setOutNode(const ade::NodeHandle& outNode, std::uint32_t outChannelGroupIndex);
public:
    bool insert(const ade::NodeHandle& nodeHandle, std::uint32_t position, const QString& name);
    bool append(const ade::NodeHandle& nodeHandle, const QString& name);
    bool remove(std::uint32_t position, std::uint32_t removeCount = 1);
    void clear();
    void setName(std::uint32_t position, const QString& name);
    void setBypassed(std::uint32_t position, bool bypassed);
    bool move(std::uint32_t position, std::uint32_t count, Inserts& rhs, std::uint32_t destPosition);
public:
    void setConnectionUpdatedCallback(ConnectionUpdatedCallback* callback);
private:
    YADAW::Audio::Engine::AudioDeviceGraphBase& graph_;
    ade::NodeHandle inNode_;
    ade::NodeHandle outNode_;
    std::uint32_t inChannelGroupIndex_;
    std::uint32_t outChannelGroupIndex_;
    std::vector<ade::NodeHandle> nodes_;
    std::vector<QString> names_;
    std::vector<bool> bypassed_;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> channelGroupIndices_;
    std::vector<std::vector<IDGen::ID>> auxInputIDs_;
    std::vector<std::vector<IDGen::ID>> auxOutputIDs_;
    IDGen* auxInputIDGen_ = nullptr;
    IDGen* auxOutputIDGen_ = nullptr;
    NodeAddedCallback* nodeAddedCallback_;
    NodeRemovedCallback* nodeRemovedCallback_;
    ConnectionUpdatedCallback* connectionUpdatedCallback_;
};
}

#endif //YADAW_SRC_AUDIO_MIXER_INSERTS
