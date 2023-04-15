#include "test/common/PluginWindowThread.hpp"

#include "audio/host/VST3Host.hpp"
#include "audio/plugin/VST3Plugin.hpp"
#include "native/Native.hpp"
#include "native/VST3Native.hpp"
#include "dao/PluginTable.hpp"
#include "native/Native.hpp"
#include "util/Constants.hpp"

#include <QGuiApplication>
#include <QScreen>
#include <QString>
#include <QWindow>

#include <atomic>
#include <chrono>
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

char tuid[16];
void testPlugin(YADAW::Audio::Plugin::VST3Plugin& plugin, bool initializePlugin, bool activatePlugin, bool processPlugin)
{
    if(initializePlugin)
    {
        assert(plugin.initialize(48000, 480));
        auto audioInputGroupCount = plugin.audioInputGroupCount();
        std::printf("%d audio input(s)", audioInputGroupCount);
        if(audioInputGroupCount != 0)
        {
            std::printf(":");
            for(int i = 0; i < audioInputGroupCount; ++i)
            {
                const auto& group = plugin.audioInputGroupAt(i);
                if(group->isMain())
                {
                    std::printf("\n> ");
                }
                else
                {
                    std::printf("\n  ");
                }
                std::printf(
                    "%d: %ls (%d channels)", i + 1, reinterpret_cast<wchar_t*>(group->name().data()),
                    static_cast<int>(group->channelCount()));
            }
        }
        auto audioOutputGroupCount = plugin.audioOutputGroupCount();
        std::printf("\n%d audio output(s)", audioOutputGroupCount);
        if(audioOutputGroupCount != 0)
        {
            std::printf(":");
            for(int i = 0; i < audioOutputGroupCount; ++i)
            {
                const auto& group = plugin.audioOutputGroupAt(i);
                if(group->isMain())
                {
                    std::printf("\n> ");
                }
                else
                {
                    std::printf("\n  ");
                }
                std::printf(
                    "%d: %ls (%d channels)", i + 1, reinterpret_cast<wchar_t*>(group->name().data()),
                    static_cast<int>(group->channelCount()));
            }
        }
        std::printf("\n");
        if(activatePlugin)
        {
            assert(plugin.activate());
            if(processPlugin)
            {
                assert(plugin.startProcessing());
                plugin.setProcessContext(&(YADAW::Audio::Host::VST3Host::processContext()));
                PluginWindowThread pluginWindowThread(nullptr);
                // Prepare audio process data {
                audioProcessData.singleBufferSize = 480;
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
                                    componentHandler->consumeOutputParameterChanges(YADAW::Native::currentTimeValueInNanosecond());
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
                        auto timestamp = YADAW::Native::currentTimeValueInNanosecond();
                        plugin.componentHandler()->switchBuffer(timestamp);
                        auto& processContext = YADAW::Audio::Host::VST3Host::processContext();
                        processContext.state = ProcessContext::StatesAndFlags::kSystemTimeValid | ProcessContext::StatesAndFlags::kPlaying;
                        processContext.sampleRate = 48000;
                        processContext.projectTimeSamples = 0;
                        while(!stop.load(std::memory_order::memory_order_acquire))
                        {
                            auto sleepTo = std::chrono::steady_clock::now() + std::chrono::milliseconds(10);
                            processContext.systemTime = timestamp;
                            plugin.process(audioProcessData);
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
            QGuiApplication application(argc, argv);
            library = YADAW::Native::Library(argv[argIndex]);
            ++argIndex;
            auto plugin = YADAW::Native::createVST3FromLibrary(library);
            if(std::strlen(argv[2]) == 32)
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
            library = YADAW::Native::Library(record.path);
            auto plugin = YADAW::Native::createVST3FromLibrary(library);
            std::memcpy(tuid, record.uid.data(), 16);
            assert(plugin.createPlugin(tuid));
            testPlugin(plugin, initializePlugin, activatePlugin, processPlugin);
            ++argIndex;
        }
    }
    std::wprintf(L"Press any key to continue...");
    getchar();
    return 0;
}
