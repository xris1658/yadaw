#include "audio/backend/AudioGraphBackend.hpp"
#include "audio/base/Gain.hpp"
#include "native/Native.hpp"
#include "util/Constants.hpp"

#include <QString>

#include <atomic>
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

std::uint64_t callbackDuration = 0;

double sampleRate = 0;

void audioGraphCallback(int inputCount, const AudioGraphBackend::InterleaveAudioBuffer* inputs,
    int outputCount, const AudioGraphBackend::InterleaveAudioBuffer* outputs)
{
    auto start = YADAW::Native::currentTimeValueInNanosecond();
    int frameCount = 0;
    int firstAvailableInput = -1;
    for(int i = 0; i < inputCount; ++i)
    {
        if(inputs[i].channelCount == 2)
        {
            firstAvailableInput = i;
            break;
        }
    }
    if(firstAvailableInput != -1)
    {
        for(int i = 0; i < outputCount; ++i)
        {
            const auto& output = outputs[i];
            frameCount = output.frameCount;
            for(int j = 0; j < output.channelCount; ++j)
            {
                for(int k = 0; k < output.frameCount; ++k)
                {
                    *reinterpret_cast<float*>(output.at(k, j)) = std::sinf((samplePosition + k) * 440 * 2 * pi / sampleRate) * 0.5 + *reinterpret_cast<float*>(inputs[firstAvailableInput].at(k, j)) * 0.5;
                }
            }
        }
    }
    else
    {
        for(int i = 0; i < outputCount; ++i)
        {
            const auto& output = outputs[i];
            frameCount = output.frameCount;
            for(int j = 0; j < output.channelCount; ++j)
            {
                for(int k = 0; k < output.frameCount; ++k)
                {
                    *reinterpret_cast<float*>(output.at(k, j)) = std::sinf((samplePosition + k) * 440 * 2 * pi / sampleRate);
                }
            }
        }
    }
    samplePosition += frameCount;
    callbackDuration = YADAW::Native::currentTimeValueInNanosecond() - start;
}

int main()
{
    std::setlocale(LC_ALL, "en_US.UTF-8");
    AudioGraphBackend audioGraphBackend;
    audioGraphBackend.initialize();
    auto outputDeviceCount = audioGraphBackend.audioOutputDeviceCount();
    auto defaultOutputDeviceIndex = audioGraphBackend.defaultAudioOutputDeviceIndex();
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
        if(defaultOutputDeviceIndex == i)
        {
            std::printf("O ");
        }
        else
        {
            std::printf("  ");
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
    sampleRate = audioGraphBackend.sampleRate();
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
    audioGraphBackend.activateDeviceInput(output - 1, true);
    audioGraphBackend.start(&audioGraphCallback);
    auto latencyInMilliseconds = audioGraphBackend.latencyInSamples() * 1000.0 / static_cast<double>(audioGraphBackend.sampleRate());
    auto bufferDuration = audioGraphBackend.bufferSizeInFrames() * 1000.0 / static_cast<double>(audioGraphBackend.sampleRate());
    std::printf("Latency: %d samples (%lf millisecond)\n", audioGraphBackend.latencyInSamples(), latencyInMilliseconds);
    std::printf("Buffer size: %d samples (%lf milliseconds)\n", audioGraphBackend.bufferSizeInFrames(), bufferDuration);
    std::getchar();
    std::atomic_bool stop; stop.store(false);
    std::thread([&stop, bufferDuration]()
    {
        while(!stop.load())
        {
            std::printf("%lf%%\n", callbackDuration / 1000000.0 / bufferDuration * 100.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }).detach();
    std::getchar();
    stop.store(true);
    audioGraphBackend.stop();
    audioGraphBackend.destroyAudioGraph();
    std::printf("Destroyed audio graph\n");
    return 0;
}
