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
};
}

#endif //YADAW_SRC_AUDIO_BACKEND_ALSADEVICESELECTOR
