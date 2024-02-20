#ifndef YADAW_SRC_AUDIO_MIXER_INSERTS
#define YADAW_SRC_AUDIO_MIXER_INSERTS

#include "audio/engine/AudioDeviceGraphBase.hpp"

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
        ade::NodeHandle inNode, ade::NodeHandle outNode,
        std::uint32_t inChannel, std::uint32_t outChannel);
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
    std::uint32_t inChannel() const;
    std::uint32_t outChannel() const;
    bool setInNode(const ade::NodeHandle& inNode, std::uint32_t inChannel);
    bool setOutNode(const ade::NodeHandle& outNode, std::uint32_t outChannel);
public:
    bool insert(const ade::NodeHandle& nodeHandle, std::uint32_t position, const QString& name);
    bool append(const ade::NodeHandle& nodeHandle, const QString& name);
    bool remove(std::uint32_t position, std::uint32_t removeCount = 1);
    void clear();
    void setName(std::uint32_t position, const QString& name);
    void setBypassed(std::uint32_t position, bool bypassed);
    bool move(std::uint32_t position, std::uint32_t count, Inserts& rhs, std::uint32_t destPosition);
public:
    void setNodeAddedCallback(std::function<NodeAddedCallback>&& callback);
    void setNodeRemovedCallback(std::function<NodeRemovedCallback>&& callback);
    void setConnectionUpdatedCallback(std::function<ConnectionUpdatedCallback>&& callback);
private:
    YADAW::Audio::Engine::AudioDeviceGraphBase& graph_;
    ade::NodeHandle inNode_;
    ade::NodeHandle outNode_;
    std::uint32_t inChannel_;
    std::uint32_t outChannel_;
    std::vector<ade::NodeHandle> nodes_;
    std::vector<ade::EdgeHandle> edges_;
    std::vector<QString> names_;
    std::vector<bool> bypassed_;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> channel_;
    std::function<NodeAddedCallback> nodeAddedCallback_;
    std::function<NodeRemovedCallback> nodeRemovedCallback_;
    std::function<ConnectionUpdatedCallback> connectionUpdatedCallback_;
};
}

#endif //YADAW_SRC_AUDIO_MIXER_INSERTS
