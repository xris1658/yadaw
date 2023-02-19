#include "audio/backend/AudioGraphBackend.hpp"
#include "audio/base/Gain.hpp"
#include "util/Constants.hpp"

#include <QString>

#include <chrono>
#include <clocale>
#include <cmath>
#include <cstdio>
#include <thread>
#include <vector>

using YADAW::Audio::Backend::AudioGraphBackend;

int inputIndex = -1;

int samplePosition = 0;

double pi = YADAW::Util::pi<float>();

void audioGraphCallback(int inputCount, const AudioGraphBackend::InterleaveAudioBuffer* inputs,
    int outputCount, const AudioGraphBackend::InterleaveAudioBuffer* outputs)
{
    int frameCount = 0;
    const auto& input = inputs[0];
    for(int i = 0; i < outputCount; ++i)
    {
        const auto& output = outputs[i];
        frameCount = output.frameCount;
        for(int j = 0; j < output.channelCount; ++j)
        {
            for(int k = 0; k < output.frameCount; ++k)
            {
                *reinterpret_cast<float*>(output.at(k, j)) = std::sinf((samplePosition + k) * 440 * 2 * pi / 44100.0);
            }
        }
    }
    samplePosition += frameCount;
}

int main()
{
    std::setlocale(LC_ALL, "en_US.UTF-8");
    AudioGraphBackend audioGraphBackend;
    auto outputDeviceCount = audioGraphBackend.audioOutputDeviceCount();
    std::vector<QString> ids;
    for(decltype(outputDeviceCount) i = 0; i < outputDeviceCount; ++i)
    {
        auto info = audioGraphBackend.audioOutputDeviceAt(i);
        if(info.isEnabled)
        {
            std::printf("  ");
        }
        else
        {
            std::printf("X ");
        }
        std::wprintf(L"%d: %s\n", (i + 1),
            reinterpret_cast<const wchar_t*>(info.name.data()));
        ids.emplace_back(info.id);
    }
    int output = -1;
    while(1)
    {
        std::printf("Select primary output device (0 for default output): ");
        if(std::scanf("%d", &output) != 1 || output < 0 || output > outputDeviceCount)
        {
            std::printf("The input is invalid. Please input again.\n");
            continue;
        }
        break;
    }
    bool createAudioGraphResult = false;
    if(output == 0)
    {
        createAudioGraphResult = audioGraphBackend.createAudioGraph();
    }
    else
    {
        createAudioGraphResult = audioGraphBackend.createAudioGraph(ids[output - 1]);
    }
    if(!createAudioGraphResult)
    {
        std::printf("Create audio graph failed!\n");
        return 0;
    }
    auto sampleRate = audioGraphBackend.sampleRate();
    std::printf("Created audio graph at %u Hz\n", sampleRate);
    if(output == 0)
    {
        auto currentOutputDevice = audioGraphBackend.currentOutputDevice();
        std::wprintf(L"Current device: %s\n",
            reinterpret_cast<const wchar_t*>(currentOutputDevice.name.data()));
    }
    output = -1;
    auto inputDeviceCount = audioGraphBackend.audioInputDeviceCount();
    ids.clear();
    for(decltype(inputDeviceCount) i = 0; i < inputDeviceCount; ++i)
    {
        auto info = audioGraphBackend.audioInputDeviceAt(i);
        if(info.isEnabled)
        {
            std::printf("  ");
        }
        else
        {
            std::printf("X ");
        }
        std::wprintf(L"%d: %s\n", (i + 1),
            reinterpret_cast<const wchar_t*>(info.name.data()));
        ids.emplace_back(info.id);
    }
    while(1)
    {
        std::printf("Select primary input device: ");
        if(std::scanf("%d", &output) != 1 || output < 0 || output > inputDeviceCount)
        {
            std::printf("The input is invalid. Please input again.\n");
            continue;
        }
        break;
    }
    inputIndex = audioGraphBackend.enableDeviceInput(ids[output - 1]);
    if(inputIndex != -1)
    {
        audioGraphBackend.start(&audioGraphCallback);
        auto latencyInMilliseconds = audioGraphBackend.latencyInSamples() * 1000.0 / static_cast<double>(audioGraphBackend.sampleRate());
        auto bufferDuration = audioGraphBackend.bufferSizeInFrames() * 1000.0 / static_cast<double>(audioGraphBackend.sampleRate());
        std::printf("Latency: %d samples (%lf millisecond)\n", audioGraphBackend.latencyInSamples(), latencyInMilliseconds);
        std::printf("Buffer size: %d samples (%lf milliseconds)\n", audioGraphBackend.bufferSizeInFrames(), bufferDuration);
        std::getchar();
        std::getchar();
        audioGraphBackend.stop();
        audioGraphBackend.destroyAudioGraph();
        std::printf("Destroyed audio graph\n");
        return 0;
    }
    std::printf("Cannot create input device!\n");
    audioGraphBackend.destroyAudioGraph();
    std::printf("Destroyed audio graph\n");
    return 0;
}
