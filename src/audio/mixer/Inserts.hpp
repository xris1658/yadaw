#ifndef YADAW_SRC_AUDIO_MIXER_INSERTS
#define YADAW_SRC_AUDIO_MIXER_INSERTS

#include "audio/engine/AudioDeviceGraphBase.hpp"

#include "audio/mixer/Common.hpp"
#include "util/BatchUpdater.hpp"
#include "util/PolymorphicDeleter.hpp"
#include "util/StatefulCoroutine.hpp"

#include <concepts>
#include <cstdint>

namespace YADAW::Audio::Mixer
{

template<typename Func>
concept IsDetachContextCallback = std::invocable<Func, Context&&>;

class Inserts
{
public:
    using InsertAddedCallback = void(Inserts&, std::uint32_t);
    using InsertAboutToBeRemovedCallback = void(Inserts&, std::uint32_t, std::uint32_t);
    using InsertRemovedCallback = void(Inserts&, std::uint32_t, std::uint32_t);
    using ConnectionUpdatedCallback = void(const Inserts&);
public:
    Inserts(YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
        IDGen& auxInputIDGen, IDGen& auxOutputIDGen,
        ade::NodeHandle inNode, ade::NodeHandle outNode,
        std::uint32_t inChannelGroupIndex, std::uint32_t outChannelGroupIndex,
        YADAW::Util::BatchUpdater* batchUpdater = nullptr);
    ~Inserts();
public:
    YADAW::Audio::Engine::AudioDeviceGraphBase& graph() const;
    std::uint32_t insertCount() const noexcept;
    bool empty() const noexcept;
    std::optional<ade::NodeHandle> insertNodeAt(std::uint32_t index) const;
    OptionalRef<const Context> insertContextAt(std::uint32_t index) const;
    OptionalRef<Context> insertContextAt(std::uint32_t index);
    std::optional<std::uint32_t> insertLatencyAt(std::uint32_t index) const;
    std::optional<bool> insertBypassed(std::uint32_t index) const;
    std::optional<std::uint32_t> insertInputChannelGroupIndexAt(std::uint32_t index) const;
    std::optional<std::uint32_t> insertOutputChannelGroupIndexAt(std::uint32_t index) const;
    std::optional<IDGen::ID> insertAuxInputID(
        std::uint32_t insertIndex, std::uint32_t channelGroupIndex) const;
    std::optional<IDGen::ID> insertAuxOutputID(
        std::uint32_t insertIndex, std::uint32_t channelGroupIndex) const;
    const ade::NodeHandle& inNode() const;
    const ade::NodeHandle& outNode() const;
    std::uint32_t inChannelGroupIndex() const;
    std::uint32_t outChannelGroupIndex() const;
    bool setInNode(const ade::NodeHandle& inNode, std::uint32_t inChannelGroupIndex);
    bool setOutNode(const ade::NodeHandle& outNode, std::uint32_t outChannelGroupIndex);
public:
    bool insert(const ade::NodeHandle& nodeHandle, Context&& context, std::uint32_t position);
    bool append(const ade::NodeHandle& nodeHandle, Context&& context);
    template<IsDetachContextCallback Callback>
    bool detachContexts(Callback&& callback, std::uint32_t position, std::uint32_t detachCount = 1)
    {
        if(position < insertCount() && detachCount > 0 && position + detachCount <= insertCount())
        {
            FOR_RANGE(i, position, position + detachCount)
            {
                std::forward<Callback>(callback)(std::move(contexts_[i]));
            }
            return true;
        }
        return false;
    }
    bool remove(std::uint32_t position, std::uint32_t removeCount = 1);
    enum CoRemoveState: YADAW::Util::StateID
    {
        Initial = YADAW::Util::InitialState,
        AboutToBeRemoved,
        Finished = YADAW::Util::FinishedState
    };
    YADAW::Util::StatefulCoroutine<bool> coRemove(std::uint32_t position, std::uint32_t removeCount = 1);
    void clear();
    void setBypassed(std::uint32_t position, bool bypassed);
    bool move(std::uint32_t position, std::uint32_t count, Inserts& rhs, std::uint32_t destPosition);
public:
    OptionalRef<YADAW::Util::BatchUpdater> batchUpdater();
    void setBatchUpdater(YADAW::Util::BatchUpdater& batchUpdater);
    void resetBatchUpdater();
public:
    YADAW::Util::PMRUniquePtr<void>& getInsertCallbackUserData() const;
    void setInsertCallbackUserData(YADAW::Util::PMRUniquePtr<void>&& userData);
    void setInsertAddedCallback(InsertAddedCallback* callback);
    void setInsertAboutToBeRemovedCallback(InsertAboutToBeRemovedCallback* callback);
    void setInsertRemovedCallback(InsertRemovedCallback* callback);
    void setConnectionUpdatedCallback(ConnectionUpdatedCallback* callback);
private:
    YADAW::Audio::Engine::AudioDeviceGraphBase& graph_;
    ade::NodeHandle inNode_;
    ade::NodeHandle outNode_;
    std::uint32_t inChannelGroupIndex_;
    std::uint32_t outChannelGroupIndex_;
    std::vector<ade::NodeHandle> nodes_;
    std::vector<Context> contexts_;
    std::vector<bool> bypassed_;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> channelGroupIndices_;
    std::vector<std::vector<IDGen::ID>> auxInputIDs_;
    std::vector<std::vector<IDGen::ID>> auxOutputIDs_;
    IDGen* auxInputIDGen_ = nullptr;
    IDGen* auxOutputIDGen_ = nullptr;
    ConnectionUpdatedCallback* connectionUpdatedCallback_;
    YADAW::Util::BatchUpdater* batchUpdater_;
    mutable YADAW::Util::PMRUniquePtr<void> insertCallbackUserData_;
    InsertAddedCallback* insertAddedCallback_;
    InsertAboutToBeRemovedCallback* insertAboutToBeRemovedCallback_;
    InsertRemovedCallback* insertRemovedCallback_;
};
}

#endif // YADAW_SRC_AUDIO_MIXER_INSERTS
