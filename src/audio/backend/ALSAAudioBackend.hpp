#ifndef YADAW_ALSAAUDIOBACKEND
#define YADAW_ALSAAUDIOBACKEND

#include <alsa/asoundlib.h>
#include <alsa/pcm.h>

#include <cstdint>
#include <memory>

namespace YADAW::Audio::Backend
{
class ALSAAudioBackend
{
    class Impl;
private:
    std::unique_ptr<Impl> pImpl_;
};
}

#endif //YADAW_ALSAAUDIOBACKEND
