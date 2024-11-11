#include "audio/host/EventFileDescriptorSupport.hpp"
#include "audio/host/CLAPHost.hpp"
#include "audio/plugin/CLAPPlugin.hpp"
#include "audio/util/AudioProcessDataPointerContainer.hpp"
#include "dao/PluginTable.hpp"
#include "native/Library.hpp"
#include "util/AlignHelper.hpp"
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

#include "audio/host/CLAPEventList.hpp"


struct PluginRuntime
{
    YADAW::Util::AlignedStorage<YADAW::Audio::Host::CLAPHost> host;
    std::unique_ptr<YADAW::Audio::Plugin::CLAPPlugin> plugin;
    std::atomic_flag runAudioThread;
    std::thread audioThread;
    YADAW::Audio::Host::CLAPHost& createHost()
    {
        auto ptr = new(&host) YADAW::Audio::Host::CLAPHost(*plugin);
        return *ptr;
    }
    void finish()
    {
        if(auto gui = plugin->pluginGUI())
        {
            gui->detachWithWindow();
        }
        runAudioThread.clear();
        audioThread.join();
        plugin->deactivate();
        plugin->uninitialize();
        plugin.reset();
        using YADAW::Audio::Host::CLAPHost;
        auto ptr = reinterpret_cast<YADAW::Audio::Host::CLAPHost*>(&host);
        ptr->~CLAPHost();
    }
};

PluginRuntime runtime;

std::unique_ptr<YADAW::Audio::Plugin::CLAPPlugin> createPlugin(YADAW::Native::Library& library)
{
    if(auto entry = library.getExport<const clap_plugin_entry*>("clap_entry"))
    {
        return std::make_unique<YADAW::Audio::Plugin::CLAPPlugin>(entry, library.path());
    }
    return nullptr;
}

YADAW::Native::Library createPluginFromArgs(int& argIndex, char* argv[], QWindow& pluginWindow)
{
    const char* uid = nullptr;
    YADAW::Native::Library library;
    int id = -1;
    std::string uidAsString;
    std::sscanf(argv[argIndex], "%d", &id);
    if(id == -1)
    {
        std::printf("\nTesting plugin: %s...\n", argv[argIndex]);
        library = YADAW::Native::Library(argv[argIndex]);
        ++argIndex;
        uid = argv[argIndex];
        ++argIndex;
    }
    else
    {
        const auto& record = YADAW::DAO::selectPluginById(id);
        std::printf("\nTesting plugin: %ls...\n", record.path.toStdWString().data());
        library = YADAW::Native::Library(record.path);
        uidAsString = record.uid.data();
    }
    runtime.plugin = createPlugin(library);
    if(!runtime.plugin)
    {
        throw std::runtime_error("Error: Init plugin library failed");
    }
    if(!runtime.plugin->createPlugin(uid? uid: uidAsString.data()))
    {
        runtime.plugin.reset();
        throw std::runtime_error("Error: Create plugin failed!");
    }
    auto factory = runtime.plugin->factory();
    FOR_RANGE0(i, factory->get_plugin_count(factory))
    {
        auto desc = factory->get_plugin_descriptor(factory, i);
        if(std::strcmp(desc->id, uid? uid: uidAsString.data()))
        {
            pluginWindow.setTitle(QString::fromUtf8(desc->name));
            break;
        }
    }
    ++argIndex;
    return library;
}

void latencyChanged(YADAW::Audio::Plugin::CLAPPlugin& plugin)
{
    std::printf("New latency: %" PRIu32 "\n", plugin.latencyInSamples());
}

void testPlugin(QWindow& pluginWindow)
{
    auto& plugin = *runtime.plugin;
    auto sampleRate = 48000;
    auto bufferSize = 480;
    auto& host = runtime.createHost();
    host.setMainThreadId(std::this_thread::get_id());
    if(plugin.initialize(sampleRate, bufferSize))
    {
        if(plugin.activate())
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
                    pluginWindow.setGeometry(
                        (rect.width() - pluginWindow.width()) / 2,
                        (rect.height() - pluginWindow.height()) / 2,
                        pluginWindow.width(),
                        pluginWindow.height()
                    );
                }
                YADAW::UI::setWindowResizable(pluginWindow, gui->resizableByUser());
            }
            std::vector<std::uint32_t> inputCounts;
            auto inputGroupCount = plugin.audioInputGroupCount();
            inputCounts.reserve(inputGroupCount);
            FOR_RANGE0(i, inputGroupCount)
            {
                inputCounts.emplace_back(plugin.audioInputGroupAt(i)->get().channelCount());
            }
            std::vector<std::uint32_t> outputCounts;
            auto outputGroupCount = plugin.audioOutputGroupCount();
            outputCounts.reserve(outputGroupCount);
            FOR_RANGE0(i, outputGroupCount)
            {
                outputCounts.emplace_back(plugin.audioOutputGroupAt(i)->get().channelCount());
            }
            runtime.audioThread = std::thread([&plugin, &host, sampleRate, bufferSize, inputCounts = std::move(inputCounts), outputCounts = std::move(outputCounts)
                ]()
            {
                host.setAudioThreadId(std::this_thread::get_id());
                YADAW::Audio::Host::CLAPEventList events;
                events.attachToProcessData(plugin.processData());
                plugin.startProcessing();
                runtime.runAudioThread.test_and_set(std::memory_order_acq_rel);
                YADAW::Audio::Util::AudioProcessDataPointerContainer<float> container;
                container.setSingleBufferSize(bufferSize);
                std::vector<std::vector<std::vector<float>>> input;
                input.reserve(inputCounts.size());
                std::vector<std::vector<std::vector<float>>> output;
                output.reserve(outputCounts.size());
                container.setInputGroupCount(inputCounts.size());
                FOR_RANGE0(i, inputCounts.size())
                {
                    auto& input2 = input.emplace_back();
                    auto inputCount = inputCounts[i];
                    container.setInputCount(i, inputCount);
                    input2.resize(inputCount, std::vector<float>(bufferSize, 0.0f));
                    FOR_RANGE0(j, inputCount)
                    {
                        container.setInput(i, j, input2[j].data());
                    }
                }
                container.setOutputGroupCount(outputCounts.size());
                FOR_RANGE0(i, outputCounts.size())
                {
                    auto& output2 = output.emplace_back();
                    auto outputCount = outputCounts[i];
                    container.setOutputCount(i, outputCount);
                    output2.resize(outputCount, std::vector<float>(bufferSize, 0.0f));
                    FOR_RANGE0(j, outputCount)
                    {
                        container.setOutput(i, j, output2[j].data());
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