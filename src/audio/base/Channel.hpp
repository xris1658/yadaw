#ifndef YADAW_SRC_AUDIO_BASE_CHANNEL
#define YADAW_SRC_AUDIO_BASE_CHANNEL

namespace YADAW::Audio::Base
{
enum class ChannelType
{
    Custom = -2,
    Invalid,
    Center, FrontCenter = Center,
    Left, FrontLeft = Left,
    Right, FrontRight = Right,
    SideLeft,
    SideRight,
    RearLeft,
    RearRight,
    RearCenter,
    LFE,
};

enum class ChannelGroupType
{
    eInvalid = -2,
    eCustomGroup,
    eEmpty,
    eMono,
    eStereo,
    eLRC,
    eQuad,
    e50,
    e51,
    e61,
    e71,
    eEnd
};

constexpr int InvalidChannelCount = -1;
}

#endif // YADAW_SRC_AUDIO_BASE_CHANNEL
