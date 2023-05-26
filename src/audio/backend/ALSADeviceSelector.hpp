#ifndef YADAW_SRC_AUDIO_BACKEND_ALSADEVICESELECTOR
#define YADAW_SRC_AUDIO_BACKEND_ALSADEVICESELECTOR

#include <cstdint>

namespace YADAW::Audio::Backend
{
struct ALSADeviceSelector
{
    ALSADeviceSelector(int c, int d): cIndex(c), dIndex(d) {}
    std::uint32_t cIndex;
    std::uint32_t dIndex;
    inline std::uint64_t key() const
    {
        return (static_cast<std::uint64_t>(cIndex) << 32) + dIndex;
    }
    bool operator==(const ALSADeviceSelector& rhs) const
    {
        return key() == rhs.key();
    }
    bool operator!=(const ALSADeviceSelector& rhs) const
    {
        return key() != rhs.key();
    }
    bool operator<(const ALSADeviceSelector& rhs) const
    {
        return key() < rhs.key();
    }
    bool operator>(const ALSADeviceSelector& rhs) const
    {
        return key() > rhs.key();
    }
    bool operator<=(const ALSADeviceSelector& rhs) const
    {
        return key() <= rhs.key();
    }
    bool operator>=(const ALSADeviceSelector& rhs) const
    {
        return key() >= rhs.key();
    }
};
}

#endif //YADAW_SRC_AUDIO_BACKEND_ALSADEVICESELECTOR
