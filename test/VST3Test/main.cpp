#include "test/common/PluginWindowThread.hpp"

#include "VST3PluginLatencyChangedCallback.hpp"

#include "audio/host/VST3Host.hpp"
#include "audio/plugin/VST3Plugin.hpp"
#include "audio/util/VST3Helper.hpp"
#include "native/VST3Native.hpp"
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

std::uint64_t callbackDuration = 0;

double sampleRate = 0;

YADAW::Audio::Device::AudioProcessData<float> audioProcessData;

bool initializePlugin = true;
bool activatePlugin = true;
bool processPlugin = true;

decltype(std::chrono::steady_clock::now() - std::chrono::steady_clock::now()) duration;

char tuid[16];
void testPlugin(YADAW::Audio::Plugin::VST3Plugin& plugin, bool initializePlugin, bool activatePlugin, bool processPlugin)
{
    if(initializePlugin)
    {
        assert(plugin.initialize(44100, 64));
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
                    "%d: %ls (%d channels)", i + 1, reinterpret_cast<wchar_t*>(group.name().data()),
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
                    "%d: %ls (%d channels)", i + 1, reinterpret_cast<wchar_t*>(group.name().data()),
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
                    const auto& busInfo = eventProcessor->eventInputBusAt(i);
                    if(busInfo->isMain())
                    {
                        std::printf("\n> ");
                    }
                    else
                    {
                        std::printf("\n  ");
                    }
                    std::printf("%u: %ls (%u channels)", i + 1, reinterpret_cast<wchar_t*>(busInfo->name().data()),
                        busInfo->channelCount());
                }
            }
            auto eventOutputCount = eventProcessor->eventOutputBusCount();
            std::printf("\n%u event output(s)", eventOutputCount);
            if(eventOutputCount != 0)
            {
                std::printf(":");
                for(std::uint32_t i = 0; i < eventOutputCount; ++i)
                {
                    const auto& busInfo = eventProcessor->eventOutputBusAt(i);
                    if(busInfo->isMain())
                    {
                        std::printf("\n> ");
                    }
                    else
                    {
                        std::printf("\n  ");
                    }
                    std::printf("%u: %ls (%u channels)", i + 1, reinterpret_cast<wchar_t*>(busInfo->name().data()),
                        busInfo->channelCount());
                }
            }
            std::printf("\n");
        }
        if(activatePlugin)
        {
            assert(plugin.activate());
            VST3PluginLatencyChangedCallback callback(plugin);
            plugin.componentHandler()->latencyChanged([&callback]() { callback.latencyChanged(); });
            if(processPlugin)
            {
                assert(plugin.startProcessing());
                plugin.setProcessContext(&(YADAW::Audio::Host::VST3Host::processContext()));
                PluginWindowThread pluginWindowThread(nullptr);
                // Prepare audio process data {
                audioProcessData.singleBufferSize = 64;
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
                            idc1[i][j][k] = 0.25 * std::sinf((i + 1) * k * 2 * pi / audioProcessData.singleBufferSize);
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
                auto gui = plugin.pluginGUI();
                if(gui)
                {
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
                    gui->attachToWindow(pluginWindowThread.window());
                    std::thread uiThread(
                        [&stop, &plugin]()
                        {
                            while(!stop.load(std::memory_order::memory_order_acquire))
                            {
                                if(auto componentHandler = plugin.componentHandler(); componentHandler)
                                {
                                    componentHandler->consumeOutputParameterChanges(YADAW::Util::currentTimeValueInNanosecond());
                                }
                                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                            }
                        }
                    );
                    uiThread.detach();
                }
                std::thread audioThread(
                    [&stop, &plugin]()
                    {
                        using Steinberg::Vst::ProcessContext;
                        auto timestamp = YADAW::Util::currentTimeValueInNanosecond();
                        auto& processContext = YADAW::Audio::Host::VST3Host::processContext();
                        processContext.state = ProcessContext::StatesAndFlags::kSystemTimeValid | ProcessContext::StatesAndFlags::kPlaying;
                        processContext.sampleRate = 44100;
                        processContext.projectTimeSamples = 0;
                        // Audio callback goes here...
                        while(!stop.load(std::memory_order::memory_order_acquire))
                        {
                            auto componentHandler = plugin.componentHandler();
                            if(!componentHandler)
                            {
                                std::wprintf(L"Component handler is not available!\n");
                                break;
                            }
                            componentHandler->switchBuffer(timestamp);
                            auto sleepTo = std::chrono::steady_clock::now() + std::chrono::microseconds (64 * 10000 / 441);
                            processContext.systemTime = timestamp;
                            auto start = std::chrono::steady_clock::now();
                            plugin.process(audioProcessData);
                            duration = std::chrono::steady_clock::now() - start;
                            processContext.projectTimeSamples += audioProcessData.singleBufferSize;
                            while(std::chrono::steady_clock::now() < sleepTo)
                            {
                                std::this_thread::yield();
                            }
                        }
                    }
                );
                if(gui)
                {
                    QGuiApplication::exec();
                }
                else
                {
                    std::wprintf(L"No GUI available!");
                    getchar();
                }
                stop.store(true, std::memory_order::memory_order_release);
                audioThread.join();
                plugin.stopProcessing();
            }
            plugin.deactivate();
        }
        plugin.uninitialize();
    }
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::printf("Usage: VST3Test [<plugin path> <plugin UID>] or [<record id in database>]\nPress any key to continue...");
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
            auto plugin = YADAW::Audio::Util::createVST3FromLibrary(library);
            if(std::strlen(argv[argIndex]) == 32)
            {
                for(int i = 0; i < 16; ++i)
                {
                    tuid[i] = 0;
                    unsigned char value = argv[argIndex][i * 2] > '9'? argv[argIndex][i * 2] - 'A' + 10: argv[argIndex][i * 2] - '0';
                    value *= 16;
                    value += argv[argIndex][i * 2 + 1] > '9'? argv[argIndex][i * 2 + 1] - 'A' + 10: argv[argIndex][i * 2 + 1] - '0';
                    std::memcpy(tuid + i, &value, 1);
                }
            }
            assert(plugin.createPlugin(tuid));
            testPlugin(plugin, initializePlugin, activatePlugin, processPlugin);
            ++argIndex;
        }
        else
        {
            QGuiApplication application(argc, argv);
            const auto& record = YADAW::DAO::selectPluginById(id);
            std::printf("\nTesting plugin: %ls...\n", reinterpret_cast<const wchar_t*>(record.path.data()));
            library = YADAW::Native::Library(record.path);
            auto plugin = YADAW::Audio::Util::createVST3FromLibrary(library);
            std::memcpy(tuid, record.uid.data(), 16);
            assert(plugin.createPlugin(tuid));
            testPlugin(plugin, initializePlugin, activatePlugin, processPlugin);
            ++argIndex;
        }
        std::printf("Duration: %lld\n", duration.count());
    }
    std::wprintf(L"Press any key to continue...");
    getchar();
    return 0;
}
