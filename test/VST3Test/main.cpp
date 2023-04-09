#include "test/common/PluginWindowThread.hpp"

#include "audio/backend/AudioGraphBackend.hpp"
#include "audio/plugin/VST3Plugin.hpp"
#include "native/VST3Native.hpp"
#include "audio/base/Gain.hpp"
#include "native/Native.hpp"
#include "util/Constants.hpp"

#include <QGuiApplication>
#include <QScreen>
#include <QString>
#include <QWindow>

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

YADAW::Audio::Plugin::VST3Plugin* pPlugin;

YADAW::Audio::Device::AudioProcessData<float> audioProcessData;

void audioGraphCallback(int inputCount, const AudioGraphBackend::InterleaveAudioBuffer* inputs,
    int outputCount, const AudioGraphBackend::InterleaveAudioBuffer* outputs)
{
    auto start = YADAW::Native::currentTimeValueInNanosecond();
    pPlugin->componentHandler()->switchBuffer(start);
    for(int i = 0; i < audioProcessData.inputGroupCount; ++i)
    {
        for(int j = 0; j < audioProcessData.inputCounts[i]; ++j)
        {
            for(int k = 0; k < audioProcessData.singleBufferSize; ++k)
            {
                audioProcessData.inputs[i][j][k] = std::sinf((samplePosition + k) * 440 * (i + 1) * 2 * pi / sampleRate) * 0.125;
            }
        }
    }
    pPlugin->process(audioProcessData);
    int frameCount = 0;
    for(int i = 0; i < outputCount; ++i)
    {
        const auto& output = outputs[i];
        frameCount = output.frameCount;
        for(int j = 0; j < output.channelCount; ++j)
        {
            for(int k = 0; k < output.frameCount; ++k)
            {
                *reinterpret_cast<float*>(output.at(k, j)) = audioProcessData.outputs[0][j % audioProcessData.outputCounts[0]][k];
                // *reinterpret_cast<float*>(output.at(k, j)) = *reinterpret_cast<float*>(input.at(k, j));
            }
        }
    }
    samplePosition += frameCount;
    callbackDuration = YADAW::Native::currentTimeValueInNanosecond() - start;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        std::printf("Usage: VST3Test <plugin path> <plugin UID>\nPress any key to continue...");
        getchar();
        return 0;
    }
    std::setlocale(LC_ALL, "en_US.UTF-8");
    AudioGraphBackend audioGraphBackend;
    if(audioGraphBackend.initialize())
    {
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
        sampleRate = audioGraphBackend.sampleRate();
        std::printf("Created audio graph at %lf Hz\n", sampleRate);
        if(output == 0)
        {
            auto currentOutputDevice = audioGraphBackend.currentOutputDevice();
            std::wprintf(L"Current device: %s\n",
                reinterpret_cast<const wchar_t*>(currentOutputDevice.name.data()));
        }
        QGuiApplication application(argc, argv);
        YADAW::Native::Library library(argv[1]);
        auto plugin = YADAW::Native::createVST3FromLibrary(library);
        pPlugin = &plugin;
        char tuid[16];
        if(std::strlen(argv[2]) == 32)
        {
            for(int i = 0; i < 16; ++i)
            {
                tuid[i] = 0;
                unsigned char value = argv[2][i * 2] > '9'? argv[2][i * 2] - 'A' + 10: argv[2][i * 2] - '0';
                value *= 16;
                value += argv[2][i * 2 + 1] > '9'? argv[2][i * 2 + 1] - 'A' + 10: argv[2][i * 2 + 1] - '0';
                std::memcpy(tuid + i, &value, 1);
            }
        }
        assert(plugin.createPlugin(tuid));
        assert(plugin.initialize(audioGraphBackend.sampleRate(), audioGraphBackend.bufferSizeInFrames()));
        assert(plugin.activate());
        assert(plugin.startProcessing());
        PluginWindowThread pluginWindowThread(nullptr);
        pluginWindowThread.start();
        pluginWindowThread.window()->showNormal();
        auto factory = plugin.factory();
        auto classCount = plugin.factory()->countClasses();
        for(int i = 0; i < classCount; ++i)
        {
            Steinberg::PClassInfo classInfo;
            auto result = factory->getClassInfo(i, &classInfo);
            if(result == Steinberg::kResultOk
               // TUID is not null-termiated C-style string
               // do not use std::strcmp; use std::memcmp instead
               && std::memcmp(tuid, classInfo.cid, 16) == 0)
            {
                pluginWindowThread.window()->setTitle(QString::fromLocal8Bit(classInfo.name));
                break;
            }
        }
        // Prepare audio process data {
        audioProcessData.singleBufferSize = audioGraphBackend.bufferSizeInFrames();
        // Inputs
        std::vector<std::vector<std::vector<float>>> idc1;
        std::vector<std::vector<float*>> idc2;
        std::vector<float**> idc3;
        std::vector<int> ic;
        idc1.resize(plugin.audioInputGroupCount());
        idc2.resize(idc1.size());
        idc3.resize(idc1.size(), nullptr);
        ic.resize(idc1.size(), -1);
        audioProcessData.inputGroupCount = idc1.size();
        for(int i = 0; i < idc1.size(); ++i)
        {
            idc1[i].resize(plugin.audioInputGroupAt(i)->channelCount());
            idc2[i].resize(idc1[i].size());
            ic[i] = idc1[i].size();
            for(int j = 0; j < idc1[i].size(); ++j)
            {
                idc1[i][j].resize(audioProcessData.singleBufferSize, 0.0f);
                idc2[i][j] = idc1[i][j].data();
            }
            idc3[i] = idc2[i].data();
        }
        audioProcessData.inputs = idc3.data();
        audioProcessData.inputCounts = ic.data();
        // Outputs
        std::vector<std::vector<std::vector<float>>> odc1;
        std::vector<std::vector<float*>> odc2;
        std::vector<float**> odc3;
        std::vector<int> oc;
        odc1.resize(plugin.audioOutputGroupCount());
        odc2.resize(odc1.size());
        odc3.resize(odc1.size(), nullptr);
        oc.resize(odc1.size(), -1);
        audioProcessData.outputGroupCount = odc1.size();
        for(int i = 0; i < odc1.size(); ++i)
        {
            odc1[i].resize(plugin.audioOutputGroupAt(i)->channelCount());
            odc2[i].resize(odc1[i].size());
            oc[i] = odc1[i].size();
            for(int j = 0; j < odc1[i].size(); ++j)
            {
                odc1[i][j].resize(audioProcessData.singleBufferSize, 0.0f);
                odc2[i][j] = odc1[i][j].data();
            }
            odc3[i] = odc2[i].data();
        }
        audioProcessData.outputs = odc3.data();
        audioProcessData.outputCounts = oc.data();
        // } prepare audio process data
        std::atomic_bool stop;
        stop.store(false);
        plugin.pluginGUI()->attachToWindow(pluginWindowThread.window());
        audioGraphBackend.start(&audioGraphCallback);
        std::thread uiThread([&stop, &plugin]()
        {
            while(!stop.load(std::memory_order::memory_order_acquire))
            {
                plugin.componentHandler()->consumeOutputParameterChanges();
                std::this_thread::sleep_for(std::chrono::milliseconds(33));
            }
        });
        auto latencyInMilliseconds =
            audioGraphBackend.latencyInSamples() * 1000.0 / static_cast<double>(audioGraphBackend.sampleRate());
        auto bufferDuration =
            audioGraphBackend.bufferSizeInFrames() * 1000.0 / static_cast<double>(audioGraphBackend.sampleRate());
        std::printf("Latency: %d samples (%lf millisecond)\n", audioGraphBackend.latencyInSamples(), latencyInMilliseconds);
        std::printf("Buffer size: %d samples (%lf milliseconds)\n", audioGraphBackend.bufferSizeInFrames(), bufferDuration);
        QGuiApplication::exec();
        stop.store(true, std::memory_order::memory_order_release);
        uiThread.join();
        audioGraphBackend.stop();
        plugin.stopProcessing();
        plugin.deactivate();
        plugin.uninitialize();
        audioGraphBackend.destroyAudioGraph();
        std::printf("Destroyed audio graph\n");
        audioGraphBackend.uninitialize();
    }
    return 0;
}
