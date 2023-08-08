#ifndef YADAW_SRC_AUDIO_UTIL_CHANNELGROUPREMIXER
#define YADAW_SRC_AUDIO_UTIL_CHANNELGROUPREMIXER

#include "audio/util/AudioChannelGroup.hpp"
#include "audio/device/IAudioDevice.hpp"
#include "util/Constants.hpp"

namespace YADAW::Audio::Util
{
using YADAW::Audio::Device::IAudioChannelGroup;
class ChannelGroupRemixer: public YADAW::Audio::Device::IAudioDevice
{
public:
    ChannelGroupRemixer(const IAudioChannelGroup& inputChannelGroup,
        const IAudioChannelGroup& outputChannelGroup);
    ~ChannelGroupRemixer() noexcept;
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;
public:
    float getGain(std::uint32_t from, std::uint32_t to) const;
    void setGain(std::uint32_t from, std::uint32_t to, float gain);
private:
    AudioChannelGroup inputChannelGroup_;
    AudioChannelGroup outputChannelGroup_;
    std::vector<std::vector<float>> gain_;
};

const float stereoFromMono[2][1] = {
    {1.0f},
    {1.0f}
};

const float monoFromStereo[1][2] = {
    {
        0.5f,
        0.5f
    }
};

// https://www.rfc-editor.org/rfc/rfc7845#section-5.1.1.5

const float stereoFromLRC[2][3] = {
    {
        1.0f,
        0.5f * YADAW::Util::sqrt2<float>() - 1.0f,
        0.0f
    },
    {
        0.0f,
        0.5f * YADAW::Util::sqrt2<float>() - 1.0f,
        1.0f
    }
};

const float stereoFromLRCNormalization = 2.0f / (2.0f + YADAW::Util::sqrt2<float>());

const float stereoFromLRCNormalized[2][3] = {
    {
        stereoFromLRC[0][0] * stereoFromLRCNormalization,
        stereoFromLRC[0][1] * stereoFromLRCNormalization,
        stereoFromLRC[0][2] * stereoFromLRCNormalization
    },
    {
        stereoFromLRC[1][0] * stereoFromLRCNormalization,
        stereoFromLRC[1][1] * stereoFromLRCNormalization,
        stereoFromLRC[1][2] * stereoFromLRCNormalization
    }
};

const float stereoFromQuad[2][4] = {
    {
        1.0f,
        0.0f,
        YADAW::Util::sqrt3<float>() * 0.5f,
        0.5f
    },
    {
        0.0f,
        1.0f,
        0.5f,
        YADAW::Util::sqrt3<float>() * 0.5f
    },
};

const float stereoFromQuadNormalization = 2.0f / (3.0f + YADAW::Util::sqrt3<float>());

const float stereoFromQuadNormalized[2][4] = {
    {
        stereoFromQuad[0][0] * stereoFromQuadNormalization,
        stereoFromQuad[0][1] * stereoFromQuadNormalization,
        stereoFromQuad[0][2] * stereoFromQuadNormalization,
        stereoFromQuad[0][3] * stereoFromQuadNormalization
    },
    {
        stereoFromQuad[1][0] * stereoFromQuadNormalization,
        stereoFromQuad[1][1] * stereoFromQuadNormalization,
        stereoFromQuad[1][2] * stereoFromQuadNormalization,
        stereoFromQuad[1][3] * stereoFromQuadNormalization
    }
};

const float stereoFrom50[2][5] = {
    {
        1.0f,
        0.5f * YADAW::Util::sqrt2<float>(),
        0.0f,
        0.5f * YADAW::Util::sqrt3<float>(),
        0.5f
    },
    {
        0.0f,
        0.5f * YADAW::Util::sqrt2<float>(),
        1.0f,
        0.5f,
        0.5f * YADAW::Util::sqrt3<float>()
    }
};

const float stereoFrom50Normalization = 1.0f / (3.0f + YADAW::Util::sqrt2<float>() + YADAW::Util::sqrt3<float>());

const float stereoFrom50Normalized[2][5] = {
    {
        stereoFrom50[0][0] * stereoFrom50Normalization,
        stereoFrom50[0][1] * stereoFrom50Normalization,
        stereoFrom50[0][2] * stereoFrom50Normalization,
        stereoFrom50[0][3] * stereoFrom50Normalization,
        stereoFrom50[0][4] * stereoFrom50Normalization,
    },
    {
        stereoFrom50[1][0] * stereoFrom50Normalization,
        stereoFrom50[1][1] * stereoFrom50Normalization,
        stereoFrom50[1][2] * stereoFrom50Normalization,
        stereoFrom50[1][3] * stereoFrom50Normalization,
        stereoFrom50[1][4] * stereoFrom50Normalization,
    }
};

const float stereoFrom51[2][6] = {
    {
        1.0f,
        0.5f * YADAW::Util::sqrt2<float>(),
        0.0f,
        0.5f * YADAW::Util::sqrt3<float>(),
        0.5f,
        0.5f * YADAW::Util::sqrt2<float>()
    },
    {
        0.0f,
        0.5f * YADAW::Util::sqrt2<float>(),
        1.0f,
        0.5f,
        0.5f * YADAW::Util::sqrt3<float>(),
        0.5f * YADAW::Util::sqrt2<float>()
    }
};

const float stereoFrom51Normalization = 1.0f / (3.0f + YADAW::Util::sqrt3<float>() + 2 * YADAW::Util::sqrt2<float>());

const float stereoFrom51Normalized[2][6] = {
    {
        stereoFrom51[0][0] * stereoFrom51Normalization,
        stereoFrom51[0][1] * stereoFrom51Normalization,
        stereoFrom51[0][2] * stereoFrom51Normalization,
        stereoFrom51[0][3] * stereoFrom51Normalization,
        stereoFrom51[0][4] * stereoFrom51Normalization,
        stereoFrom51[0][5] * stereoFrom51Normalization
    },
    {
        stereoFrom51[1][0] * stereoFrom51Normalization,
        stereoFrom51[1][1] * stereoFrom51Normalization,
        stereoFrom51[1][2] * stereoFrom51Normalization,
        stereoFrom51[1][3] * stereoFrom51Normalization,
        stereoFrom51[1][4] * stereoFrom51Normalization,
        stereoFrom51[1][5] * stereoFrom51Normalization
    }
};

const float stereoFrom61[2][7] = {
    {
        1.0f,
        0.5f * YADAW::Util::sqrt2<float>(),
        0.0f,
        0.5f * YADAW::Util::sqrt3<float>(),
        0.5f,
        YADAW::Util::sqrt2<float>() * YADAW::Util::sqrt3<float>() * 0.25f,
        0.5f * YADAW::Util::sqrt2<float>()
    },
    {
        0.0f,
        0.5f * YADAW::Util::sqrt2<float>(),
        1.0f,
        0.5f,
        0.5f * YADAW::Util::sqrt3<float>(),
        YADAW::Util::sqrt2<float>() * YADAW::Util::sqrt3<float>() * 0.25f,
        0.5f * YADAW::Util::sqrt2<float>()
    }
};

const float stereoFrom61Normalization = 1.0f / (3.0f + 2.0f * (YADAW::Util::sqrt3<float>() + YADAW::Util::sqrt2<float>()));

const float stereoFrom61Normalized[2][7] = {
    {
        stereoFrom61[0][0] * stereoFrom61Normalization,
        stereoFrom61[0][1] * stereoFrom61Normalization,
        stereoFrom61[0][2] * stereoFrom61Normalization,
        stereoFrom61[0][3] * stereoFrom61Normalization,
        stereoFrom61[0][4] * stereoFrom61Normalization,
        stereoFrom61[0][5] * stereoFrom61Normalization,
        stereoFrom61[0][6] * stereoFrom61Normalization
    },
    {
        stereoFrom61[1][0] * stereoFrom61Normalization,
        stereoFrom61[1][1] * stereoFrom61Normalization,
        stereoFrom61[1][2] * stereoFrom61Normalization,
        stereoFrom61[1][3] * stereoFrom61Normalization,
        stereoFrom61[1][4] * stereoFrom61Normalization,
        stereoFrom61[1][5] * stereoFrom61Normalization,
        stereoFrom61[1][6] * stereoFrom61Normalization
    }
};

const float stereoFrom71[2][8] = {
    {
        1.0f,
        0.5f * YADAW::Util::sqrt2<float>(),
        0.0f,
        0.5f * YADAW::Util::sqrt3<float>(),
        0.5f,
        0.5f * YADAW::Util::sqrt3<float>(),
        0.5f,
        0.5f * YADAW::Util::sqrt2<float>()
    },
    {
        0.0f,
        0.5f * YADAW::Util::sqrt2<float>(),
        1.0f,
        0.5f,
        0.5f * YADAW::Util::sqrt3<float>(),
        0.5f,
        0.5f * YADAW::Util::sqrt3<float>(),
        0.5f * YADAW::Util::sqrt2<float>()
    }
};

const float stereoFrom71Normalization = 2.0f / (2.0f + YADAW::Util::sqrt2<float>() + YADAW::Util::sqrt3<float>());


const float stereoFrom71Normalized[2][8] = {
    {
        stereoFrom71[0][0] * stereoFrom71Normalization,
        stereoFrom71[0][1] * stereoFrom71Normalization,
        stereoFrom71[0][2] * stereoFrom71Normalization,
        stereoFrom71[0][3] * stereoFrom71Normalization,
        stereoFrom71[0][4] * stereoFrom71Normalization,
        stereoFrom71[0][5] * stereoFrom71Normalization,
        stereoFrom71[0][6] * stereoFrom71Normalization,
        stereoFrom71[0][7] * stereoFrom71Normalization
    },
    {
        stereoFrom71[1][0] * stereoFrom71Normalization,
        stereoFrom71[1][1] * stereoFrom71Normalization,
        stereoFrom71[1][2] * stereoFrom71Normalization,
        stereoFrom71[1][3] * stereoFrom71Normalization,
        stereoFrom71[1][4] * stereoFrom71Normalization,
        stereoFrom71[1][5] * stereoFrom71Normalization,
        stereoFrom71[1][6] * stereoFrom71Normalization,
        stereoFrom71[1][7] * stereoFrom71Normalization
    }
};
}
#endif //YADAW_SRC_AUDIO_UTIL_CHANNELGROUPREMIXER
