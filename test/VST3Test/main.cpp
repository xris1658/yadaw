#include "audio/host/EventFileDescriptorSupport.hpp"
#include "audio/host/VST3ComponentHandler.hpp"
#include "audio/plugin/VST3Plugin.hpp"
#include "audio/util/AudioProcessDataPointerContainer.hpp"
#include "audio/util/VST3Helper.hpp"
#include "dao/PluginTable.hpp"
#include "ui/Runtime.hpp"
#include "ui/UI.hpp"

#include "test/common/DisableStreamBuffer.hpp"
#include "test/common/CloseWindowEventFilter.hpp"

#include <QGuiApplication>
#include <QScreen>

#include <atomic>
#include <cinttypes>
#include <memory>
#include <thread>

struct PluginRuntime
{
    std::unique_ptr<YADAW::Audio::Plugin::VST3Plugin> plugin;
    std::atomic_flag runAudioThread;
    std::thread audioThread;
    void finish()
    {
        if(auto gui = plugin->pluginGUI())
        {
            gui->detachWithWindow();
        }
        runAudioThread.clear();
        audioThread.join();
        plugin->stopProcessing();
        plugin->deactivate();
        plugin->uninitialize();
        plugin->destroyPlugin();
        plugin.reset();
    }
};

PluginRuntime runtime;

std::unique_ptr<YADAW::Audio::Plugin::VST3Plugin> createPlugin(YADAW::Native::Library& library)
{
    auto factory = library.getExport<YADAW::Audio::Plugin::VST3Plugin::FactoryEntry>("GetPluginFactory");
    if(!factory)
    {
        return std::unique_ptr<YADAW::Audio::Plugin::VST3Plugin>();
    }
    auto init = library.getExport<YADAW::Native::VST3InitEntry>(YADAW::Native::initEntryName);
    auto exit = library.getExport<YADAW::Audio::Plugin::VST3Plugin::ExitEntry>(YADAW::Native::exitEntryName);
    return std::make_unique<YADAW::Audio::Plugin::VST3Plugin>(init, factory, exit, library.handle());
}

YADAW::Native::Library createPluginFromArgs(int& argIndex, char* argv[], QWindow& pluginWindow)
{
    Steinberg::TUID tuid;
    YADAW::Native::Library library;
    int id = -1;
    std::sscanf(argv[argIndex], "%d", &id);
    if(id == -1)
    {
        std::printf("\nTesting plugin: %s...\n", argv[argIndex]);
        library = YADAW::Native::Library(argv[argIndex]);
        ++argIndex;
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
        ++argIndex;
    }
    else
    {
        const auto& record = YADAW::DAO::selectPluginById(id);
        std::printf("\nTesting plugin: %ls...\n", record.path.toStdWString().data());
        library = YADAW::Native::Library(record.path);
        std::memcpy(tuid, record.uid.data(), 16);
    }
    auto plugin = createPlugin(library);
    if(!plugin)
    {
        throw std::runtime_error("Error: Init plugin library failed");
    }
    if(!plugin->createPlugin(tuid))
    {
        throw std::runtime_error("Error: Create plugin failed!");
    }
    auto factory = plugin->factory();
    auto classCount = factory->countClasses();
    for(int i = 0; i < classCount; ++i)
    {
        Steinberg::PClassInfo classInfo;
        auto result = factory->getClassInfo(i, &classInfo);
        if(result == Steinberg::kResultOk
           // TUID is not null-termiated C-style string
           // do not use std::strcmp; use std::memcmp instead
           && std::memcmp(tuid, classInfo.cid, 16) == 0)
        {
            pluginWindow.setTitle(QString::fromLocal8Bit(classInfo.name));
            break;
        }
    }
    runtime.plugin = std::move(plugin);
    ++argIndex;
    return library;
}

void latencyChanged(YADAW::Audio::Plugin::VST3Plugin& plugin)
{
    std::printf("New latency: %" PRIu32 "\n", plugin.latencyInSamples());
}

void testPlugin(QWindow& pluginWindow)
{
    auto& plugin = *runtime.plugin;
    auto sampleRate = 48000;
    auto bufferSize = 480;
    if(plugin.initialize(sampleRate, bufferSize))
    {
        YADAW::Audio::Host::VST3ComponentHandler componentHandler(plugin);
        componentHandler.setLatencyChangedCallback(&latencyChanged);
        if(plugin.activate())
        {
            if(plugin.startProcessing())
            {
                pluginWindow.show();
                auto gui = plugin.pluginGUI();
                if(gui)
                {
                    gui->attachToWindow(&pluginWindow);
                    pluginWindow.show();
                    pluginWindow.setFlags(
                        Qt::WindowType::Dialog |
                        Qt::WindowType::CustomizeWindowHint |
                        Qt::WindowType::WindowTitleHint |
                        Qt::WindowType::WindowCloseButtonHint
                    );
                    if(auto* screen = pluginWindow.screen())
                    {
                        auto rect = screen->availableVirtualGeometry();
                        pluginWindow.setPosition(
                            (rect.width() - pluginWindow.width()) / 2,
                            (rect.height() - pluginWindow.height()) / 2
                        );
                    }
                    YADAW::UI::setWindowResizable(pluginWindow, gui->resizableByUser());
                }
                runtime.audioThread = std::thread([&plugin, sampleRate, bufferSize]()
                {
                    runtime.runAudioThread.test_and_set(std::memory_order_acq_rel);
                    YADAW::Audio::Util::AudioProcessDataPointerContainer<float> container;
                    container.setSingleBufferSize(bufferSize);
                    std::vector<std::vector<std::vector<float>>> input;
                    std::vector<std::vector<std::vector<float>>> output;
                    auto inputGroupCount = plugin.audioInputGroupCount();
                    container.setInputGroupCount(inputGroupCount);
                    input.resize(inputGroupCount);
                    FOR_RANGE0(i, inputGroupCount)
                    {
                        auto inputCount = plugin.audioInputGroupAt(i)->get().channelCount();
                        container.setInputCount(i, inputCount);
                        input[i].resize(inputCount, std::vector<float>(bufferSize, 0.0f));
                        FOR_RANGE0(j, inputCount)
                        {
                            container.setInput(i, j, input[i][j].data());
                        }
                    }
                    auto outputGroupCount = plugin.audioOutputGroupCount();
                    container.setOutputGroupCount(outputGroupCount);
                    output.resize(outputGroupCount);
                    FOR_RANGE0(i, outputGroupCount)
                    {
                        auto outputCount = plugin.audioOutputGroupAt(i)->get().channelCount();
                        container.setOutputCount(i, outputCount);
                        output[i].resize(outputCount, std::vector<float>(bufferSize, 0.0f));
                        FOR_RANGE0(j, outputCount)
                        {
                            container.setOutput(i, j, output[i][j].data());
                        }
                    }
                    while(runtime.runAudioThread.test_and_set(std::memory_order_acquire))
                    {
                        auto due = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000 * bufferSize / sampleRate);
                        plugin.process(container.audioProcessData());
                        while(std::chrono::steady_clock::now() < due)
                        {
                            std::this_thread::yield();
                        }
                    }
                    plugin.stopProcessing();
                    runtime.runAudioThread.clear();
                });
            }
        }
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
    QGuiApplication application(argc, argv);
#if __linux__
    auto& efds = YADAW::Audio::Host::EventFileDescriptorSupport::instance();
    auto& timer = YADAW::UI::idleProcessTimer();
    efds.start(timer);
#endif
    QWindow pluginWindow;
    std::setlocale(LC_ALL, "en_US.UTF-8");
    int argIndex = 1;
    YADAW::Native::Library library;
    CloseWindowEventFilter closeWindowEventFilter;
    QObject::connect(
        &closeWindowEventFilter, &CloseWindowEventFilter::aboutToClose,
        [&]() mutable
        {
            runtime.finish();
            if(argIndex == argc)
            {
                closeWindowEventFilter.setClose(true);
            }
            else
            {
                closeWindowEventFilter.setClose(false);
                pluginWindow.hide();
                library = createPluginFromArgs(argIndex, argv, pluginWindow);
                testPlugin(pluginWindow);
            }
        }
    );
    library = createPluginFromArgs(argIndex, argv, pluginWindow);
    testPlugin(pluginWindow);
    closeWindowEventFilter.setWindow(pluginWindow);
    auto ret = application.exec();
#if __linux__
    efds.stop();
#endif
    return ret;
}
