#ifndef YADAW_SRC_NATIVE_LINUX_ALSAHELPER
#define YADAW_SRC_NATIVE_LINUX_ALSAHELPER

#if __linux__

#include <alsa/asoundlib.h>

#include <memory>

#define DEFINE_ALSA_DELETE(alsa_struct_base) \
void alsa_free(alsa_struct_base##_t* ptr)    \
{                                            \
    alsa_struct_base##_free(ptr);            \
}

#define DEFINE_ALSA_CLOSE(alsa_struct_base)   \
int alsa_close(alsa_struct_base##_t* ptr)     \
{                                             \
    auto ret = alsa_struct_base##_close(ptr); \
    return ret;                               \
}

namespace Impl
{
DEFINE_ALSA_CLOSE(snd_rawmidi)
DEFINE_ALSA_CLOSE(snd_pcm)
DEFINE_ALSA_CLOSE(snd_seq)
DEFINE_ALSA_DELETE(snd_pcm_info)
DEFINE_ALSA_DELETE(snd_seq_queue_info)
DEFINE_ALSA_DELETE(snd_seq_port_info)
DEFINE_ALSA_DELETE(snd_rawmidi_status)
DEFINE_ALSA_DELETE(snd_rawmidi_params)
DEFINE_ALSA_DELETE(snd_rawmidi_info)
}

template<typename T>
class ALSADeleter
{
public:
    ALSADeleter() = default;
    ALSADeleter(const ALSADeleter<T>&) = default;
    ALSADeleter(ALSADeleter<T>&&) noexcept = default;
    ALSADeleter& operator=(const ALSADeleter<T>&) = default;
    ALSADeleter& operator=(ALSADeleter<T>&&) noexcept = default;
    ~ALSADeleter() noexcept = default;
public:
    void operator()(void* ptr) { Impl::alsa_free(static_cast<T*>(ptr)); }
};

template<typename T>
class ALSACloser
{
public:
    ALSACloser() = default;
    ALSACloser(const ALSACloser<T>&) = default;
    ALSACloser(ALSACloser<T>&&) noexcept = default;
    ALSACloser& operator=(const ALSACloser<T>&) = default;
    ALSACloser& operator=(ALSACloser<T>&&) noexcept = default;
    ~ALSACloser() noexcept = default;
public:
    void operator()(void* ptr) { Impl::alsa_close(static_cast<T*>(ptr)); }
};

template<typename T>
using ALSAClosablePointer = std::unique_ptr<T, ALSACloser<T>>;

template<typename T>
using ALSADeletablePointer = std::unique_ptr<T, ALSADeleter<T>>;

#endif

#endif // YADAW_SRC_NATIVE_LINUX_ALSAHELPER
