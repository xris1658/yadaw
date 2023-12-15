#include "test/common/PluginWindowThread.hpp"
#include "test/common/DisableStreamBuffer.hpp"
#include "VST3DoubleBuffer.hpp"
#include "VST3PluginLatencyChangedCallback.hpp"

#include "audio/host/VST3ComponentHandler.hpp"
#include "audio/host/VST3EventDoubleBuffer.hpp"
#include "audio/host/VST3Host.hpp"
#include "audio/plugin/VST3Plugin.hpp"
#include "audio/util/VST3Helper.hpp"
#include "midi/MessageToVST3Event.hpp"
#include "midi/MIDIInputDevice.hpp"
#include "native/VST3Native.hpp"
#include "dao/PluginTable.hpp"
#include "native/Native.hpp"
#include "util/Constants.hpp"
#include "util/Util.hpp"
#include "util/AtomicMutex.hpp"

#include <pluginterfaces/vst/ivstprocesscontext.h>

#include <QGuiApplication>
#include <QScreen>
#include <QString>
#include <QTimer>
#include <QWindow>

#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <clocale>
#include <cstdio>
#include <mutex>
#include <thread>
#include <vector>

int inputIndex = -1;

int samplePosition = 0;

double pi = YADAW::Util::pi<float>();

std::uint64_t callbackDuration = 0;

YADAW::Audio::Device::AudioProcessData<float> audioProcessData;

bool initializePlugin = true;
bool activatePlugin = true;
bool processPlugin = true;

Steinberg::Vst::ProcessContext processContext;

char tuid[16];

const std::uint32_t sampleRate = 48000;

const std::uint32_t bufferSize = 480;

std::int64_t audioCallbackTime = 0;

YADAW::Util::AtomicMutex audioCallbackTimeMutex;

YADAW::Audio::Host::VST3EventDoubleBuffer doubleBuffer;

std::uint64_t eventCount = 0;

std::uint64_t underflowEventCount = 0;

std::uint64_t overflowEventCount = 0;

std::int32_t maxOverflow = 0;

std::int32_t maxUnderflow = 0;

YADAW::Util::AtomicMutex mutex;

std::int64_t translateEventDuration = 0;

void translateEvent(const YADAW::MIDI::MIDIInputDevice& device, const YADAW::MIDI::Message& message)
{
    using namespace Steinberg::Vst;
    auto now = YADAW::Util::currentTimeValueInNanosecond();
    if(auto optionalEvent = doubleBuffer.emplaceInputEvent(); optionalEvent.has_value())
    {
        ++eventCount;
        auto& vst3Event = optionalEvent->get();
        YADAW::MIDI::fillVST3EventFromMessage(message, vst3Event);
        vst3Event.busIndex = 0;
        vst3Event.flags = Event::EventFlags::kIsLive;
        vst3Event.ppqPosition = 0;
        {
            std::lock_guard<YADAW::Util::AtomicMutex> lg(audioCallbackTimeMutex);
            auto& sampleOffset = vst3Event.sampleOffset;
            sampleOffset = (message.timestampInNanoseconds - audioCallbackTime) * sampleRate / 1000000000;
            bool rectified = false;
            if(sampleOffset < 0)
            {
                maxUnderflow = std::min(maxUnderflow, sampleOffset);
                sampleOffset = 0;
                rectified = true;
                ++underflowEventCount;
            }
            if(sampleOffset >= bufferSize)
            {
                maxOverflow = std::max(maxOverflow, sampleOffset - static_cast<std::int32_t>(bufferSize) - 1);
                sampleOffset = bufferSize - 1;
                rectified = true;
                ++overflowEventCount;
            }
        }
    }
    translateEventDuration = std::max(translateEventDuration, YADAW::Util::currentTimeValueInNanosecond() - now);
}

void testPlugin(YADAW::Audio::Plugin::VST3Plugin& plugin, bool initializePlugin, bool activatePlugin, bool processPlugin)
{
    using YADAW::MIDI::MIDIInputDevice;
    auto inputCount = MIDIInputDevice::inputDeviceCount();
    std::printf("%lld input devices\n", inputCount);
    for(decltype(inputCount) i = 0; i < inputCount; ++i)
    {
        const auto& device = MIDIInputDevice::inputDeviceAt(i);
        std::printf("  %lld: %ls\n",
            i + 1,
            device->name.toStdWString().data()
        );
    }
    int index = -1;
    while(true)
    {
        std::scanf("%d", &index);
        if(index > 0 && index <= inputCount)
        {
            break;
        }
    }
    getchar();
    YADAW::MIDI::MIDIInputDevice device(MIDIInputDevice::inputDeviceAt(index - 1)->id);
    if(initializePlugin)
    {
        YADAW::Audio::Host::VST3ComponentHandler componentHandler(plugin);
        if(!plugin.initialize(sampleRate, bufferSize))
        {
            throw std::runtime_error("Initialization failed!");
        }
        std::vector<YADAW::Audio::Base::ChannelGroupType> inputChannels(plugin.audioInputGroupCount(), YADAW::Audio::Base::ChannelGroupType::eStereo);
        std::vector<YADAW::Audio::Base::ChannelGroupType> outputChannels(plugin.audioOutputGroupCount(), YADAW::Audio::Base::ChannelGroupType::eStereo);
        plugin.setChannelGroups(inputChannels.data(), inputChannels.size(), outputChannels.data(), outputChannels.size());
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
                    "%d: %ls (%d channels)", i + 1, group.name().toStdWString().data(),
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
                    "%d: %ls (%d channels)", i + 1, group.name().toStdWString().data(),
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
                    std::printf("%u: %ls (%u channels)", i + 1, busInfo.name().toStdWString().data(),
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
                    std::printf("%u: %ls (%u channels)", i + 1, busInfo.name().toStdWString().data(),
                        busInfo.channelCount());
                }
            }
            std::printf("\n");
        }
        if(activatePlugin)
        {
            if(!plugin.activate())
            {
                throw std::runtime_error("Activation failed!");
            }
            VST3PluginLatencyChangedCallback callback(plugin);
            componentHandler.latencyChanged([&callback]() { callback.latencyChanged(); });
            if(processPlugin)
            {
                if(!plugin.startProcessing())
                {
                    throw std::runtime_error("Start processing failed!");
                }
                plugin.setProcessContext(processContext);
                PluginWindowThread pluginWindowThread(nullptr);
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
                auto gui = plugin.pluginGUI();
                if(gui)
                {
                    pluginWindowThread.start();
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
                    pluginWindowThread.window()->showNormal();
                }
                QTimer timer;
                timer.setInterval(std::chrono::milliseconds(10));
                timer.setSingleShot(false);
                timer.callOnTimeout(
                    [&componentHandler]()
                    {
                        componentHandler.consumeOutputParameterChanges(YADAW::Util::currentTimeValueInNanosecond());
                    });
                timer.start();
                std::thread audioThread(
                    [&stop, &plugin, &componentHandler]()
                    {
                        using Steinberg::Vst::ProcessContext;
                        processContext.state = ProcessContext::StatesAndFlags::kSystemTimeValid | ProcessContext::StatesAndFlags::kPlaying;
                        processContext.sampleRate = sampleRate;
                        processContext.projectTimeSamples = 0;
                        decltype(std::chrono::steady_clock::now()) sleepTo;
                        // Audio callback goes here...
                        while(!stop.load(std::memory_order::memory_order_acquire))
                        {
                            auto now = std::chrono::steady_clock::now();
                            {
                                std::lock_guard<YADAW::Util::AtomicMutex> lg(audioCallbackTimeMutex);
                                audioCallbackTime = now.time_since_epoch().count();
                                componentHandler.switchBuffer(audioCallbackTime);
                                {
                                    std::lock_guard<YADAW::Util::AtomicMutex> lg(mutex);
                                    doubleBuffer.switchBuffer();
                                }
                                processContext.systemTime = audioCallbackTime;
                            }
                            auto [pi, po] = doubleBuffer.pluginSideEventList();
                            plugin.setEventList(&pi, &po);
                            sleepTo = now + std::chrono::microseconds(static_cast<std::int64_t>(bufferSize * 1000000 / static_cast<double>(sampleRate)));
                            plugin.process(audioProcessData);
                            processContext.projectTimeSamples += audioProcessData.singleBufferSize;
                            while(std::chrono::steady_clock::now() < sleepTo)
                            {
                                std::this_thread::yield();
                            }
                        }
                    }
                );
                device.start(&translateEvent);
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
                timer.stop();
                device.stop();
            }
            plugin.deactivate();
        }
        plugin.uninitialize();
    }
}

int main(int argc, char* argv[])
{
    disableStdOutBuffer();
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
            if(!plugin.createPlugin(tuid))
            {
                throw std::runtime_error("Error: Create plugin failed!");
            }
            testPlugin(plugin, initializePlugin, activatePlugin, processPlugin);
            ++argIndex;
        }
        else
        {
            QGuiApplication application(argc, argv);
            const auto& record = YADAW::DAO::selectPluginById(id);
            std::printf("\nTesting plugin: %ls...\n", record.path.toStdWString().data());
            library = YADAW::Native::Library(record.path);
            auto plugin = YADAW::Audio::Util::createVST3FromLibrary(library);
            std::memcpy(tuid, record.uid.data(), 16);
            if(!plugin.createPlugin(tuid))
            {
                throw std::runtime_error("Error: Create plugin failed!");
            }
            testPlugin(plugin, initializePlugin, activatePlugin, processPlugin);
            ++argIndex;
        }
    }
    std::printf("%llu events (%llu overflow, %llu underflow) are rectified in %llu events.\n",
        overflowEventCount + underflowEventCount,
        overflowEventCount,
        underflowEventCount,
        eventCount);
    std::printf("Max underflow: %d\n", maxUnderflow);
    std::printf("Max overflow:  %d\n", maxOverflow);
    std::printf("Max event translation duration: %lld\n", translateEventDuration);
    std::printf("Press <ENTER> to continue...");
    getchar();
    return 0;
}
