#include "audio/backend/CoreAudioBackend.hpp"

#include "util/IntegerRange.hpp"

#include <QString>

#include <cinttypes>
#include <cstdint>
#include <cstdio>

void printSampleRateRanges(
    const std::vector<YADAW::Audio::Backend::CoreAudioBackend::SampleRateRange>& ranges,
    FILE* stream = stdout)
{
    for(const auto& range: ranges)
    {
        if (range.first == range.second)
        {
            std::fprintf(stream, "%lf, ", range.first);;
        }
        else
        {
            std::fprintf(stream, "%lf - %lf, ", range.first, range.second);
        }
    }
}

int main()
{
    using YADAW::Audio::Backend::CoreAudioBackend;
    auto inputCount = CoreAudioBackend::audioInputDeviceCount();
    auto outputCount = CoreAudioBackend::audioOutputDeviceCount();
    std::printf("%" PRIu32" input devices\n", inputCount);
    std::printf("%" PRIu32" output devices\n", outputCount);
    std::printf("Input Devices:\n");
    auto defaultInput = CoreAudioBackend::defaultInputDevice();
    auto defaultOutput = CoreAudioBackend::defaultOutputDevice();
    char defaultIndicators[2] = {' ', '>'};
    FOR_RANGE0(i, inputCount)
    {
        auto [id, name] = *CoreAudioBackend::audioInputDeviceAt(i);
        auto localName = name.toLocal8Bit();
        std::printf("  %c #%" PRIu32": %s\n", defaultIndicators[id == defaultInput], i + 1, localName.data());
        std::printf("        Sample Rate: %lf\n", CoreAudioBackend::deviceNominalSampleRate(true, id).value_or(0.00));
        auto sampleRateRanges = CoreAudioBackend::deviceAvailableNominalSampleRates(true, id)
        .value_or(std::vector<CoreAudioBackend::SampleRateRange>());
        std::printf("        Sample Rate Range: ");
        printSampleRateRanges(sampleRateRanges, stdout);
        std::printf("\n");
        std::printf("        Channel Counts: ");
        auto channelCounts = CoreAudioBackend::deviceAvailableChannelCounts(true, id).value_or(std::vector<std::uint32_t>());
        for(auto channelCount: channelCounts)
        {
            std::printf("%" PRIu32", ", channelCount);
        }
        std::printf("\n");
    }
    std::printf("Output Devices:\n");
    FOR_RANGE0(i, outputCount)
    {
        auto [id, name] = *CoreAudioBackend::audioOutputDeviceAt(i);
        auto localName = name.toLocal8Bit();
        std::printf("  %c #%" PRIu32": %s\n", defaultIndicators[id == defaultOutput], i + 1, localName.data());
        std::printf("        Sample Rate: %lf\n", CoreAudioBackend::deviceNominalSampleRate(false, id).value_or(0.00));
        auto sampleRateRanges = CoreAudioBackend::deviceAvailableNominalSampleRates(false, id)
        .value_or(std::vector<CoreAudioBackend::SampleRateRange>());
        std::printf("        Sample Rate Range: ");
        printSampleRateRanges(sampleRateRanges, stdout);
        std::printf("\n");
        std::printf("        Channel Counts: ");
        auto channelCounts = CoreAudioBackend::deviceAvailableChannelCounts(false, id).value_or(std::vector<std::uint32_t>());
        for(auto channelCount: channelCounts)
        {
            std::printf("%" PRIu32", ", channelCount);
        }
        std::printf("\n");
    }
}