#include "test/common/PluginWindowThread.hpp"

#include "CLAPPluginLatencyChangedCallback.hpp"

#include "audio/host/CLAPEventList.hpp"
#include "audio/host/CLAPHost.hpp"
#include "audio/plugin/CLAPPlugin.hpp"
#include "audio/util/CLAPHelper.hpp"
#include "native/CLAPNative.hpp"
#include "dao/PluginTable.hpp"
#include "native/Native.hpp"
#include "util/Constants.hpp"
#include "util/Util.hpp"

#include <QGuiApplication>
#include <QScreen>
#include <QString>
#include <QWindow>

#include <atomic>
#include <chrono>
#include <cmath>
#include <clocale>
#include <cstdio>
#include <thread>
#include <vector>

int inputIndex = -1;

int samplePosition = 0;

double pi = YADAW::Util::pi<float>();

double sampleRate = 0;

YADAW::Audio::Device::AudioProcessData<float> audioProcessData;

bool initializePlugin = true;
bool activatePlugin = true;
bool processPlugin = true;

void testPlugin(YADAW::Audio::Plugin::CLAPPlugin& plugin, bool initializePlugin, bool activatePlugin, bool processPlugin)
{
    YADAW::Audio::Host::CLAPEventList inputEventList;
    plugin.host().setMainThreadId(std::this_thread::get_id());
    if(initializePlugin)
    {
        auto bufferSize = 512;
        assert(plugin.initialize(44100, bufferSize));
        auto audioInputGroupCount = plugin.audioInputGroupCount();
        std::printf("%d audio input(s)", audioInputGroupCount);
        if(audioInputGroupCount != 0)
        {
            std::printf(":");
            for(int i = 0; i < audioInputGroupCount; ++i)
            {
                const auto& group = plugin.audioInputGroupAt(i)->get();
                if(group.isMain())
                {
                    std::printf("\n> ");
                }
                else
                {
                    std::printf("\n  ");
                }
                std::printf(
                    "%d: %s (%d channels)", i + 1, group.name().toLocal8Bit().data(),
                    static_cast<int>(group.channelCount()));
            }
        }
        auto audioOutputGroupCount = plugin.audioOutputGroupCount();
        std::printf("\n%d audio output(s)", audioOutputGroupCount);
        if(audioOutputGroupCount != 0)
        {
            std::printf(":");
            for(int i = 0; i < audioOutputGroupCount; ++i)
            {
                const auto& group = plugin.audioOutputGroupAt(i)->get();
                if(group.isMain())
                {
                    std::printf("\n> ");
                }
                else
                {
                    std::printf("\n  ");
                }
                std::printf(
                    "%d: %s (%d channels)", i + 1, group.name().toLocal8Bit().data(),
                    static_cast<int>(group.channelCount()));
            }
        }
        std::printf("\n");
        auto eventProcessor = plugin.eventProcessor();
        if(eventProcessor)
        {
            auto eventInputCount = eventProcessor->eventInputBusCount();
            std::printf("%u event input(s)", eventInputCount);
            if(eventInputCount != 0)
            {
                std::printf(":");
                for(std::uint32_t i = 0; i < eventInputCount; ++i)
                {
                    const auto& busInfo = eventProcessor->eventInputBusAt(i)->get();
                    if(busInfo.isMain())
                    {
                        std::printf("\n> ");
                    }
                    else
                    {
                        std::printf("\n  ");
                    }
                    std::printf("%u: %s (%u channels)", i + 1, busInfo.name().toLocal8Bit().data(),
                        busInfo.channelCount());
                }
            }
            auto eventOutputCount = eventProcessor->eventOutputBusCount();
            std::printf("\n%u event output(s)", eventOutputCount);
            if(eventOutputCount != 0)
            {
                std::printf(":");
                for(std::uint32_t i = 0; i < eventOutputCount; ++i)
                {
                    const auto& busInfo = eventProcessor->eventOutputBusAt(i)->get();
                    if(busInfo.isMain())
                    {
                        std::printf("\n> ");
                    }
                    else
                    {
                        std::printf("\n  ");
                    }
                    std::printf("%u: %s (%u channels)", i + 1, busInfo.name().toLocal8Bit().data(),
                        busInfo.channelCount());
                }
            }
            std::printf("\n");
        }
        if(activatePlugin)
        {
            assert(plugin.activate());
            CLAPPluginLatencyChangedCallback callback(plugin);
            plugin.host().latencyChanged(
                [&callback]()
                { callback.latencyChanged(); }
            );
            if(processPlugin)
            {
                PluginWindowThread pluginWindowThread(nullptr);
                inputEventList.attachToProcessData(plugin.processData());
                // Prepare audio process data {
                audioProcessData.singleBufferSize = bufferSize;
                // Inputs
                std::vector<std::vector<std::vector<float>>> idc1;
                std::vector<std::vector<float*>> idc2;
                std::vector<float**> idc3;
                std::vector<std::uint32_t> ic;
                idc1.resize(plugin.audioInputGroupCount());
                idc2.resize(idc1.size());
                idc3.resize(idc1.size(), nullptr);
                ic.resize(idc1.size(), -1);
                audioProcessData.inputGroupCount = idc1.size();
                for(int i = 0; i < idc1.size(); ++i)
                {
                    idc1[i].resize(plugin.audioInputGroupAt(i)->get().channelCount());
                    idc2[i].resize(idc1[i].size());
                    ic[i] = idc1[i].size();
                    for(int j = 0; j < idc1[i].size(); ++j)
                    {
                        idc1[i][j].resize(audioProcessData.singleBufferSize, 0.0f);
                        idc2[i][j] = idc1[i][j].data();
                        for(int k = 0; k < audioProcessData.singleBufferSize; ++k)
                        {
#if(__GNUC__)
                            idc1[i][j][k] = 0.25 * std::sin((i + 1) * k * 2 * pi / audioProcessData.singleBufferSize);
#else
                            idc1[i][j][k] = 0.25 * std::sinf((i + 1) * k * 2 * pi / audioProcessData.singleBufferSize);
#endif
                        }
                    }
                    idc3[i] = idc2[i].data();
                }
                audioProcessData.inputs = idc3.data();
                audioProcessData.inputCounts = ic.data();
                // Outputs
                std::vector<std::vector<std::vector<float>>> odc1;
                std::vector<std::vector<float*>> odc2;
                std::vector<float**> odc3;
                std::vector<std::uint32_t> oc;
                odc1.resize(plugin.audioOutputGroupCount());
                odc2.resize(odc1.size());
                odc3.resize(odc1.size(), nullptr);
                oc.resize(odc1.size(), -1);
                audioProcessData.outputGroupCount = odc1.size();
                for(int i = 0; i < odc1.size(); ++i)
                {
                    odc1[i].resize(plugin.audioOutputGroupAt(i)->get().channelCount());
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
                std::thread audioThread(
                    [bufferSize, &stop, &plugin]()
                    {
                        plugin.host().setAudioThreadId(std::this_thread::get_id());
                        assert(plugin.startProcessing());
                        // Audio callback goes here...
                        while(!stop.load(std::memory_order::memory_order_acquire))
                        {
                            auto sleepTo =
                                std::chrono::steady_clock::now() + std::chrono::microseconds(
                                    bufferSize * 10000 / 441);
                            plugin.process(audioProcessData);
                            while(std::chrono::steady_clock::now() < sleepTo)
                            {
                                std::this_thread::yield();
                            }
                        }
                        plugin.stopProcessing();
                    }
                );
                auto gui = plugin.pluginGUI();
                if(gui)
                {
                    pluginWindowThread.start();
                    pluginWindowThread.window()->showNormal();
                    gui->attachToWindow(pluginWindowThread.window());
                    QGuiApplication::exec();
                }
                else
                {
                    std::wprintf(L"No GUI available!");
                    getchar();
                }
                stop.store(true, std::memory_order::memory_order_release);
                audioThread.join();
                plugin.deactivate();
            }
        }
        plugin.uninitialize();
    }
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::printf("Usage: CLAPTest [<plugin path> <plugin UID>] or [<record id in database>]\nPress any key to continue...");
        getchar();
        return 0;
    }
    std::setlocale(LC_ALL, "en_US.UTF-8");
    int argIndex = 1;
    while(argIndex != argc)
    {
        YADAW::Native::Library library;
        int id = -1;
        std::sscanf(argv[argIndex], "%d", &id);
        if(id == -1)
        {
            std::printf("\nTesting plugin: %s...\n", argv[argIndex]);
            QGuiApplication application(argc, argv);
            library = YADAW::Native::Library(argv[argIndex]);
            ++argIndex;
            auto plugin = YADAW::Audio::Util::createCLAPFromLibrary(library);
            assert(plugin.createPlugin(argv[argIndex]));
            testPlugin(plugin, initializePlugin, activatePlugin, processPlugin);
            ++argIndex;
        }
        else
        {
            QGuiApplication application(argc, argv);
            const auto& record = YADAW::DAO::selectPluginById(id);
            std::printf("\nTesting plugin: %s...\n", record.path.toLocal8Bit().data());
            library = YADAW::Native::Library(record.path);
            auto plugin = YADAW::Audio::Util::createCLAPFromLibrary(library);
            assert(plugin.createPlugin(record.uid.data()));
            testPlugin(plugin, initializePlugin, activatePlugin, processPlugin);
            ++argIndex;
        }
    }
    std::printf("Press any key to continue...");
    getchar();
    return 0;
}
