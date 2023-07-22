#include "audio/util/SampleDelay.hpp"

#include "audio/device/IAudioChannelGroup.hpp"

class MonoAudioChannelGroup: public YADAW::Audio::Device::IAudioChannelGroup
{
public:
    MonoAudioChannelGroup(const QString& name): name_(name) {}
public:
    virtual QString name() const override
    {
        return name_;
    }

    virtual uint32_t channelCount() const override
    {
        return 1;
    }

    virtual YADAW::Audio::Base::ChannelGroupType type() const override
    {
        return YADAW::Audio::Base::ChannelGroupType::eMono;
    }

    virtual YADAW::Audio::Base::ChannelType speakerAt(std::uint32_t index) const override
    {
        return index == 0?
            YADAW::Audio::Base::ChannelType::Center:
            YADAW::Audio::Base::ChannelType::Invalid;
    }

    virtual QString speakerNameAt(std::uint32_t index) const override
    {
        return index == 0?
            "Center":
            QString();
    }

    virtual bool isMain() const override
    {
        return true;
    }

private:
    QString name_;
};

int main()
{
    std::vector<float> buffer1{6.0f, 7.0f, 8.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    std::vector<float> buffer2(buffer1.size(), 0.0f);
    YADAW::Audio::Util::SampleDelay pdc(5, MonoAudioChannelGroup("Mono"));
    pdc.startProcessing();
    YADAW::Audio::Device::AudioProcessData<float> audioProcessData;
    audioProcessData.singleBufferSize = buffer1.size();
    audioProcessData.inputGroupCount = 1;
    audioProcessData.outputGroupCount = 1;
    std::uint32_t inputCount = 1;
    std::uint32_t outputCount = 1;
    audioProcessData.inputCounts = &inputCount;
    audioProcessData.outputCounts = &outputCount;
    auto inputPtr = buffer1.data();
    auto inputPtrPtr = &inputPtr;
    auto outputPtr = buffer1.data();
    auto outputPtrPtr = &outputPtr;
    audioProcessData.inputs = &inputPtrPtr;
    audioProcessData.outputs = &outputPtrPtr;
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < buffer1.size(); ++j)
        {
            buffer1[j] = j + 1;
        }
        pdc.process(audioProcessData);
        for(auto value: buffer1)
        {
            std::printf("%f, ", value);
        }
        std::printf("\n");
    }
    std::printf("\n");
    pdc.stopProcessing();
    pdc.setDelay(4);
    pdc.startProcessing();
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < buffer1.size(); ++j)
        {
            buffer1[j] = j + 1;
        }
        pdc.process(audioProcessData);
        for(auto value: buffer1)
        {
            std::printf("%f, ", value);
        }
        std::printf("\n");
    }
    std::printf("\n");
    pdc.stopProcessing();
    pdc.setDelay(16);
    pdc.startProcessing();
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < buffer1.size(); ++j)
        {
            buffer1[j] = j + 1;
        }
        pdc.process(audioProcessData);
        for(auto value: buffer1)
        {
            std::printf("%f, ", value);
        }
        std::printf("\n");
    }
    std::printf("\n");
    pdc.stopProcessing();
    pdc.setDelay(0);
    pdc.startProcessing();
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < buffer1.size(); ++j)
        {
            buffer1[j] = j + 1;
        }
        pdc.process(audioProcessData);
        for(auto value: buffer1)
        {
            std::printf("%f, ", value);
        }
        std::printf("\n");
    }
    std::printf("\n");
    pdc.stopProcessing();
    pdc.setDelay(4);
    pdc.startProcessing();
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < buffer1.size(); ++j)
        {
            buffer1[j] = j + 1;
        }
        pdc.process(audioProcessData);
        for(auto value: buffer1)
        {
            std::printf("%f, ", value);
        }
        std::printf("\n");
    }
    std::printf("\n");
}