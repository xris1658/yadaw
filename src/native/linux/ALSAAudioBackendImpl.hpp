#ifndef YADAW_ALSAAUDIOBACKENDIMPL
#define YADAW_ALSAAUDIOBACKENDIMPL

#if(__linux__)

#include "audio/backend/ALSAAudioBackend.hpp"
#include "native/linux/ALSADeviceEnumerator.hpp"

#include <alsa/asoundlib.h>

#include <string>
#include <vector>

namespace YADAW::Audio::Backend
{
class ALSAAudioBackend::Impl
{
    using PCMDeviceSelector = YADAW::Native::ALSADeviceEnumerator::DeviceSelector;
public:
    static std::uint32_t audioInputCount();
    static std::uint32_t audioOutputCount();
    static std::optional<PCMDeviceSelector> audioInputDeviceAt(std::uint32_t index);
    static std::optional<PCMDeviceSelector> audioOutputDeviceAt(std::uint32_t index);
private:
    snd_pcm_t* pcm_ = nullptr;
    snd_pcm_hw_params_t* hwParams_ = nullptr;
};
}

#endif

#endif //YADAW_ALSAAUDIOBACKENDIMPL
