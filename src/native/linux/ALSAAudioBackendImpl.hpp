#ifndef YADAW_ALSAAUDIOBACKENDIMPL
#define YADAW_ALSAAUDIOBACKENDIMPL

#if(__linux__)

#include "audio/backend/ALSAAudioBackend.hpp"
#include "audio/backend/ALSADeviceSelector.hpp"

#include <alsa/asoundlib.h>

#include <string>
#include <vector>

namespace YADAW::Audio::Backend
{
class ALSAAudioBackend::Impl
{
public:
    static std::uint32_t audioInputCount();
    static std::uint32_t audioOutputCount();
    static std::optional<ALSADeviceSelector> audioInputDeviceAt(std::uint32_t index);
    static std::optional<ALSADeviceSelector> audioOutputDeviceAt(std::uint32_t index);
private:
    snd_pcm_t* pcm_ = nullptr;
    snd_pcm_hw_params_t* hwParams_ = nullptr;
};
}

#endif

#endif //YADAW_ALSAAUDIOBACKENDIMPL
