#include "audio/plugin/VestifalPlugin.hpp"
#include "audio/util/VestifalHelper.hpp"
#include "dao/PluginTable.hpp"
#include "native/Library.hpp"
#include "native/VestifalNative.hpp"
#include "test/common/PluginWindowThread.hpp"

#include <QGuiApplication>

#include <chrono>
#include <clocale>
#include <thread>

using namespace std::literals;

bool initializePlugin = true;
bool activatePlugin = true;
bool processPlugin = true;

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::printf("Usage: VestifalTest [record id in database]\nPress any key to continue...");
        getchar();
        return 0;
    }
    std::setlocale(LC_ALL, "en_US.UTF-8");
    int argIndex = 1;
    while(argIndex != argc)
    {
        int id = -1;
        std::sscanf(argv[argIndex], "%d", &id);
        ++argIndex;
        if(id != -1)
        {
            const auto& record = YADAW::DAO::selectPluginById(id);
            std::printf("\nTesting plugin: %ls...\n", reinterpret_cast<const wchar_t*>(record.name.data()));
            auto library = YADAW::Native::Library(record.path);
            {
                std::int32_t pluginId;
                std::memcpy(&pluginId, record.uid.data(), sizeof(pluginId));
                auto plugin = YADAW::Audio::Util::createVestifalFromLibrary(library, pluginId);
                if(initializePlugin)
                {
                    plugin.initialize(48000, 480);
                    if(activatePlugin)
                    {
                        plugin.activate();
                        if(processPlugin)
                        {
                            QGuiApplication application(argc, argv);
                            std::vector<std::uint32_t> inputCounts(plugin.audioInputGroupCount(), 1);
                            std::vector<std::uint32_t> outputCounts(plugin.audioOutputGroupCount(), 1);
                            std::vector<std::vector<float>> inputs;
                            std::vector<std::vector<float>> outputs;
                            std::vector<float*> inputsAsPtr;
                            std::vector<float*> outputsAsPtr;
                            inputs.resize(plugin.audioInputGroupCount(), std::vector<float>(480, 0.0f));
                            outputs.resize(plugin.audioOutputGroupCount(), std::vector<float>(480, 0.0f));
                            for(auto& input: inputs)
                            {
                                inputsAsPtr.emplace_back(input.data());
                            }
                            for(auto& output: outputs)
                            {
                                outputsAsPtr.emplace_back(output.data());
                            }
                            plugin.startProcessing();
                            PluginWindowThread pwt;
                            auto hasGUI = plugin.gui();
                            if(hasGUI)
                            {
                                pwt.start();
                                plugin.gui()->attachToWindow(pwt.window());
                                pwt.window()->show();
                                pwt.window()->setTitle(record.name);
                            }
                            std::atomic_bool run {true};
                            std::thread audioThread(
                                [&run, &plugin, &inputsAsPtr, &outputsAsPtr]
                                {
                                    while(run.load(std::memory_order::memory_order_acquire))
                                    {
                                        auto next = std::chrono::steady_clock::now() + 10ms;
                                        plugin.effect()->processReplacing(plugin.effect(), inputsAsPtr.data(), outputsAsPtr.data(), 480);
                                        while(std::chrono::steady_clock::now() < next)
                                        {
                                            std::this_thread::yield();
                                        }
                                    }
                                }
                            );
                            std::thread idleThread(
                                [&run, effect = plugin.effect()]
                                {
                                    while(run.load(std::memory_order::memory_order_acquire))
                                    {
                                        auto next = std::chrono::steady_clock::now() + 20ms;
                                        runDispatcher(effect, EffectOpcode::effectEditorIdle);
                                        std::this_thread::sleep_until(next);
                                    }
                                }
                            );
                            if(hasGUI)
                            {
                                QGuiApplication::exec();
                            }
                            if(!hasGUI)
                            {
                                std::printf("No GUI available!");
                                getchar();
                            }
                            run.store(false, std::memory_order::memory_order_release);
                            audioThread.join();
                            idleThread.join();
                            plugin.stopProcessing();
                        }
                        plugin.deactivate();
                    }
                    plugin.uninitialize();
                }
            }
        }
    }
    return 0;
}