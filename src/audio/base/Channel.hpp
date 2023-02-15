#ifndef YADAW_AUDIO_BASE_CHANNEL
#define YADAW_AUDIO_BASE_CHANNEL

namespace YADAW::Audio::Base
{
enum class ChannelType
{
    Custom = -2,
    Invalid,
    Center,
    Left,
    Right
};

enum class ChannelGroupType
{
    Custom = -1,
    NoChannel,
    Mono,
    Stereo
};

constexpr int InvalidChannelCount = -1;
}

#endif //YADAW_AUDIO_BASE_CHANNEL
